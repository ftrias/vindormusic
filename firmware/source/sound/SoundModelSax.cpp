#include "firmware.h"
#include <Audio.h>
#include "AudioEffectTransform.h"
#include "Settings.h"
#include "SynthSettings.h"
#include "SoundSynthesis.h"
#include "SoundModelSax.h"

// #include "saxomega_samples.h"
#include "omega_Sax_samples.h"
#ifdef BASE_SOUNDS_MULTIPLE
#include "omega_Flute_samples.h"
#include "omega_Clarinet_samples.h"
#endif

// #define USE_SAX_FILTER

AudioSynthWavetable      fil_wavetable;
AudioEffectAmplifyRamp2  fil_output;
#ifdef USE_SAX_FILTER
AudioFilterStateVariable fil_wt_filter;
AudioConnection          fpatchCord31(fil_wavetable, fil_wt_filter);
AudioConnection          fpatchCord31b(fil_wt_filter, fil_output);
#else
AudioConnection          fpatchCord31(fil_wavetable, fil_output);
#endif

AudioStream &SoundModelSax::getOutput() {
    return fil_output;
}

void SoundModelSax::setVolume(float playVolume) {
    AudioNoInterrupts();
    fil_wavetable.amplitude(playVolume);
#ifdef USE_SAX_FILTER
    fil_wt_filter.frequency(100.0F + playVolume * currentFreq * 4);
    // fil_wt_filter.resonance(1.0F + playVolume);
#endif
    AudioInterrupts();
}

void SoundModelSax::resetFilter(SynthSettings *sound) {
#ifdef BASE_SOUNDS_MULTIPLE
    switch (sound->midiNumber) {
        case 74: 
            fil_wavetable.setInstrument(omega_Flute); 
            freqAdj = 0.498F;
            fil_output.gain(0.9F);
            break; // Flute
        case 72: 
            fil_wavetable.setInstrument(omega_Clarinet); 
            freqAdj = 0.5F;
            fil_output.gain(0.9F);
            break; // Clarinet
        case 67:
        default: 
            fil_wavetable.setInstrument(omega_Sax); 
            freqAdj = 0.5065F;
            fil_output.gain(1.2F);
            break;
    }
#else
    freqAdj = 0.5065F;
    fil_wavetable.setInstrument(omega_Sax);
    fil_output.gain(1.2F);
#endif

    fil_wavetable.playFrequency(0.0F, 0);
#ifdef USE_SAX_FILTER
    fil_wt_filter.frequency(500.0F);
#endif
}

void SoundModelSax::setFreq(float playFreq) {
    currentFreq = playFreq * freqAdj;
    fil_wavetable.setFrequency(currentFreq);
}

void SoundModelSax::noteOn(int note, int attack) {
    if (attack) {
        fil_wavetable.playNote(note);
    }
}