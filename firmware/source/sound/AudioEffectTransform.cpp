#ifdef ARDUINO

#include <Audio.h>
#include "AudioEffectTransform.h"

#define FLUX 128

void AudioEffectTransform::update(void)
{
    audio_block_t *blocka;
    int16_t *pa, *end, sample;
    int32_t m;
    
    blocka = receiveWritable(0);
    if (!blocka) {
        return;
    }

    if (direction==1 && divisor==1) {
        transmit(blocka);        
        release(blocka);
        return;        
    }
    
    pa = blocka->data;
    end = pa + AUDIO_BLOCK_SAMPLES;
    while (pa < end) {
        sample = *pa;
        m = direction * sample;
        if (divisor!=1) m /= divisor;
        *pa++ = m;
    }
    transmit(blocka);
    release(blocka);
}

#if 0

void AudioOutputMem::update(void)
{
    audio_block_t *block_in = receiveReadOnly(0);
    if (!block_in) {
        valid = 0;
        return;
    }
    valid = 1;
    for(int i=0; i < AUDIO_BLOCK_SAMPLES; i++) {
        data[i] = block_in->data[i];
    }
    transmit(block_in);
}

void AudioEffectSpeed::update(void)
{
    static audio_block_t *block_in = NULL;
    static int sample = 0;
    const int sample_max = AUDIO_BLOCK_SAMPLES * (1 << speed_bits);

    audio_block_t *block_out = allocate();
    if (!block_out) return;
    
    for(int i=0; i < AUDIO_BLOCK_SAMPLES; i++) {
        if (!block_in) {
            sub->getNext();
            if (!sub->valid) {
                release(block_out);
                return;
            }
            sample = 0;
        }
        else {
            sample += speed;
            if (sample >= sample_max) {
                release(block_in);
                sub->getNext();
                if (!sub->valid) {
                    release(block_out);
                    return;
                }
                sample = sample_max - sample;
            }
        }
        int j = sample >> speed_bits;
        block_out->data[i] = sub->data[j];
    }
    
    transmit(block_out);
    release(block_out);
}

#endif

void AudioEffectDeclick::update(void)
{
    audio_block_t *blocka;
    int16_t *pa, *end;
    
    blocka = receiveWritable(0);
    if (!blocka) {
        return;
    }

    pa = blocka->data;
    end = pa + AUDIO_BLOCK_SAMPLES;
    while(pa < end) {
        average = (average * scale + *pa) >> bits;
        *(pa++) = average;
    }

    transmit(blocka);        
    release(blocka);
    return;        
}

// void AudioEffectDeclick::setStrength(int sbits)
// {
//     if (bits==-99) bits = 4;
//     bits = sbits;
//     scale = (1<<bits)-1;
// }

#define MULTI_UNITYGAIN 65536
#define RAMP_JUMP 1

void AudioEffectAmplifyRamp::update(void)
{
    audio_block_t *block;

    // no change, so try a few optimizations
    if (target_multiplier == multiplier) {
        if (multiplier == 0) {
                // zero gain, discard any input and transmit nothing
                block = receiveReadOnly(0);
                if (block) release(block);
                return;
        } 
        else if (multiplier == MULTI_UNITYGAIN) {
                // unity gain, pass input to output without any change
                block = receiveReadOnly(0);
                if (block) {
                        transmit(block);
                        release(block);
                }
                return;
        } 
    }

    // apply gain to signal
    block = receiveWritable(0);
    if (! block) return;

    uint32_t *p = (uint32_t *)block->data;
    const uint32_t *end = (uint32_t *)(block->data + AUDIO_BLOCK_SAMPLES);

    do {
        // ramp up/down volume changes
        // multiplier = target_multiplier;
        if (multiplier != target_multiplier) {
            if (multiplier < target_multiplier - RAMP_JUMP) multiplier += RAMP_JUMP;
            else if (multiplier > target_multiplier + RAMP_JUMP) multiplier -= RAMP_JUMP;
        }

        uint32_t tmp32 = *p; // read 2 samples from *data
        int32_t val1 = signed_multiply_32x16b(multiplier, tmp32);
        int32_t val2 = signed_multiply_32x16t(multiplier, tmp32);
        val1 = signed_saturate_rshift(val1, 16, 0);
        val2 = signed_saturate_rshift(val2, 16, 0);
        *p++ = pack_16b_16b(val2, val1);
    } while (p < end);

    transmit(block);
    release(block);
}

void AudioEffectAmplifyRamp2::update(void)
{
    audio_block_t *block;

    // no change, so try a few optimizations
    if (target_multiplier == multiplier) {
        if (multiplier == 0) {
                // zero gain, discard any input and transmit nothing
                block = receiveReadOnly(0);
                if (block) release(block);
                return;
        } 
        else if (multiplier == MULTI_UNITYGAIN) {
                // unity gain, pass input to output without any change
                block = receiveReadOnly(0);
                if (block) {
                        transmit(block);
                        release(block);
                }
                return;
        } 
    }

    // apply gain to signal
    block = receiveWritable(0);
    if (! block) return;

    uint32_t *p = (uint32_t *)block->data;
    const uint32_t *end = (uint32_t *)(block->data + AUDIO_BLOCK_SAMPLES);

    do {
        if (multiplier != target_multiplier) {
            multiplier = (multiplier * ramp_scale + target_multiplier) >> ramp_bits;
        }

        uint32_t tmp32 = *p; // read 2 samples from *data
        int32_t val1 = signed_multiply_32x16b(multiplier, tmp32);
        int32_t val2 = signed_multiply_32x16t(multiplier, tmp32);
        val1 = signed_saturate_rshift(val1, 16, 0);
        val2 = signed_saturate_rshift(val2, 16, 0);
        *p++ = pack_16b_16b(val2, val1);
    } while (p < end);

    transmit(block);
    release(block);
}

#endif
