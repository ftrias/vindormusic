#ifndef SOUNDMODELPHYSICAL_H
#define SOUNDMODELPHYSICAL_H

#include "firmware.h"
#include <Audio.h>
#include "SoundSynthesis.h"
#include "AudioEffectTransform.h"

class SoundModelPhysical : public SoundModel  {
public:
    AudioStream &getOutput();
    void setVolume(float playVolume);
    void resetFilter(SynthSettings *sound);
    void setFreq(float freq);
    void noteOn(int note, int attack);
};

#endif