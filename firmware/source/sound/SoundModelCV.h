#ifndef SOUNDMODELCV_H
#define SOUNDMODELCV_H

#include "firmware.h"
#include "SoundSynthesis.h"
#include "AudioEffectTransform.h"

class SoundModelCV : public SoundModel  {
public:
    AudioStream &getOutput();
    void setVolume(float playVolume);
    void resetFilter(SynthSettings *sound);
    void setFreq(float freq);
    void noteOn(int note, int attack);

public:
    void setOneWire(int status);
#ifdef USE_SIMPLE_CV
    AudioStream &getFreqOutput();
    AudioStream &getVolumeOutput();
#endif
};

#endif