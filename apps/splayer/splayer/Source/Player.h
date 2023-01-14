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

const unsigned char MIDI_MFG1=0x00;
const unsigned char MIDI_MFG2=0x20;
const unsigned char MIDI_MFG3=0x50;

class KnobLookAndFeel : public LookAndFeel_V4
{
public:
    Image knob;

    KnobLookAndFeel() {
        knob = ImageCache::getFromMemory (BinaryData::knob_0_100_png, BinaryData::knob_0_100_pngSize);
    }

    void drawRotarySlider (Graphics &g, int x, int y, int width, int height, float sliderPosProportional, const float rotaryStartAngle, const float rotaryEndAngle, Slider &slider)
    {
        const int sz = knob.getWidth();
        const int num = knob.getHeight() / sz - 1;
        const int frame = sliderPosProportional * num;

        int dim = width<height?width:height;

        x += (width - dim) / 2;

        g.drawImage(knob, x, y, dim, dim, 0, frame * sz, sz, sz);
    }
};
//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Projucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class Player  : public AudioAppComponent,
                private MidiInputCallback,
                private MidiKeyboardStateListener,
                public Timer,
                public ChangeListener,
                public juce::ComboBox::Listener,
                public juce::Slider::Listener,
                public juce::Button::Listener
{
public:
    //==============================================================================
    Player ();
    ~Player() override;

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    void loadFileDialog();

    void queryMidiInputs();
    void setMidiInput (int index);

    void handleIncomingMidiMessage (MidiInput* source, const MidiMessage& message) override;
    void handleNoteOn (MidiKeyboardState*, int midiChannel, int midiNoteNumber, float velocity) override;
    void handleNoteOff (MidiKeyboardState*, int midiChannel, int midiNoteNumber, float velocity) override;

    void timerCallback() override;
    virtual void changeListenerCallback(ChangeBroadcaster* source) override;
    //[/UserMethods]

    void paint (juce::Graphics& g) override;
    void resized() override;
    void comboBoxChanged (juce::ComboBox* comboBoxThatHasChanged) override;
    void sliderValueChanged (juce::Slider* sliderThatWasMoved) override;
    void buttonClicked (juce::Button* buttonThatWasClicked) override;



private:
    //[UserVariables]   -- You can add your own custom variables in this section.

    AudioDeviceManager deviceManager;           // [1]
//    ComboBox midiInputList;                     // [2]
    Label midiInputListLabel;
    int lastInputIndex = 0;                     // [3]
    bool isAddingFromMidiInput = false;         // [4]

    MidiKeyboardState keyboardState;            // [5]
    MidiKeyboardComponent keyboardComponent;    // [6]

    TextEditor midiMessagesBox;
    double startTime;

    StringArray midiInputArray;
    Image logo;
    KnobLookAndFeel knobLook;

    double progressValue;
    ScopedPointer<ProgressBar> progress;
    //[/UserVariables]

    //==============================================================================
    std::unique_ptr<juce::ComboBox> soundCategory;
    std::unique_ptr<juce::ComboBox> soundList;
    std::unique_ptr<juce::ComboBox> midiInputList;
    std::unique_ptr<juce::Slider> volumeSlider;
    std::unique_ptr<juce::Slider> roomSlider;
    std::unique_ptr<juce::Label> label;
    std::unique_ptr<juce::Label> label2;
    std::unique_ptr<juce::TextButton> panic;
    std::unique_ptr<juce::HyperlinkButton> www;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Player)
};

//[EndFile] You can add extra defines here...
//[/EndFile]

