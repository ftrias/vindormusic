#include <Audio.h>
#include "AudioEffectDelayDown.h"
#include "AudioPlayUtils.h"

void AudioEffectDelayDown::update(void)
{
    int16_t *pa;
    
    audio_block_t *block_in = receiveReadOnly(0);
    if (block_in) {
        pa = block_in->data;

        if (echo) {
            for(int i=0; i<AUDIO_BLOCK_SAMPLES; i+=downsample) {
                int sum = pa[i];
                for(int j=1; j<downsample; j++) sum += pa[i+j];
                int8_t b = AudioPlayUtils::MuLawCompress(sum / downsample);
                buffer[buffer_in] = buffer[buffer_in] / (2*echo) + b / echo;
                if (++buffer_in >= buffer_length) buffer_in = 0;
            }
        }
        else {
            for(int i=0; i<AUDIO_BLOCK_SAMPLES; i+=downsample) {
                int sum = pa[i];
                for(int j=1; j<downsample; j++) sum += pa[i+j];
                buffer[buffer_in] = AudioPlayUtils::MuLawCompress(sum / downsample);
                if (++buffer_in >= buffer_length) buffer_in = 0;
            }
        }
        release(block_in);
    }
    else {
        // no data coming in, but we still have to send pending data out
        for(int i=0; i < AUDIO_BLOCK_SAMPLES / downsample; i++) {
            buffer[buffer_in] = MuLawZero;
            if (++buffer_in >= buffer_length) buffer_in = 0;
        }
    }

    audio_block_t *block_out = allocate();
    if (!block_out) return;
    pa = block_out->data;

    buffer_out = buffer_in;
    if (buffer_out >= buffer_length) buffer_out = 0;

    if (downsample == 1) {
        for(int i=0; i < AUDIO_BLOCK_SAMPLES; i++) {
            pa[i] = MuLawDecompressTable[buffer[buffer_out]];
            if (++buffer_out >= buffer_length) buffer_out = 0;
        }
    }
    else {
        for(int i=0; i<AUDIO_BLOCK_SAMPLES; i+=downsample) {
            int v = MuLawDecompressTable[buffer[buffer_out]];

            int change = (v - prev_value) / downsample;
            int rem = (v - prev_value) % downsample;
            pa[i] = (prev_value += change + rem);
            for(int j=1; j<downsample; j++) {
                pa[i+j] = (prev_value += change);
            }
            prev_value = v; // may not be needed; kept for sanity

            // for(int j=0; j<downsample; j++)
                // pa[i+j] = v;

            if (++buffer_out >= buffer_length) buffer_out = 0;
        }
    }
    
    transmit(block_out);
    release(block_out);
}