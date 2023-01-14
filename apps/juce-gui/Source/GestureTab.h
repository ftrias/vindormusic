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
#include <JuceHeader.h>
//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Projucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class GestureTab  : public Component,
                    public ChangeListener,
                    public juce::Slider::Listener,
                    public juce::ComboBox::Listener
{
public:
    //==============================================================================
    GestureTab ();
    ~GestureTab() override;

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
    void changeListenerCallback(ChangeBroadcaster* source) override;
    void loadInfo();
    void saveMidiCC(const char *name, Slider *slider, int offset);
    void loadMidiCC(const char *name, Slider *slider, int offset);
    //[/UserMethods]

    void paint (juce::Graphics& g) override;
    void resized() override;
    void sliderValueChanged (juce::Slider* sliderThatWasMoved) override;
    void comboBoxChanged (juce::ComboBox* comboBoxThatHasChanged) override;



private:
    //[UserVariables]   -- You can add your own custom variables in this section.
    //[/UserVariables]

    //==============================================================================
    std::unique_ptr<juce::Slider> midiGesture_cw;
    std::unique_ptr<juce::Label> label3;
    std::unique_ptr<juce::Label> label4;
    std::unique_ptr<juce::Label> label9;
    std::unique_ptr<juce::Label> label10;
    std::unique_ptr<juce::Label> label11;
    std::unique_ptr<juce::Label> label12;
    std::unique_ptr<juce::Label> label13;
    std::unique_ptr<juce::Slider> midiGesture_ccw;
    std::unique_ptr<juce::Slider> midiGesture_ccw_up2;
    std::unique_ptr<juce::Slider> midiGesture_ccw_down2;
    std::unique_ptr<juce::Slider> midiGesture_cw_up;
    std::unique_ptr<juce::Slider> midiGesture_cw_down;
    std::unique_ptr<juce::Label> label14;
    std::unique_ptr<juce::Label> label16;
    std::unique_ptr<juce::Label> label17;
    std::unique_ptr<juce::Label> label18;
    std::unique_ptr<juce::Label> label19;
    std::unique_ptr<juce::Label> label;
    std::unique_ptr<juce::ComboBox> enableGesturesType;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GestureTab)
};

//[EndFile] You can add extra defines here...
//[/EndFile]

