#ifndef SOUNDMODELANALOG_H
#define SOUNDMODELANALOG_H

#include "firmware.h"
#include "SoundSynthesis.h"
#include "AudioEffectTransform.h"

class SoundModelAnalog : public SoundModel  {
public:
    AudioStream &getOutput();
    void setVolume(float playVolume);
    void resetFilter(SynthSettings *sound);
    void setFreq(float freq);
    void noteOn(int note, int attack);

private:
    static float playFreq;
    static float playVolume;
    static float vibratoRate;
    void updateFilterFreq();
};

#endif