#ifndef synth_waveform_sdcard_h_
#define synth_waveform_sdcard_h_

#ifdef USE_SDCARD

#include "Arduino.h"
#include "AudioStream.h"
#include "arm_math.h"

#include "Audio.h"
#include <stdint.h>

// #include "Instrument.h"
#include "SdLib.h"
#include "AudioPlayUtils.h"
#include "vutils.h"

const int crossblocks = 16;    // number of blocks to perform crossfade
const int crosssamples = crossblocks * AUDIO_BLOCK_SAMPLES;

const int buffer_len = AUDIO_BLOCK_SAMPLES;
const int readcount = crossblocks + 2;

class SDCardFileBuffer {
private:
    int loop_start;
    int loop_end;
    int file_offset;
    int readbusy = 0;
    int readindex = 0;
    int writeindex = -1;
    int16_t readbuffer[readcount][buffer_len] __attribute__((aligned));
    int buffer_count;
    
public:
    bool open(File &pfile, const char *filename, int loopStart, int loopEnd);
    int readBlockRaw(File &pfile, int16_t* data, int read_len);
    int16_t *getNextBlock();
    int16_t *getNextBlock(File &pfile, int peek=0);
    void readBlockFill(File &pfile);
    void startLoop(File &pfile);
    int readHeader(File &pfile);
};

class AudioSynthSDCard : public AudioStream, public AudioPlayUtils
{
public:
    AudioSynthSDCard(void) : AudioStream(0, NULL)
    { begin(); }
private:
    int playing;
    File pfile;
    SDCardFileBuffer pc[2];
    int pcurrent;
    int pcross;
    int pskip;
    
    char fn[256];
    int loop_start;
    int loop_end;
    // int magnitude;
    EMA<2> magnitude;
    int desired_magnitude;
    int read_index;
    int play_index;
    int16_t buffer[buffer_len];
    int16_t buffer2[buffer_len];
    
    int file_offset;
    int opennew;
    int crossfade;
    int crossfade_start;
    int crossfade_end;
    int crossidx;

    int stop_flag;
    int16_t lastvalue;
    int skipupdate;
    int skipblocks;
    int cushionmax = 0;  
    
    int readbusy;
    
    int readBlockRaw(int16_t* data, int read_len);
    
    int16_t *getNextBlock();
    void readBlockFill();
    
public:
    void begin();
    void setFactor(int f);
    bool play(const char *filename, int loopStart=0, int loopEnd=0, int jumpToLoop=0);
    bool isPlaying();
    virtual void update();
#ifdef USE_SD_SPEED
    void update_speed();
    int play_speed = 0;
#endif
    void amplitude(float n);
    void stop();
    void readahead();
};

#endif

#endif
