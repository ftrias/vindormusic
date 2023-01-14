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
//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Projucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class SystemTab  : public Component,
                   public ChangeListener,
                   public TextEditor::Listener,
                   public juce::ComboBox::Listener,
                   public juce::Slider::Listener,
                   public juce::Button::Listener
{
public:
    //==============================================================================
    SystemTab ();
    ~SystemTab() override;

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
    void changeListenerCallback (ChangeBroadcaster* source) override;
    void setGestureVisible(bool status);
    void loadInfo();
    void buttonClickedCallback();
    void saveMidiCC(const char *name, Slider *slider, int offset = 0);
    void loadMidiCC(const char *name, Slider *slider, int offset = 0);

    virtual void textEditorFocusLost(TextEditor &text) override;

    //[/UserMethods]

    void paint (juce::Graphics& g) override;
    void resized() override;
    void comboBoxChanged (juce::ComboBox* comboBoxThatHasChanged) override;
    void sliderValueChanged (juce::Slider* sliderThatWasMoved) override;
    void buttonClicked (juce::Button* buttonThatWasClicked) override;



private:
    //[UserVariables]   -- You can add your own custom variables in this section.
    //[/UserVariables]

    //==============================================================================
    std::unique_ptr<juce::GroupComponent> groupMIDI;
    std::unique_ptr<juce::ComboBox> midiMessage;
    std::unique_ptr<juce::Label> label5;
    std::unique_ptr<juce::Label> label6;
    std::unique_ptr<juce::ComboBox> usbMode;
    std::unique_ptr<juce::Label> label7;
    std::unique_ptr<juce::Slider> midiRate;
    std::unique_ptr<juce::Label> label8;
    std::unique_ptr<juce::ToggleButton> midiHires;
    std::unique_ptr<juce::GroupComponent> groupDevice;
    std::unique_ptr<juce::TextButton> factory;
    std::unique_ptr<juce::TextButton> firmware;
    std::unique_ptr<juce::Label> label;
    std::unique_ptr<juce::Slider> idleSleep;
    std::unique_ptr<juce::Slider> midiChannel;
    std::unique_ptr<juce::Label> label2;
    std::unique_ptr<juce::GroupComponent> wifiGroupBox;
    std::unique_ptr<juce::ToggleButton> wifiEnabled;
    std::unique_ptr<juce::TextEditor> wifiSSID;
    std::unique_ptr<juce::TextEditor> wifiPass;
    std::unique_ptr<juce::Label> label15;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SystemTab)
};

//[EndFile] You can add extra defines here...
//[/EndFile]

