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
#include "VindorWrap.h"
#include "LookStyle.h"
//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Projucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class NewComponent  : public Component,
                      public Timer,
                      public ChangeListener,
                      public juce::Button::Listener
{
public:
    //==============================================================================
    NewComponent ();
    ~NewComponent() override;

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
    static void callback(VindorControl *object, void *data);
    void updateFirmware();
    void timerCallback() override;
    void changeListenerCallback (ChangeBroadcaster *source) override;
    //[/UserMethods]

    void paint (juce::Graphics& g) override;
    void resized() override;
    void buttonClicked (juce::Button* buttonThatWasClicked) override;
    bool keyPressed (const juce::KeyPress& key) override;



private:
    //[UserVariables]   -- You can add your own custom variables in this section.
    TooltipWindow tooltips;
    TabLookAndFeel tabLook;
    //[/UserVariables]

    //==============================================================================
    std::unique_ptr<juce::TabbedComponent> tabbedComponent;
    std::unique_ptr<juce::HyperlinkButton> www;
    std::unique_ptr<juce::TextButton> saveFlash;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NewComponent)
};

//[EndFile] You can add extra defines here...
//[/EndFile]

