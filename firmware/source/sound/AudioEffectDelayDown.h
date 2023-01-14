#ifndef effect_delay_down_h_
#define effect_delay_down_h_

#include "Arduino.h"
#include "AudioStream.h"
#include "utility/dspinst.h"

const int AUDIO_SAMPLE_RATE_EXACT_INT = AUDIO_SAMPLE_RATE_EXACT;

class AudioEffectDelayDown : public AudioStream
{
public:
	AudioEffectDelayDown(const int down, uint8_t *data, const int sz, const int echo_divisor=0) : AudioStream(1, inputQueueArray), 
    echo(echo_divisor), downsample(down), buffer(data), buffer_length(sz), 
    buffer_in(0) , prev_value(0)
    { 
        memset(buffer, MuLawZero, buffer_length); 
        buffer_out = AUDIO_BLOCK_SAMPLES / downsample;
    }
    virtual void update(void);
    void setEcho(int echo_divisor) { echo = echo_divisor; }

private:
    static const uint8_t MuLawZero = 255; // 255 is 0 in MuLaw
    int echo;
    const int downsample;
    uint8_t *buffer;
    int buffer_length;
    int buffer_in;
    int buffer_out;
    int prev_value;

    audio_block_t *inputQueueArray[1];
};

template <int DOWNSAMPLE, int MILLISECONDS>
class AudioEffectDelayDownBuffer : public AudioEffectDelayDown
{
private:
    static const int BUFFER_LENGTH_SZ = AUDIO_SAMPLE_RATE_EXACT_INT * MILLISECONDS / 1000 / DOWNSAMPLE + AUDIO_BLOCK_SAMPLES / DOWNSAMPLE;
    uint8_t data[BUFFER_LENGTH_SZ];
public:
	AudioEffectDelayDownBuffer() : AudioEffectDelayDown(DOWNSAMPLE, data, BUFFER_LENGTH_SZ) { }
};

#endif