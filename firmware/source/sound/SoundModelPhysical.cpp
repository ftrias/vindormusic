#include "firmware.h"
#include "SoundModelPhysical.h"
#include <Audio.h>
#include "AudioEffectDelayDown.h"
#include "AudioSampleSaxperiodw.h"

float playFreq;

#define CHORUS_DELAY_LENGTH (6*AUDIO_BLOCK_SAMPLES)
short delayline[CHORUS_DELAY_LENGTH];

AudioSynthWaveformDc     dcPlay;            //xy=446,423
AudioSynthWaveform       modulator;      //xy=454,351
AudioSynthWaveformDc     dcRange;            //xy=512,713
AudioEffectEnvelope      fm_env;      //xy=603,349
AudioEffectMultiply      modulator_X_breath;      //xy=610,416
AudioSynthWaveform       sax_sample;      //xy=673,660
AudioEffectEnvelope      filter_env;      //xy=675,719
// AudioEffectFlange        flange1;        //xy=833,595
AudioSynthWaveformModulated waveformMod1;   //xy=842,504
AudioFilterStateVariable filter1;        //xy=845,667
AudioSynthNoisePink      pink1;          //xy=849,722
AudioMixer4              osc_mixer;         //xy=1019,653
AudioMixer4              processed_breath;
AudioEffectEnvelope      amp_env;      //xy=1165,686
AudioSynthWaveformDc     dcOffset;            //xy=1167,746
AudioEffectMultiply      amplitude_X_breath;      //xy=1345,718
AudioEffectMultiply      rawBreath_X_range;
AudioEffectDelayDownBuffer<4, 200>     fil_delay;
AudioSynthWaveform       fundamental1;
AudioAmplifier           filter_depth;
AudioMixer4              mixer_output;
AudioAmplifier           level_output;

AudioConnection          patchCord1(processed_breath, 0, modulator_X_breath, 1);
AudioConnection          patchCord2(modulator, fm_env);
AudioConnection          patchCord3(dcPlay, filter_depth);
AudioConnection          patchCord3a(filter_depth, filter_env);
AudioConnection          patchCord4(fm_env, 0, modulator_X_breath, 0);
AudioConnection          patchCord5(modulator_X_breath, 0, waveformMod1, 0);
AudioConnection          patchCord6(sax_sample, 0, filter1, 0);
AudioConnection          patchCord7(filter_env, 0, filter1, 1);
AudioConnection          patchCord8(fundamental1, 0, osc_mixer, 1);
AudioConnection          patchCord9(waveformMod1, 0, osc_mixer, 0);
AudioConnection          patchCord11(filter1, 0, osc_mixer, 2);
AudioConnection          patchCord12(pink1, 0, osc_mixer, 3);
AudioConnection          patchCord14(osc_mixer, amp_env);
AudioConnection          patchCord15(amp_env, 0, amplitude_X_breath, 0);
AudioConnection          patchCord16(processed_breath, 0, amplitude_X_breath, 1);
AudioConnection          patchCord17(amplitude_X_breath, 0, mixer_output, 0);
AudioConnection          patchCord18(mixer_output, 0, fil_delay, 0);
AudioConnection          patchCord19(fil_delay, 0, mixer_output, 1);
AudioConnection          patchCord20(dcPlay, 0, rawBreath_X_range, 0);
AudioConnection          patchCord21(dcRange, 0, rawBreath_X_range, 1);
AudioConnection          patchCord22(rawBreath_X_range, 0, processed_breath, 0);
AudioConnection          patchCord23(dcOffset, 0, processed_breath, 1);
AudioConnection          patchCord24(mixer_output, level_output);

AudioStream &SoundModelPhysical::getOutput() {
    return level_output;
}

void SoundModelPhysical::setVolume(float playVolume) {
    if (playVolume==0.0F) {
        AudioNoInterrupts();
        fm_env.noteOff();
        amp_env.noteOff();
        filter_env.noteOff();
        AudioInterrupts();
    }
    else {
        AudioNoInterrupts();
        dcPlay.amplitude(playVolume, 4);
        AudioInterrupts();
    }
}

