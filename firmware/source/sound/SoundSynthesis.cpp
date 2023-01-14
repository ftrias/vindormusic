/**
 * @file SoundSynthesis.cpp
 * @author Fernando Trias
 * @brief 
 * @version 0.1
 * @date 
 * 
 * @copyright Copyright (c) 2013-2021 by Vindor Music, Inc.
 * 
 */

#include "firmware.h"
#include "fdebug.h"

#include <IntervalTimer.h>

#include "Settings.h"
#include "SynthSettings.h"
#include "SoundSynthesis.h"
#include "Display.h"
#include "Lights.h"


#ifdef USE_SDCARD
#include "SdLib.h"
#endif

#include "vdebug.h"

#include <Audio.h>

#include "SoundModelAnalog.h"
#include "SoundModelSax.h"
#include "SoundModelPhysical.h"
#include "SoundModelSD.h"
#include "SoundModelCV.h"

float SoundSynthesis::invert = 1.0F;

/********
 * 
 * Sound modeling and generation
 *
 */

SoundModelAnalog model_analog;
SoundModelSax model_sax;
SoundModelPhysical model_physical;
SoundModelSD model_sd;
#if defined(USE_SIMPLE_CV) || defined(USE_EXTERNAL_CV)
SoundModelCV model_cv;
#endif
SoundModel *model = &model_sax;

#define USE_USBAUDIO_IN
#define USE_USBAUDIO_OUT

#ifdef USE_USBAUDIO_IN
AudioInputUSB usbIn;
#endif

#ifdef USE_USBAUDIO_OUT
AudioOutputUSB usbOut;
#endif

#ifdef USE_SGTL5000
#include <AudioControlSGTL5000Vindor.h>
AudioControlSGTL5000Vindor codec;
AudioOutputI2S    dac;
#else
AudioOutputAnalog dac;
#endif

AudioEffectDeclick declick;
AudioMixer4 mixer_models;
AudioMixer4 mixer_synth;
AudioMixer4 mixer_audio_R;
AudioMixer4 mixer_audio_L;
AudioMixer4 mixer_signal_R;
AudioMixer4 mixer_signal_L;

AudioEffectTransform trans_L;
AudioEffectTransform trans_R;

AudioConnection patchCord_physical_model(model_physical.getOutput(), 0, mixer_models, 0);
AudioConnection patchCord_analog_model(model_analog.getOutput(), 0, mixer_models, 1);
AudioConnection patchCord_wavetable_model(model_sax.getOutput(), 0, mixer_models, 2);
#ifdef USE_SDCARD
AudioConnection patchCord_sd_model(model_sd.getOutput(), 0, mixer_models, 3);
#endif

AudioConnection cx(mixer_models, 0, mixer_synth, 0);

#ifdef USE_AUDIO_MEM
AudioPlayMem audio_mem;
AudioConnection cw3(audio_mem, 0, mixer_synth, 3);
#endif

AudioConnection fpatchCord18a(mixer_synth, 0, declick, 0);

AudioConnection cm0R(declick, 0, mixer_audio_R, 0);
AudioConnection cm0L(declick, 0, mixer_audio_L, 0);

#ifdef USE_ADD_NOISE
AudioSynthNoiseWhite     usb_noise;
AudioConnection cm3(usb_noise, 0, mixer_audio_R, 2);
AudioConnection cm4(usb_noise, 0, mixer_audio_L, 2);
#endif

#ifdef USE_USBAUDIO_IN
AudioConnection cm2(usbIn, 0, mixer_audio_R, 1);
AudioConnection cm1(usbIn, 1, mixer_audio_L, 1);
#endif

#ifdef USE_SGTL5000
AudioConnection trL(mixer_audio_L, 0, trans_L, 0); // stereo
AudioConnection trR(mixer_audio_R, 0, trans_R, 0); // stereo

#ifdef USE_SIMPLE_CV
AudioConnection cv0R(model_cv.getFrequencyOutput(), 0, mixer_signal_L, 1);
AudioConnection cv0L(model_cv.getVolumeOutput(), 0, mixer_signal_R, 1);
#endif

AudioConnection coxL(trans_L, 0, mixer_signal_L, 0); // stereo
AudioConnection coxR(trans_R, 0, mixer_signal_R, 0);

