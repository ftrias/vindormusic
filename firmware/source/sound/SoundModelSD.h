#ifndef SOUNDMODELSD_H
#define SOUNDMODELSD_H

#ifdef USE_SDCARD

#include "firmware.h"
#include <Audio.h>
#include "SoundSynthesis.h"
#include "AudioSynthSDCard.h"

class SoundModelSD : public SoundModel  {
public:
    AudioStream &getOutput();
    void setVolume(float playVolume);
    void resetFilter(SynthSettings *sound);
    void setFreq(float freq);
    void noteOn(int note, int attack);

    bool readSoundInfo();
    bool hasInstrument(int inst);
    int getNameForInstrument(char *s, int inst);
    void processBackground();
    bool stopWait();
    void setPrefix(int inst);

#ifdef USE_SD_DIR
    void setDir(const char *dir);
    void setDirNum(int entry);
    int loadDirList(char *dest);
#endif

private:
    static float playFreq;
    static float playVolume;
    static float vibratoRate;
    const char *note2wav(int note);
    void getPrefix(char *str, int inst);
};

#endif

#endif