void SoundModelPhysical::resetFilter(SynthSettings *sound) {
    float tone = (float)(sound->u.physical.tone);
    float smoothness = (float)(sound->u.physical.smoothness);
    float echo = (float)(sound->u.physical.echo);
    int i = sound->u.physical.table;
    if (i==0) {
        sax_sample.arbitraryWaveform((const short*)AudioSampleSaxperiodw, 172.0);
    }
    else {
        sax_sample.arbitraryWaveform((const short*)wavetable_list[i], 172.0);
    }
    waveformMod1.phaseModulation(180);

    switch(i) {
        case 0: 
            pink1.amplitude(1); 
            filter1.frequency(mapParam(tone, 0, 127, 200, 1000)); //1400
            filter1.resonance(2); //2
            break;
        default:
            pink1.amplitude(mapParam(tone, 0, 127, 0, 0.5));
            filter1.frequency(mapParam(tone, 0, 127, 200, 4000)); //1400
            filter1.resonance(mapParam(tone, 0, 127, 1, 1.5)); //2
            break;
    }
    filter1.octaveControl(4); //1

    dcRange.amplitude(mapParam(smoothness, 0, 127, 0.99, 0.99)); //1 depth of timbral modulation sound->u.physical.breathDepth
    dcOffset.amplitude(mapParam(smoothness, 0, 127, 0.01, 0.01)); //0 timbral offset

    fm_env.hold(0); //0
    fm_env.attack(mapParam(smoothness, 0, 127, 50, 50)); //50
    fm_env.decay(mapParam(smoothness, 0, 127, 100, 400)); //400
    fm_env.sustain(mapParam(smoothness, 0, 127, 1.0, 1.0)); //0.9
    fm_env.release(mapParam(smoothness, 0, 127, 50, 200)); //100

    amp_env.hold(0); //0
    amp_env.attack(mapParam(smoothness, 0, 127, 10, 200)); //20
    amp_env.decay(mapParam(smoothness, 0, 127, 100, 400)); //1000
    amp_env.sustain(mapParam(smoothness, 0, 127, 1.0, 1.0)); //0.6
    amp_env.release(mapParam(smoothness, 0, 127, 50, 200)); //100

    filter_env.hold(0); //0
    filter_env.attack(mapParam(smoothness, 0, 127, 20, 400)); //80
    filter_env.decay(mapParam(smoothness, 0, 127, 100, 400)); //400
    filter_env.sustain(mapParam(tone, 0, 127, 0.7, 0.7)); //0.3
    filter_env.release(mapParam(smoothness, 0, 127, 50, 200)); //100

    osc_mixer.gain(0, 0.0); //0.1 FM OSC LEVEL
    osc_mixer.gain(1, 0.0); //0.4 FUNDAMENTAL OSC LEVEL
    osc_mixer.gain(2, 0.0); //0.5-4 SAMPLE OSC LEVEL
    osc_mixer.gain(3, 0.0); //0 NOISE LEVEL
    filter_depth.gain(3);

    mixer_output.gain(0, mapParam(echo, 0, 127, 0.2, 0.5)); //0.95 DELAY DRY LEVEL
    mixer_output.gain(1, mapParam(echo, 0, 127, 0.001, 0.5)); //0.1  DELAY FEEDBACK AMOUNT

    level_output.gain(10.0);

    setFreq(playFreq);
    modulator.begin(1, playFreq * 0.5, WAVEFORM_TRIANGLE); //
    waveformMod1.begin(1, playFreq, WAVEFORM_SINE); //
    sax_sample.begin(1, playFreq * 0.5, WAVEFORM_ARBITRARY);
    fundamental1.begin(1, playFreq * 0.5, WAVEFORM_SINE);
}

void SoundModelPhysical::setFreq(float freq) {
    playFreq = freq;
    const float high = 1000;
    const float thresh = 200;

    const float FM_GAIN_BASE = 0.2;
    const float SINE_GAIN_BASE = 0.4;
    const float SAMPLE_GAIN_BASE = 0.5;
    const float NOISE_GAIN_BASE = 0.01;

    const float FM_GAIN_HIGH = 0.0;
    const float SINE_GAIN_HIGH = 0.7;
    const float SAMPLE_GAIN_HIGH = 0.0;
    const float NOISE_GAIN_HIGH = 0.03;

    if (freq > high) {
      osc_mixer.gain(0, FM_GAIN_HIGH); // turn off FM osc to avoid aliasing
      osc_mixer.gain(1, SINE_GAIN_HIGH); // when freq too high turn sine wave all the way up
      osc_mixer.gain(2, SAMPLE_GAIN_HIGH); // turn off sample osc to avoid aliasing
      osc_mixer.gain(3, NOISE_GAIN_HIGH);
    }
    else if (freq < thresh) {
      osc_mixer.gain(0, FM_GAIN_BASE);
      osc_mixer.gain(1, SINE_GAIN_BASE); // baseline osc balance
      osc_mixer.gain(2, SAMPLE_GAIN_BASE);
      osc_mixer.gain(3, NOISE_GAIN_BASE);
    }
    else {
      osc_mixer.gain(0, mapParam(freq, thresh, high, FM_GAIN_BASE, FM_GAIN_HIGH));
      osc_mixer.gain(1, mapParam(freq, thresh, high, SINE_GAIN_BASE, SINE_GAIN_HIGH));
      osc_mixer.gain(2, mapParam(freq, thresh, high,  SAMPLE_GAIN_BASE,  SAMPLE_GAIN_HIGH));
      osc_mixer.gain(3, mapParam(freq, thresh, high, NOISE_GAIN_BASE, NOISE_GAIN_HIGH));
    }
    modulator.frequency(playFreq * 0.5);
    waveformMod1.frequency(playFreq);
    sax_sample.frequency(playFreq * 0.5);
    fundamental1.frequency(playFreq * 0.5);
}

void SoundModelPhysical::noteOn(int note, int attack) {
    if (attack) {
        fm_env.noteOn();
        amp_env.noteOn();
        filter_env.noteOn();
    }
}