#ifdef AUDIO_RIGHLEFT_REVERSE_BUG
AudioConnection coL(mixer_signal_L, 0, dac, 1); // stereo
AudioConnection coR(mixer_signal_R, 0, dac, 0);
#else
AudioConnection coL(mixer_signal_L, 0, dac, 0); // stereo
AudioConnection coR(mixer_signal_R, 0, dac, 1);
#endif

#else
AudioConnection trL(mixer_audio_L, 0, mixer_signal_L, 0); // stereo
AudioConnection coL(mixer_signal_L, dac); // mono
#endif

#ifdef USE_USBAUDIO_OUT
AudioConnection coUL(declick, 0, usbOut, 0); // usb, only route our own sound
AudioConnection coUR(declick, 0, usbOut, 1);
#endif

/******
 * 
 * Generation functions
 * 
 */

/**
 * @brief Return numeric code to display in debug info
 * 
 * @param id 1 -> sgtl5000 id
 * @return int Code to display
 */
int SoundSynthesis::getDebug(int id) {
    switch(id) {
        case 1: return codec.getId();
    }
    return 0;
}

/**
 * @brief Construct a new Sound Synthesis:: Sound Synthesis object
 * 
 */
SoundSynthesis::SoundSynthesis() {
    sound = &(settings.soundList[0]);
}

/**
 * @brief For a given MIDI note, return the frequency
 *
 * @param note MIDI note
 * @param part Fractional note number (-1 to +1)
 * @return     frequency in Hz.
 */
