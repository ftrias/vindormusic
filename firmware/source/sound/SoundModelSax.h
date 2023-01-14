#ifndef SOUNDMODELSAX_H
#define SOUNDMODELSAX_H

#include "firmware.h"
#include "SoundSynthesis.h"
#include "AudioEffectTransform.h"

class SoundModelSax : public SoundModel  {
public:
    AudioStream &getOutput();
    void setVolume(float playVolume);
    void resetFilter(SynthSettings *sound);
    void setFreq(float freq);
    void noteOn(int note, int attack);
private:
    float currentFreq;
    float freqAdj;
};

#endif