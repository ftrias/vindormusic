/* Audio Library for Teensy 3.X
 * Copyright (c) 2014, Paul Stoffregen, paul@pjrc.com
 *
 * Development of this audio library was funded by PJRC.COM, LLC by sales of
 * Teensy and Audio Adaptor boards.  Please support PJRC's efforts to develop
 * open source software by purchasing Teensy or other PJRC products.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice, development funding notice, and this permission
 * notice shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef effect_trans_h_
#define effect_trans_h_

#ifdef ARDUINO

#include "Arduino.h"
#include "AudioStream.h"
#include "utility/dspinst.h"

class AudioEffectTransform : public AudioStream
{
public:
	AudioEffectTransform() : AudioStream(1, inputQueueArray) { }
    virtual void update(void);
    int direction = 1;
    int divisor = 1;
private:
    audio_block_t *inputQueueArray[1];
};

class AudioOutputMem : public AudioStream
{
public:
	AudioOutputMem() : AudioStream(1, inputQueueArray) { }
    virtual void update(void);
    int16_t data[AUDIO_BLOCK_SAMPLES];
    int valid;
private:
    audio_block_t *inputQueueArray[1];
};

class AudioEffectSpeed : public AudioStream
{
public:
	AudioEffectSpeed(AudioOutputMem *sub_stream) : AudioStream(1, inputQueueArray) { 
        sub = sub_stream;
    }
    virtual void update(void);
    audio_block_t *getNext();

    AudioOutputMem *sub;
    const int speed_bits = 8;
    int speed = (1 << speed_bits);
private:
    audio_block_t *inputQueueArray[1];
};

class AudioEffectDeclick : public AudioStream
{
public:
	AudioEffectDeclick() : AudioStream(1, inputQueueArray), average(0) { }
    virtual void update(void);
    // void setStrength(int sbits=-99);
private:
    const int bits = 4;
    const int scale = (1<<bits)-1;
    int average;
    audio_block_t *inputQueueArray[1];
};

class AudioEffectAmplifyRamp : public AudioStream
{
public: 
    AudioEffectAmplifyRamp(void) : AudioStream(1, inputQueueArray), target_multiplier(0), multiplier(0) { }
        virtual void update(void);
        void gain(float n) {
                if (n > 32767.0f) n = 32767.0f;
                else if (n < -32767.0f) n = -32767.0f;
                target_multiplier = n * 65536.0f;
        }
private:
        int32_t target_multiplier;        
        int32_t multiplier;
        audio_block_t *inputQueueArray[1];
};

class AudioEffectAmplifyRamp2 : public AudioStream
{
public: 
    AudioEffectAmplifyRamp2(void) : AudioStream(1, inputQueueArray), target_multiplier(0), multiplier(0) { }
        virtual void update(void);
        void gain(float n) {
            target_multiplier = n * 65536.0f;
        }
        void gain(int n) {
            target_multiplier = n;
        }
private:
        const int ramp_bits = 4;
        const int ramp_scale = ((1<<ramp_bits)-1);
        int32_t target_multiplier;        
        int32_t multiplier;
        audio_block_t *inputQueueArray[1];
};

#endif

#endif