#if defined(USE_SIMPLE_CV) || defined(USE_EXTERNAL_CV)
float SoundSynthesis::midi2freq_hz(int note, float part) {
#else
float SoundSynthesis::midi2freq(int note, float part) {
#endif
    // const float freqEqual[] = {
    //     440.0, 466.1637615180899, 493.8833012561241, 523.2511306011972, 554.3652619537442, 587.3295358348151, 
    //     622.2539674441618, 659.2551138257398, 698.4564628660078, 739.9888454232688, 783.9908719634985, 830.6093951598903
    // };
    // const int i2pow[] = {1, 2, 4, 8, 16, 32, 64};
    // const float diffBagpipe[] = {
    //     -26.0, -28.836238481910073, -30.116698743875872, -35.74886939880275, -28.634738046255848, -33.67046416518485, 
    //     -37.746032555838156, -39.74488617426016, -47.54353713399223, -37.0111545767312, -44.009128036501465, -49.390604840109745
    // };
    // const float diffJust[] {
    //     3.94, -4.77, 3.33, 0.0, 9.28, -1.34, -5.68, 5.2, 0.8, 4.14, -0.88, -6.6
    // };
    // const float diffMeantone[] {
    //     0.0, 3.84, -1.96, 2.67, -1.32, 1.16, 2.93, -1.30, 4.66, 04.40, 2.78, 0.97
    // };

    float freqR = pow(2.0F,(((float)note)-57.0F+part)/12.0F);
    return freqR * settings.baseFreqA4;

    // const float *table;

    // switch(temperament) {
    //     case EQUAL_TEMPERAMENT:
    //         return freqR * settings.baseFreqA4;
    //     case BAGPIPE_TEMPERAMENT:
    //         table = diffBagpipe;
    //         break;
    //     case JUST_TEMPERAMENT:
    //         table = diffJust;
    //         break;
    //     case MEANTONE_TEMPERAMENT:
    //         table = diffMeantone;
    //         break;
    // }

    // int oct = (note-69)/12;
    // int base = (note-69)%12;
    // float adj = table[base] / 440.0;
    // if (oct < 0) {
    //     adj /= i2pow[-oct];
    // }
    // else {
    //     adj *= i2pow[oct];
    // }

    // return (freqR + adj) *  settings.baseFreqA4;
}

#if defined(USE_SIMPLE_CV) || defined(USE_EXTERNAL_CV)

/**
 * @brief Convert a MIDI note number to a CV voltage
 * 
 * @param note MIDI note number
 * @param part Fractional note number (-1 to +1)
 * @param type 1:Volt/Oct; 2:Hz/Oct
 * @return float Voltage
 */
float SoundSynthesis::midi2freq_cv(int note, float part, int type) {
    // returns volts desired
    switch(type) {
        // Volt/oct is 0V = 220Hz, 1V = 440Hz, 2V = 880Hz, 3V = 1760Hz
        case 1: return (float(note-36)+part)/12.0F;
        // Hz/oct, 0V = 220Hz, 1V = 440Hz, 2V = 880Hz, 4V = 1760Hz
        case 2: return pow(2.0F,(float(note-45)+part)/12.0F);
        default: return 0.0F;
    }
}

/**
 * @brief Convert MIDI note to frequency (or CV volt)
 * 
 * @param note MIDI note
 * @param part Fractional part (-1 to +1)
 * @return float Frequency (or V)
 */
float SoundSynthesis::midi2freq(int note, float part) {
    if (useCV) {
        return midi2freq_cv(note, part, settings.cv_type);
    }
    else {
        return midi2freq_hz(note, part);
    }
}

#endif

/**
 * @brief Disable the onboard codec chip
 * 
 */
void SoundSynthesis::disable() {
#ifdef USE_SGTL5000
    codec.disable();
#endif
}

/**
 * @brief Put codec to sleep
 * 
 * @param sleep true if sleep mode; false if wake mode
 */
void SoundSynthesis::sleepState(int sleep) {
#ifdef USE_SGTL5000
    if (sleep) codec.disable();
#endif
}

/**
 * @brief Change volume up or down on the codec
 * 
 * @param change Fractional increment/decrement
 * @return float New volume (0.0 to 1.0)
 */
float SoundSynthesis::deltaCodecVolume(float change) {
    settings.hpVolume += change;
    if (settings.hpVolume < 0.0) settings.hpVolume = 0.0;
    else if (settings.hpVolume > 1.0) settings.hpVolume = 1.0;
    return setCodecVolume(settings.hpVolume);
}

/**
 * @brief Set codec volume
 * 
 * @param vol Volume (0.0 to 1.0)
 * @return float Passed volume
 */
float SoundSynthesis::setCodecVolume(float vol) {
#ifdef USE_SGTL5000
    if (vol < 0.0) vol = 0.0;
    else if (vol > 1.0) vol = 1.0;
    codec.volume(vol);
#endif
    lastCodecVolume = vol;
    return vol;
}

/**
 * @brief Turn headphone power on or off
 * 
 * @param state 
 */
void SoundSynthesis::powerHeadphone(bool state) {
#ifdef USE_SGTL5000
    //codec.powerHeadphone(state);
    if (state) {
        // codec.powerHeadphone(true);
        setCodecVolume(settings.hpVolume);
        codec.unmuteHeadphone();
    }
    else {
        // codec.powerHeadphone(false);
        codec.muteHeadphone();
    }
#endif
}

/**
 * @brief Turn line out power on or off
 * 
 * @param state 
 */
void SoundSynthesis::powerLineout(bool state) {
#ifdef USE_SGTL5000
    if (state) {
        // codec.powerLineout(true);
        codec.unmuteLineout();
    }
    else {
        // codec.powerLineout(false);
        codec.muteLineout();
    }
#endif
}

#ifdef USE_SIMPLE_CV
/**
 * @brief Set the CV voltage on or off
 * 
 * @param state 
 */
void SoundSynthesis::setCV(bool state) {
    useCV = state;
    setCodecVolume(state ? cv_vol: settings.hpVolume);
    resetFilter();
}
#endif

#ifdef USE_EXTERNAL_CV
/**
 * @brief Set the CV voltage on or off
 * 
 * @param state 
 */
void SoundSynthesis::setCV(bool state) {
    useCV = state;
    model_cv.setOneWire(state);
    resetFilter();
}
#endif

/**
 * @brief Return a fancy name for the instrument (look up SD names)
 * 
 * @param s Buffer to write name to
 * @param inst Instrument number (0-9)
 * @return int 
 */
int SoundSynthesis::getNameForInstrument(char *s, int inst) {
    return model_sd.getNameForInstrument(s, inst);
}

#ifdef USE_SD_DIR

int SoundSynthesis::loadDirList(char *dest) {
    return model_sd.loadDirList(dest);
}

int SoundSynthesis::setSDInst(int entry) {
    settings.currentInstrument = 0;
    model_sd.setDirNum(entry);
    initializeSound();
    return 0;
}

#endif

/**
 * @brief Change current instrument and initialize it.
 * 
 * @param inst Instrument number
 * @return Midi instrument number, if any
 */
int SoundSynthesis::setInstrument(int inst) {
    settings.currentInstrument = inst;
    sound = &settings.soundList[inst];
    model_sd.setPrefix(inst);
    initializeSound();
    return sound->midiNumber;
}

const float attack_factor = 1.5F;

/**
 * @brief Change the volume
 *
 * @param volume volume from 0 to scalevolume
 */
void SoundSynthesis::changeVolume(int volume) {
    playVolume = (float)volume / (float)scalevolume;
    model->setVolume(playVolume);
}

/**
 * @brief Convert input linearly to a new scale
 * 
 * @param input Input number
 * @param minI Minimum input scale
 * @param maxI Max input scale
 * @param minO Min output scale
 * @param maxO Max output scale
 * @return float Converted number
 */
float mapParam(float input, float minI, float maxI, float minO, float maxO) {
    return minO + ((maxO - minO)/(maxI - minI)) * (input - minI);
}

/**
 * @brief Reset default filters and instrument parameters. Called when changing instruments.
 * 
 */
void SoundSynthesis::resetFilter() {
  AudioNoInterrupts();

#ifdef USE_SIMPLE_CV
    if (useCV) { 
        model = &model_cv;
        cv_out_vol.setV(0);
        cv_out_freq.setF(0);
        patchCord_analog_model.disconnect();
        patchCord_physical_model.disconnect();
        patchCord_physical_model.disconnect();
        patchCord_sd_model.disconnect();
    }
    else
#endif
    
#ifdef USE_EXTERNAL_CV
    if (useCV) { 
        model = &model_cv;
        patchCord_analog_model.disconnect();
        patchCord_wavetable_model.disconnect();
        patchCord_physical_model.disconnect();
        patchCord_sd_model.disconnect();
    }
    else
#endif

    if (sdcard) {
        model = &model_sd;
        model_sd.resetFilter(sound);
        mixer_models.gain(3, 1.0);
        patchCord_analog_model.disconnect();
        patchCord_wavetable_model.disconnect();
        patchCord_physical_model.disconnect();
        patchCord_sd_model.connect();
    }

    else if (sound->synth_use == PHYSICAL_MODEL) {
        model = &model_physical;
        model_physical.resetFilter(sound);
        mixer_models.gain(0, 1.0);
        patchCord_analog_model.disconnect();
        patchCord_wavetable_model.disconnect();
        patchCord_physical_model.connect();
        patchCord_sd_model.disconnect();
    }

    else if (sound->synth_use == ANALOG_MODEL) {
        model = &model_analog;
        model_analog.resetFilter(sound);
        mixer_models.gain(1, 1.0);
        patchCord_physical_model.disconnect();
        patchCord_wavetable_model.disconnect();
        patchCord_analog_model.connect();
        patchCord_sd_model.disconnect();
    }

    else if (sound->synth_use == WAVETABLE_MODEL) {
    }
    
    else if (sound->synth_use == WAVESF2_MODEL) {
        model = &model_sax;
        model_sax.resetFilter(sound);
        mixer_models.gain(2, 1.0);
        patchCord_analog_model.disconnect();
        patchCord_wavetable_model.connect();
        patchCord_physical_model.disconnect();
        patchCord_sd_model.disconnect();
    }

    AudioInterrupts();
}

/**
 * @brief Set the frequency of playing note
 * 
 * @param freq Frequency
 * @param nointerrupts true to suspend interrupts
 */
void SoundSynthesis::setFreq(float freq, bool nointerrupts) {
  if (nointerrupts) AudioNoInterrupts();

#ifdef USE_SIMPLE_CV
  if (useCV) {
    // output of the headphones is range is -1.65V to +1.65V
    // so we output scaled value that should be amplified
    // 10x so 0.1V -> 1V, 0.2V -> 2V
    // float level = freq / 16.50F;
    cv_out_freq.setF(freq);
    // debugOutputDecimal(freq * 1000);
  }
  else 
#endif
  {
    model->setFreq(playFreq);
  }

  if (nointerrupts) AudioInterrupts();
}

/**
 * @brief Called to restart an existing note. Called by Display and other tasks that may
 * need to silence note.
 * 
 */
void SoundSynthesis::restartNote() {
    if (! sdcard) {
        playFreq = midi2freq(playingNote, pitchBend);
        setFreq(playFreq);
    }
}

/**
 * @brief Start playing a note.
 * 
 * @param note MIDI note
 * @param volume Volme (0 to scalevolume)
 * @param attack 1:Use attack at start
 */
void SoundSynthesis::startNote(int note, int volume, int attack) {
    lastError = 1;
    playingNote = note;
    playFreq = midi2freq(note, (float)pitchBend/100.0);
    changeVolume(volume);
    model->noteOn(note, attack);
    setFreq(playFreq, note);
}

/**
 * @brief Stop and wait for synth to stop
 * 
 * @return true Stop successful
 * @return false Stop failed
 */
bool SoundSynthesis::stopWait() {
    changeVolume(0);
    if (sdcard) {
        return model_sd.stopWait();
    }
    return true;
}

/**
 * @brief Stop a playing note (no need to wait)
 * 
 * @param note MIDI note number
 * @param volume Volume (unused)
 */
void SoundSynthesis::stopNote(int note, int volume) {
    playingNote = note;
    changeVolume(0);
}

/**
 * @brief Set pitch bench
 * 
 * @param bend MIDI bend (+/- 8000)
 */
void SoundSynthesis::setBend(int bend) {
    bend = bend / 80; // adjust from +/-8000 to +/-100
    if (pitchBend == bend) return;
    newBend = bend;
}

/**
 * @brief Read pitch bend parameters and process if there is a change.
 * 
 */
void SoundSynthesis::processBend() {
    if (newBend == pitchBend) return;
    // pitchBend = AudioPlayUtils::cushion2(pitchBend, newBend, 40);
    pitchBend.update(newBend);
    playFreq = midi2freq(playingNote, pitchBend.asFloat()/100.0F);
    setFreq(playFreq);
}

#if 0
/**
 * @brief Set vibrato
 * 
 * @param vibrato 
 */
void SoundSynthesis::setModulateVibrato(int vibrato) {
  if (sound->synth_use == ANALOG_MODEL || sound->synth_use == WAVETABLE_MODEL){
    vibratoRate = vibrato / 512.0F;
    fil_lfo_mix1.gain(1, vibratoRate);
    fil_lfo_mix2.gain(1, vibratoRate);
    fil_lfo_mix3.gain(1, vibratoRate);
  }
}
#endif

void SoundSynthesis::setModulateTremolo(int tremolo) {
}

/**
 * @brief Invert both channel (change sign)
 * 
 * @param inv 
 */
void SoundSynthesis::setInvert(int inv) {
    invert = inv;
}

/**
 * @brief Set differential signal when sending to speaker
 * 
 * @param state 
 */
void SoundSynthesis::setDifferential(int state) {
    if (state) {
        // led.color(1, 100); // DEBUG
        trans_L.direction = 1;
        trans_R.direction = -1;
    }
    else {
        // led.color(1, 0); // DEBUG
        trans_L.direction = 1;
        trans_R.direction = 1;
    }
}

#if 0
void SoundSynthesis::rampUp() {
}

void SoundSynthesis::updateData(int slot, int16_t *data, uint32_t len) {
}
#endif

bool codec_init = false;

/**
 * @brief Initialize sound system and codec
 * 
 * @return true Sucess
 * @return false Failure
 */
bool SoundSynthesis::begin() {
#ifdef USE_SGTL5000
    alternate_i2s();
    if (! codec.enable()) {
        return false;
    }

    // audio_base.begin(400, 0.1, WAVEFORM_SINE);

    setCodecVolume(settings.hpVolume);
    codec_init = true;
#endif
    return true;
}

void SoundSynthesis::enable() {
    // CORE_PIN22_CONFIG = PORT_PCR_MUX(6); // pin 22, PTC1, I2S0_TXD0
    // connect pin 20 to nothing because of hardware bug
    CORE_PIN20_CONFIG = PORT_PCR_MUX(1); // pin 22, PTD5
    pinMode(20, INPUT);
    //	dac.begin();
}

#if 0
bool SoundSynthesis::fileExists(const char *filename) {
#ifdef USE_SDCARD
    if (! initSD()) return false;
    File f = SD.open(filename);
    if (!f) return false;
    f.close();
#endif
    return true;
}
#endif

#ifdef USE_AUDIO_MEM

void SoundSynthesis::playMem(const uint8_t *ddata, int dsize) {
    // setOutputMixer(3);
    audio_mem.playu(ddata, dsize, 8000);
    for(int i=0;i<10000; i++) {
        delay(1);
        if (! audio_mem.isPlaying()) break;
    }
    // setOutputMixer();
}

void SoundSynthesis::playMem(const int8_t *ddata, int dsize) {
    // setOutputMixer(3);
    audio_mem.play(ddata, dsize, 8000);
    for(int i=0;i<10000; i++) {
        delay(1);
        if (! audio_mem.isPlaying()) break;
    }
    // setOutputMixer();
}

#endif

/**
 * @brief Initialize SD card subsystem
 * 
 * @return true Success
 * @return false Failure (missing or other error)
 */
bool SoundSynthesis::initSD() {
#ifdef USE_SDCARD
    if (sdcardinit) {
        // we have initialized, so check to make sure we still have
        // a card inserted.
#ifdef SDCARD_DETECT
        if (digitalRead(SDCARD_DETECT)) {
            sdcardpresent = false;
            sdcardinit = false;
            return false;
        }
#endif
    }
    else {
        // not initialized, so check card

        sdcardinit = true;

#ifdef SDCARD_DETECT
        // if we have card detect pin, check that first
        if (digitalRead(SDCARD_DETECT)) {
            // no card, exit
            sdcardpresent = false;
            return false;
        }
#endif

        // try to initialize card
        if (! SD.begin(SDCARD_CS)) {
            sdcardpresent = false;
            return false;
        }
        else {
            sdcardpresent = true;
        }
    }
#endif
    return true;
}

#if 0
void SoundSynthesis::dump_memory() {
    Serial.print("Proc = ");
    Serial.print(AudioProcessorUsage());
    Serial.print(" (max=");
    Serial.print(AudioProcessorUsageMax());
    Serial.print("),  Mem = ");
    Serial.print(AudioMemoryUsage());
    Serial.print(" (max=");
    Serial.print(AudioMemoryUsageMax());
    Serial.println(")");
}
#endif

#if 0
void SoundSynthesis::setOutputMixer(int ch) {
    if (ch == -99) {
        if (sdcard) {
            mixer_synth.gain(0, 0);
            mixer_synth.gain(1, 1.0);
            mixer_synth.gain(2, 1.0);
            mixer_synth.gain(3, 1.0);
        }
        else {
            mixer_synth.gain(0, 1.0);
            mixer_synth.gain(1, 0);
            mixer_synth.gain(2, 0);
            mixer_synth.gain(3, 1.0);
        }
    }
    else {
        mixer_synth.gain(0, 0);
        mixer_synth.gain(1, 0);
        mixer_synth.gain(2, 0);
        mixer_synth.gain(3, 1.0);
        if (ch>=0) mixer_synth.gain(ch, 1.0);
    }
}
#endif

#ifdef USE_ADD_NOISE
void SoundSynthesis::setInterference(int state) {
    if (state) {
        usb_noise.amplitude(0.0005);
        mixer_dac_L.gain(2, 1);
        mixer_dac_R.gain(2, 1);
    }
    else {
        usb_noise.amplitude(0);
        mixer_dac_L.gain(2, 0);
        mixer_dac_R.gain(2, 0);
    }
}
#endif

void SoundSynthesis::setMainVolume(float vol) {
    static float lastvol;

    if (vol < 0) {
        vol = lastvol;
    }
    else {
        lastvol = vol;
    }

#if USE_SIMPLE_CV
    if (useCV) {
        mixer_signal_L.gain(0, 0);
        mixer_signal_R.gain(0, 0);
        mixer_signal_L.gain(1, 1);
        mixer_signal_R.gain(1, 1);
    }
#else
    if (0) { }
#endif
    else {
        // mixer_signal_L.gain(0, 1);
        // mixer_signal_R.gain(0, 1);
        // mixer_signal_L.gain(1, 0);
        // mixer_signal_R.gain(1, 0);

        if (settings.synthActive) {
            mixer_audio_L.gain(0, (float)invert * vol * (float)(1.0 - usbVolume));
            mixer_audio_R.gain(0, (float)invert * vol * (float)(1.0 - usbVolume));
        }
        else {
            mixer_audio_L.gain(0, 0);
            mixer_audio_R.gain(0, 0);
        }

        mixer_audio_L.gain(1, (float)invert * vol * (float)(1.0 - usbVolume));
        mixer_audio_R.gain(1, (float)invert * vol * (float)(1.0 - usbVolume));
    }

    runSlowTasks();
}

void SoundSynthesis::setEffect(int effect, float param1, float param2) {

}

/**
 * @brief Initialize pins (using alternate pins)
 * 
 */
void SoundSynthesis::alternate_i2s(void) {
    CORE_PIN3_CONFIG = PORT_PCR_MUX(6); // pin 3, PTA12, I2S0_TXD0
    CORE_PIN4_CONFIG = PORT_PCR_MUX(6); // pin 4, PTA13, I2S0_TX_FS (LRCLK)
    CORE_PIN9_CONFIG  = PORT_PCR_MUX(6); // pin  9, PTC3, I2S0_TX_BCLK
    CORE_PIN28_CONFIG = PORT_PCR_MUX(4); // pin 28, PTC8, I2S0_MCLK
};

/**
 * @brief Convert volume to log scale
 * 
 * @param in Volume (0.0-1.0)
 * @return float Log scaled volume
 */
inline float scaleLogVolume(float in) {
    const float logbase = 10.0F;
    return (pow(logbase, in)-1.0F)/(logbase-1.0F);
}

/**
 * @brief Run synth low priority tasks (like USB volume)
 * 
 */
void SoundSynthesis::runSlowTasks() {
#ifdef USE_USBAUDIO_IN
    if (usbIn.features.change) {
        usbIn.features.change = 0;
        if (usbIn.features.mute) {
            // codec.volume(1, 0.0F);
            mixer_audio_R.gain(1, 0.0F);
            mixer_audio_L.gain(1, 0.0F);
        }
        else {
            // codec.volume(1, 1.0F);
            mixer_audio_R.gain(1, scaleLogVolume(usbVolume * usbIn.features.getVolume(0)));
            mixer_audio_L.gain(1, scaleLogVolume(usbVolume * usbIn.features.getVolume(1)));
        }
    }

    if (codec.getStatus() == 0) {
        // Display::displayDebug("HP RESET");
        setCodecVolume(settings.hpVolume);
    }
#endif
}

#define SD_CARD_READAHEAD

/**
 * @brief Process tasks that need to be done quickly like reading SD card.
 * 
 */
void SoundSynthesis::runFastTasks() {
#ifdef SD_CARD_READAHEAD
    model_sd.processBackground();
#endif
    processBend();
}

/**
 * @brief Initialize waveforms and other settings for playing back sounds.
 * 
 */
bool SoundSynthesis::initializeSound() {
    static bool memory_alloc = true;
    if (memory_alloc) {
        memory_alloc = false;
        AudioMemory(30);
        AudioProcessorUsageMaxReset();
        AudioMemoryUsageMaxReset();
    }

#ifdef USE_SGTL5000
    if (! codec_init) {
        codec_init = true;
        if (! codec.enable()) {
            return false;
        }
        else {
            setCodecVolume(settings.hpVolume);
        }
    }
#endif

    mixer_synth.gain(0, 1.0);

    setMainVolume(1.0);
    // mixer_dac.gain(0, 1-usbVolume);
    // mixer_dac.gain(1, usbVolume);

    // setOutputMixer(-1);

#ifdef USE_SDCARD
    initSD();
    if (sdcardpresent) {
        if (model_sd.readSoundInfo()) {
            sdcard = true;
        }
        else {
            sdcard = false;
        }
    }
    else {
        sdcard = false;
    }
#endif

    resetFilter();

#ifndef USE_SGTL5000
    dac.analogReference(INTERNAL);
#endif

    playingNote = 0;
    pitchBend = 0;

    return true;
}