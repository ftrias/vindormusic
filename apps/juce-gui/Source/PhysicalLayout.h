/*
  ==============================================================================

  This is an automatically generated GUI class created by the Projucer!

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Created with Projucer version: 6.1.5

  ------------------------------------------------------------------------------

  The Projucer is part of the JUCE library.
  Copyright (c) 2020 - Raw Material Software Limited.

  ==============================================================================
*/

#pragma once

//[Headers]     -- You can add your own extra header files here --
#include "../JuceLibraryCode/JuceHeader.h"
#include "OscillatorControl.h"
#include "LookStyle.h"
//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Projucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class PhysicalLayout  : public Component,
                        public ChangeListener,
                        public juce::Slider::Listener
{
public:
    //==============================================================================
    PhysicalLayout ();
    ~PhysicalLayout() override;

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
	void changeListenerCallback(ChangeBroadcaster* source) override;
	void loadSettings();
    //[/UserMethods]

    void paint (juce::Graphics& g) override;
    void resized() override;
    void sliderValueChanged (juce::Slider* sliderThatWasMoved) override;



private:
    //[UserVariables]   -- You can add your own custom variables in this section.
	UnipolarSlideLookAndFeel unipolarLook;
    HarmonicSlideLookAndFeel hardLook;
    ChorusSlideLookAndFeel chorusLook;
    //[/UserVariables]

    //==============================================================================
    std::unique_ptr<juce::Slider> tone;
    std::unique_ptr<juce::Slider> smoothness;
    std::unique_ptr<juce::Slider> echo;
    std::unique_ptr<juce::Label> label;
    std::unique_ptr<juce::Label> label2;
    std::unique_ptr<juce::Label> label3;
    std::unique_ptr<juce::Slider> base;
    std::unique_ptr<juce::Label> label4;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PhysicalLayout)
};

//[EndFile] You can add extra defines here...
//[/EndFile]

