/*
  ==============================================================================

  This is an automatically generated GUI class created by the Projucer!

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Created with Projucer version: 5.4.7

  ------------------------------------------------------------------------------

  The Projucer is part of the JUCE library.
  Copyright (c) 2017 - ROLI Ltd.

  ==============================================================================
*/

#pragma once

//[Headers]     -- You can add your own extra header files here --
#include "../JuceLibraryCode/JuceHeader.h"
#include "LookStyle.h"
#include "OscillatorControl.h"
//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Projucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class OscComponent  : public Component,
                      public Slider::Listener
{
public:
    //==============================================================================
    OscComponent ();
    ~OscComponent() override;

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
    /*
    void drawWave(ImageButton *button, int increment = 0);
    float getFrequency();
    float getVolume();
    int getWaveform();
     */
    void setNumber(int n);
    void loadSettings();
    //[/UserMethods]

    void paint (Graphics& g) override;
    void resized() override;
    void sliderValueChanged (Slider* sliderThatWasMoved) override;



private:
    //[UserVariables]   -- You can add your own custom variables in this section.
    WaveSlideLookAndFeel waveLook;
    SlideLookAndFeel slideLook;
    DetuneSlideLookAndFeel detuneLook;
    HarmonicSlideLookAndFeel octaveLook;
    UnipolarSlideLookAndFeel unipolarLook;

    OscillatorControl control;
    int number;
    //[/UserVariables]

    //==============================================================================
    std::unique_ptr<Slider> wave2;
    std::unique_ptr<Slider> octave;
    std::unique_ptr<Slider> frequency;
    std::unique_ptr<Slider> volume;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OscComponent)
};

//[EndFile] You can add extra defines here...
//[/EndFile]

