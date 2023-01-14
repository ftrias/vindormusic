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
//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Projucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class TuningTab  : public Component,
                   public ChangeListener,
                   public Slider::Listener,
                   public ComboBox::Listener,
                   public Button::Listener
{
public:
    //==============================================================================
    TuningTab ();
    ~TuningTab() override;

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
    virtual void changeListenerCallback (ChangeBroadcaster* source) override;
    void loadInfo();
    //[/UserMethods]

    void paint (Graphics& g) override;
    void resized() override;
    void sliderValueChanged (Slider* sliderThatWasMoved) override;
    void comboBoxChanged (ComboBox* comboBoxThatHasChanged) override;
    void buttonClicked (Button* buttonThatWasClicked) override;



private:
    //[UserVariables]   -- You can add your own custom variables in this section.
    //[/UserVariables]

    //==============================================================================
    std::unique_ptr<Slider> tune_a4;
    std::unique_ptr<Label> label;
    std::unique_ptr<Label> label2;
    std::unique_ptr<ComboBox> keyChoice;
    std::unique_ptr<Label> label3;
    std::unique_ptr<Slider> octave;
    std::unique_ptr<Slider> instrument;
    std::unique_ptr<Label> label4;
    std::unique_ptr<Label> fingeringLabel;
    std::unique_ptr<TextButton> fingeringButton;
    std::unique_ptr<Label> label6;
    std::unique_ptr<ToggleButton> octMode;
    std::unique_ptr<Label> label7;
    std::unique_ptr<Slider> pressure_sensitivity;
    std::unique_ptr<HyperlinkButton> fingeringChartLink;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TuningTab)
};

//[EndFile] You can add extra defines here...
//[/EndFile]

