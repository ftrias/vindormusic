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
#include "OscComponent.h"
#include "LookStyle.h"
//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Projucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class SoundTab  : public Component,
                  public ChangeListener,
                  public juce::Slider::Listener,
                  public juce::Button::Listener
{
public:
    //==============================================================================
    SoundTab ();
    ~SoundTab() override;

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
    void loadSettings();
    int waveSlide2Id(int value);
    int id2waveSlide(int value);
    void dumpSettings();

    int *store_x;
    int *store_y;

    int prev_w = 0;
    int prev_h = 0;
    void scaleComponents(Component *parent, float w, float h);
    void moveComponents(int x, bool absolute=false);

    void changeListenerCallback (ChangeBroadcaster *source) override;
    //[/UserMethods]

    void paint (juce::Graphics& g) override;
    void resized() override;
    void sliderValueChanged (juce::Slider* sliderThatWasMoved) override;
    void buttonClicked (juce::Button* buttonThatWasClicked) override;



private:
    //[UserVariables]   -- You can add your own custom variables in this section.
    KnobLookAndFeel knobLook;
    UnipolarSlideLookAndFeel unipolarLook;
    ChorusSlideLookAndFeel chorusLook;
    CutoffKnobLookAndFeel cutoffLook;
    ResonanceKnobLookAndFeel resonanceLook;
    LFOFreqKnobLookAndFeel lfoFreqLook;
    LFODelayKnobLookAndFeel lfoDelayLook;
    WaveSlideLookAndFeel lfoWaveLook;
    UnipolarSlideLookAndFeel lfoLevelLook;
    MidiInstrumentLookAndFeel midiLook;
    //[/UserVariables]

    //==============================================================================
    std::unique_ptr<juce::GroupComponent> groupComponent;
    std::unique_ptr<juce::GroupComponent> groupComponent2;
    std::unique_ptr<juce::GroupComponent> groupComponent4;
    std::unique_ptr<juce::Slider> resonance;
    std::unique_ptr<OscComponent> osc1;
    std::unique_ptr<juce::Slider> cutoff;
    std::unique_ptr<OscComponent> osc2;
    std::unique_ptr<OscComponent> osc3;
    std::unique_ptr<juce::Slider> noise;
    std::unique_ptr<juce::Label> label;
    std::unique_ptr<juce::Label> label2;
    std::unique_ptr<juce::Label> label3;
    std::unique_ptr<juce::Slider> lfoFreq;
    std::unique_ptr<juce::Slider> lfoDelay;
    std::unique_ptr<juce::Label> label10;
    std::unique_ptr<juce::Label> label12;
    std::unique_ptr<juce::ToggleButton> attack;
    std::unique_ptr<juce::Slider> lfoWave;
    std::unique_ptr<juce::Slider> lfoLevel;
    std::unique_ptr<juce::Label> label11;
    std::unique_ptr<juce::Label> label13;
    std::unique_ptr<juce::ImageButton> page1;
    std::unique_ptr<juce::ImageButton> page2;
    std::unique_ptr<juce::Label> label5;
    std::unique_ptr<juce::Label> label6;
    std::unique_ptr<juce::Label> label7;
    std::unique_ptr<juce::Label> label8;
    std::unique_ptr<juce::Slider> chorus;
    std::unique_ptr<juce::Label> label4;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SoundTab)
};

//[EndFile] You can add extra defines here...
//[/EndFile]

