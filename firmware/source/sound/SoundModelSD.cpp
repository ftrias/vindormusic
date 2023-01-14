#ifdef USE_SDCARD

#include "SoundModelSD.h"
#include "AudioSynthSDCard.h"
#include "Display.h"
#include "WatchDog.h"
#include "vdebug.h"

AudioSynthSDCard audio_sd;

char wavprefix[128];
char wavext[4];

// #ifdef USE_SDCARD

const int num_position = 120;
long start_position[num_position];
long end_position[num_position];

char *parseInt(char *str, long *result) {
    char *p=str;
    for(; *p; p++) {
        if (*p==0 || *p==',' || *p=='\n') {
            *p = 0;
            *result = atoi(str);
            return p+1;
        }
    }
    *result = 0; // this gets rid of warnings
    return NULL;
}

int file_gets(File *file, char *str, int sz) {
    int b;
    int i = 0;
    while((b = file->read()) > 0) {
        str[i] = (char)b;
        if (str[i] == '\r') continue;
        if (str[i] == '\n') {
            str[i] = 0;
            return i;
        }
        i++;
        if (i>=sz) {
            str[sz-1] = 0;
            return i;
        }
    }
    if (i == 0 && b == -1) return -1;
    str[i] = 0;
    return i;
}

bool SoundModelSD::hasInstrument(int inst) {
    char str[64];
    getPrefix(str, inst);
    strcat(str, ".inf");
    return SD.exists(str);
}


int SoundModelSD::getNameForInstrument(char *s, int inst) {
    char str[64];
    getPrefix(str, inst);
    strcat(str, ".inf");
    File file = SD.open(str, FILE_READ);
    if (! file) {
        sprintf(s, "%d", inst);
        return 0;
    }
    while (1) {
        int c = file_gets(&file, str, sizeof(str)-1);
        if (c<0) break;
        if (str[0] == '#') continue;
        char *eq = strstr(str, "=");
        if (!eq) break; // only read header lines
        *eq = 0;
        if (strcmp(str, "disp") == 0) {
            strcpy(s, eq+1);
            file.close();
            return 1;
        }
    }
    file.close();
    sprintf(s, "%d", inst);
    return 0;
}

bool SoundModelSD::readSoundInfo() {
    for(int i=0; i<num_position; i++) {
        start_position[i] = end_position[i] = 0;
    }

    char str[256];
    strncpy(str, wavprefix, sizeof(str));
    strcat(str, ".inf");

    // must check for existence because SD.open may hang on non-existant file
    if (! SD.exists(str)) {
        // showDebug("Not found ", str);
        return false;
    }
    File file = SD.open(str, FILE_READ);
    if (!file) {
        showDebug("Error opening ", str);
        return false;
    }

    // defaults
    audio_sd.setFactor(1);
    strcpy(wavext, "wav");

    int itemsread = 0;
    while (itemsread < 200) {
        int c = file_gets(&file, str, sizeof(str)-1);
        if (c < 0) break;
        if (c == 0) continue;
        str[c] = 0;

        if (str[0] == '#') continue;

        char *eq = strstr(str, "=");
        if (eq) {
            *eq = 0;
            if (strcmp(str, "factor") == 0) {
                long datafactor;
                parseInt(eq+1, &datafactor);
                audio_sd.setFactor(datafactor);
            }
            else if (strcmp(str, "disp") == 0) {
                showDebug(eq+1);
            }
            else if (strcmp(str, "format") == 0) {
                if (strcmp(eq+1, "raw")==0) {
                    strcpy(wavext, "raw");
                }
                else if (strcmp(eq+1, "wav")==0) {
                    strcpy(wavext, "wav");
                }
            }
        }
        else {
            char *p = str;
            long note;
            p = parseInt(p, &note);
            if (note > 0 && note < num_position) {
                p = parseInt(p, &start_position[note]);
                p = parseInt(p, &end_position[note]);
            }
            itemsread++;
        }
    }
    file.close();

    return true;
}

AudioStream &SoundModelSD::getOutput() {
    return audio_sd;
}

void SoundModelSD::setVolume(float playVolume)  {
    audio_sd.amplitude(playVolume);
}

void SoundModelSD::resetFilter(SynthSettings *sound) {
    audio_sd.begin();
}

void SoundModelSD::setFreq(float freq) {
}

void SoundModelSD::noteOn(int note, int attack) {
    audio_sd.play(note2wav(note), start_position[note], end_position[note]);
}

void SoundModelSD::processBackground() {
    audio_sd.readahead();
}

bool SoundModelSD::stopWait() {
    audio_sd.stop();
    int start = millis();
    while(millis() - start < 1000) {
        watchdog.kick();
        if (! audio_sd.isPlaying()) return true;
    }
    return false;
}

const char *SoundModelSD::note2wav(int note) {
    static char out[256];
    sprintf(out, "%s%d.%s", wavprefix, note, wavext);
    return out;
}

void SoundModelSD::getPrefix(char *str, int inst) {
    strcpy(str, "vindor/s");
    // inst is from 0 to 98, map to string 1 to 99
    inst++;
    if (inst < 10){
        str[8] = '0' + inst;
        str[9] = 0;
    }
    else {
        str[8] = '0' + inst % 10;
        str[9] = '0' + inst / 10;
        str[10] = 0;
    }
    strcat(str, "/play");
}

void SoundModelSD::setPrefix(int inst) {
    getPrefix(wavprefix, inst);
}

#ifdef USE_SD_DIR

void SoundModelSD::setDir(const char *dir) {
    strcpy(wavprefix, dir);
    strcat(wavprefix, "/play");
}

static File next;
static File dir;

void SoundModelSD::setDirNum(int entry) {
    dir.open("vindor");
    for (int i=0; i<=entry; i++) {
        next.close();
        next.openNext(&dir, O_RDONLY);
    }
    char x[128];
    next.getName(x, sizeof(x));
    strcpy(wavprefix, "vindor/");
    strcat(wavprefix, x);
    strcat(wavprefix, "/play");
}

int SoundModelSD::loadDirList(char *dest)  {
    if (dest == NULL) {
        dir.open("vindor");
        return 1;
    }
    if (next.openNext(&dir, O_RDONLY)) {
        next.getName(dest, 16);
        next.close();
        return 1;
    }
    return 0;
}

#endif

#endif //  USE_SDCARD