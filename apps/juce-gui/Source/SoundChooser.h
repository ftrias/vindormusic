/*
  ==============================================================================

  This is an automatically generated GUI class created by the Projucer!

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Created with Projucer version: 6.0.7

  ------------------------------------------------------------------------------

  The Projucer is part of the JUCE library.
  Copyright (c) 2020 - Raw Material Software Limited.

  ==============================================================================
*/

#pragma once

//[Headers]     -- You can add your own extra header files here --
#include "../JuceLibraryCode/JuceHeader.h"
#include "PhysicalLayout.h"
#include "WaveSF2Layout.h"
#include "SoundTab.h"
//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Projucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class SoundChooser  : public Component,
                      public ChangeListener,
                      public juce::ComboBox::Listener,
                      public juce::Button::Listener
{
public:
    //==============================================================================
    SoundChooser ();
    ~SoundChooser() override;

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
	void changeListenerCallback(ChangeBroadcaster* source) override;
    //[/UserMethods]

    void paint (juce::Graphics& g) override;
    void resized() override;
    void comboBoxChanged (juce::ComboBox* comboBoxThatHasChanged) override;
    void buttonClicked (juce::Button* buttonThatWasClicked) override;



private:
    //[UserVariables]   -- You can add your own custom variables in this section.
	std::unique_ptr<PhysicalLayout> physicalModel;
    std::unique_ptr<WaveSF2Layout> waveSF2Model;
	std::unique_ptr<SoundTab> analogModel;
    //[/UserVariables]

    //==============================================================================
    std::unique_ptr<juce::ComboBox> modelChoice;
    std::unique_ptr<juce::ComboBox> instrumentChoice;
    std::unique_ptr<juce::TextButton> playButton;
    std::unique_ptr<juce::ToggleButton> enableSpeakerToggle;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SoundChooser)
};

//[EndFile] You can add extra defines here...
//[/EndFile]

