#include "firmware.h"
#include <Audio.h>
#include "AudioEffectTransform.h"
#include "Settings.h"
#include "SynthSettings.h"
#include "SoundSynthesis.h"
#include "SoundModelAnalog.h"

float SoundModelAnalog::playFreq = 0.0;
float SoundModelAnalog::playVolume = 0.0;
float SoundModelAnalog::vibratoRate = 0.0;

SynthSettings *sound;

AudioSynthWaveform       fil_lfo;      //xy=77.99999237060547,56.22222900390625
AudioEffectEnvelope      fil_lfo_envelope;      //xy=124,128
AudioMixer4              fil_lfo_mix1;         //xy=308,60
AudioMixer4              fil_lfo_mix2;         //xy=311.99998474121094,127.77778625488281
AudioMixer4              fil_lfo_mix3;         //xy=314,196

AudioSynthWaveform       fil_vibrato;

AudioSynthWaveformModulated fil_waveform1;   //xy=443,46
AudioSynthWaveformModulated fil_waveform2;   //xy=446,92
AudioSynthWaveformModulated fil_waveform3;   //xy=452.1111145019531,137.77777099609375
// AudioSynthWaveform       fil_waveform4;      //xy=64,138
// AudioSynthWaveform       fil_waveform5;      //xy=64,179
// AudioSynthWaveform       fil_waveform6;      //xy=65,225
AudioSynthNoiseWhite     fil_noise1;         //xy=451.4444274902344,209.55560302734375
AudioMixer4              fil_mixer1;         //xy=613,116

int16_t fil_chorus1_buffer[fil_chorus1_len];
AudioEffectChorus        fil_chorus1;        //xy=882,182

AudioFilterStateVariable fil_filter1;        //xy=703,201
// AudioEffectDelayDownBuffer<4, 200>     fil_echo; // combine buffer with other delay line?
// AudioMixer4              fil_output;         //xy=613,116
AudioEffectEnvelope      fil_envelope1;      //xy=788,117
AudioEffectAmplifyRamp2  fil_amplifier;

AudioConnection          fpatchCord1(fil_lfo, fil_lfo_envelope);
AudioConnection          fpatchCord2(fil_lfo_envelope, 0, fil_lfo_mix1, 0);
AudioConnection          fpatchCord3(fil_lfo_envelope, 0, fil_lfo_mix2, 0);
AudioConnection          fpatchCord4(fil_lfo_envelope, 0, fil_lfo_mix3, 0);
AudioConnection          fpatchCord2a(fil_vibrato, 0, fil_lfo_mix1, 1);
AudioConnection          fpatchCord3a(fil_vibrato, 0, fil_lfo_mix2, 1);
AudioConnection          fpatchCord4a(fil_vibrato, 0, fil_lfo_mix3, 1);
AudioConnection          fpatchCord5(fil_lfo_mix1, 0, fil_waveform1, 0);
AudioConnection          fpatchCord6(fil_lfo_mix2, 0, fil_waveform2, 0);
AudioConnection          fpatchCord7(fil_lfo_mix3, 0, fil_waveform3, 0);

AudioConnection          fpatchCord8(fil_waveform1, 0, fil_mixer1, 0);
AudioConnection          fpatchCord9(fil_waveform2, 0, fil_mixer1, 1);
AudioConnection          fpatchCord11(fil_waveform3, 0, fil_mixer1, 2);
AudioConnection          fpatchCord12(fil_noise1, 0, fil_mixer1, 3);

AudioConnection          fpatchCord12b(fil_mixer1, fil_amplifier);

AudioConnection          fpatchCord17(fil_amplifier, 0, fil_filter1, 0);
AudioConnection          fpatchCord19(fil_filter1, fil_envelope1);
AudioConnection          fpatchCord18c(fil_envelope1, fil_chorus1);

AudioStream &SoundModelAnalog::getOutput() {
    return fil_chorus1;
}

void SoundModelAnalog::setVolume(float volume) {
    playVolume = volume;
    if (playVolume==0.0F) {
        AudioNoInterrupts();
        fil_envelope1.noteOff();
        fil_lfo_envelope.noteOff();
        AudioInterrupts();
    }
    else {
        AudioNoInterrupts();
        updateFilterFreq();
        fil_amplifier.gain(playVolume);
        AudioInterrupts();
    }
}

