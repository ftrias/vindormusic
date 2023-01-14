/*
  ==============================================================================

    OscillatorControl.cpp
    Created: 4 Apr 2018 2:48:32pm
    Author:  Fernando Trias

  ==============================================================================
*/

#include "OscillatorControl.h"

void OscillatorControl::setSliderVolume(Slider *volume)
{
    double v= volume->getValue();
    int x = v * 127.0;
    deviceControl->sendFilterSetting(3, number, x);
}
void OscillatorControl::setSliderFrequency(Slider *frequency)
{
    double v = frequency->getValue();
    v = pow(2.0, v / 12.0);
    deviceControl->sendFilterSettingFloat(5, number, v);
}
int OscillatorControl::gui2Code(int code) {
    int w;
    switch(code) {
        case 0: w = WAVEFORM_SINE; break;
        case 1: w = WAVEFORM_TRIANGLE; break;
        case 2: w = WAVEFORM_SQUARE; break;
        case 3: w = WAVEFORM_SAWTOOTH; break;
        case 4: w = WAVEFORM_SAWTOOTH_REVERSE; break;
    }
    return w;
}
void OscillatorControl::setSliderWave(Slider *wave2)
{
    int w = wave2->getValue();
    switch(w) {
        case 0: w = WAVEFORM_SINE; break;
        case 1: w = WAVEFORM_TRIANGLE; break;
        case 2: w = WAVEFORM_SQUARE; break;
        case 3: w = WAVEFORM_SAWTOOTH; break;
        case 4: w = WAVEFORM_SAWTOOTH_REVERSE; break;
    }
    deviceControl->sendFilterSetting(6, number, w);
}
void OscillatorControl::setSliderLfoVolume(Slider *lfoVolume)
{
    int w = lfoVolume->getValue() * 127.0;
    deviceControl->sendFilterSetting(9, number, w);
}

#include <math.h>

void OscillatorControl::setSliderOctave(Slider *octave)
{
    int oct = octave->getValue();
    float mult = pow(2, oct);
    int b1 = 0;
    int b2 = 0;
    // b1 is the integer part
    // b2 is 1/integer
    if (mult >= 1) b1 = mult;
    else if (mult < 1) b2 = 1.0F / mult;
    deviceControl->sendFilterSetting(4, number, b1, b2);
}
