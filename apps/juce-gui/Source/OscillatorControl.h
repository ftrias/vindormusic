/*
  ==============================================================================

    OscillatorControl.h
    Created: 4 Apr 2018 2:48:32pm
    Author:  Fernando Trias

  ==============================================================================
*/

#pragma once

#include "VindorWrap.h"

#define WAVEFORM_SINE              0
#define WAVEFORM_SAWTOOTH          1
#define WAVEFORM_SQUARE            2
#define WAVEFORM_TRIANGLE          3
#define WAVEFORM_ARBITRARY         4
#define WAVEFORM_PULSE             5
#define WAVEFORM_SAWTOOTH_REVERSE  6
#define WAVEFORM_SAMPLE_HOLD       7
#define WAVEFORM_TRIANGLE_VARIABLE 8

class OscillatorControl {
public:
    int number;
    void setSliderVolume(Slider *volume);
    void setSliderFrequency(Slider *frequency);
    void setSliderWave(Slider *wave2);
    void setSliderLfoVolume(Slider *lfoVolume);
    void setSliderOctave(Slider *octave);
    static int gui2Code(int code);

};