void SoundModelAnalog::resetFilter(SynthSettings *s) {
    sound = s;
    fil_filter1.frequency(sound->u.analog.filterFreq);
    fil_filter1.resonance(sound->u.analog.filterResonance);
    fil_noise1.amplitude(0.2);

    fil_waveform1.frequencyModulation(1);
    fil_waveform2.frequencyModulation(1);
    fil_waveform3.frequencyModulation(1);
    fil_waveform1.begin(1.0, 10.0, sound->u.analog.fil_wave[0]);
    fil_waveform2.begin(1.0, 10.0, sound->u.analog.fil_wave[1]);
    fil_waveform3.begin(1.0, 10.0, sound->u.analog.fil_wave[2]);
    setFreq(playFreq);

    fil_mixer1.gain(0, sound->u.analog.fil_vol[0]);
    fil_mixer1.gain(1, sound->u.analog.fil_vol[1]);
    fil_mixer1.gain(2, sound->u.analog.fil_vol[2]);
    fil_mixer1.gain(3, sound->u.analog.fil_vol_noise);

    if (sound->u.analog.playAttack) {
        fil_envelope1.attack(50);
        fil_envelope1.sustain(0.8);
        fil_envelope1.release(20);
    }
    else {
        fil_envelope1.attack(5);
        fil_envelope1.sustain(1.0);
        fil_envelope1.release(5);
    }
    fil_envelope1.releaseNoteOn(0);

    fil_lfo.begin(0.25, sound->u.analog.fil_lfo_freq, sound->u.analog.fil_lfo_wave);
    fil_lfo_envelope.delay(sound->u.analog.fil_lfo_delay/2);
    fil_lfo_envelope.attack(sound->u.analog.fil_lfo_delay/2);
    fil_lfo_envelope.sustain(1.0);
    fil_lfo_mix1.gain(0, sound->u.analog.fil_lfo_mix[0]);
    fil_lfo_mix2.gain(0, sound->u.analog.fil_lfo_mix[1]);
    fil_lfo_mix3.gain(0, sound->u.analog.fil_lfo_mix[2]);

    fil_vibrato.begin(0.25, 4, WAVEFORM_TRIANGLE);
    fil_lfo_mix1.gain(1, vibratoRate);
    fil_lfo_mix2.gain(1, vibratoRate);
    fil_lfo_mix3.gain(1, vibratoRate);

    fil_chorus1.begin(fil_chorus1_buffer, fil_chorus1_len, sound->u.analog.fil_chorus1_num);
}

void SoundModelAnalog::setFreq(float freq) {
    playFreq = freq;
    fil_filter1.frequency(sound->u.analog.filterFreq * (playFreq / 261.6F));
    fil_waveform1.frequency(sound->u.analog.fil_freq[0] * sound->u.analog.fil_freq_bias[0] * playFreq);
    fil_waveform2.frequency(sound->u.analog.fil_freq[1] * sound->u.analog.fil_freq_bias[1] * playFreq);
    fil_waveform3.frequency(sound->u.analog.fil_freq[2] * sound->u.analog.fil_freq_bias[2] * playFreq);
}

void SoundModelAnalog::noteOn(int note, int attack) {
    if (attack) {
        fil_envelope1.noteOn();
        fil_lfo_envelope.noteOn();
    }
}

void SoundModelAnalog::updateFilterFreq() {
#ifdef USE_FILTER_SYNTH
    // Display::displayDebug("CF=", (int)sound->filterFreq);
    if (sound->u.analog.filterFreq < 1.0F) {
        // float cf = 10.0F + playVolume * 10000.0F;
        // Display::displayDebug("CFX=", (int)cf);
        fil_filter1.frequency(10.0F + playVolume * 10000.0F);
    }
    else {
        fil_filter1.frequency(sound->u.analog.filterFreq * (1 + playVolume));
    }
#endif
}

#if 0
void SoundModelAnalog::setModulateVibrato(int vibrato) {
  if (sound->synth_use == ANALOG_MODEL || sound->synth_use == WAVETABLE_MODEL){
    vibratoRate = vibrato / 512.0F;
    fil_lfo_mix1.gain(1, vibratoRate);
    fil_lfo_mix2.gain(1, vibratoRate);
    fil_lfo_mix3.gain(1, vibratoRate);
  }
}
#endif