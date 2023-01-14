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

//[Headers] You can add your own extra header files here...
//[/Headers]

#include "NewComponent.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...
#include "TuningTab.h"
#include "SoundChooser.h"
#include "StatusTab.h"
#include "SystemTab.h"
#include "ConvertFiles.h"
#include "VindorWrap.h"
#include "LookStyle.h"
#include "GestureTab.h"
//[/MiscUserDefs]

//==============================================================================
NewComponent::NewComponent ()
{
    //[Constructor_pre] You can add your own custom stuff here..
    //[/Constructor_pre]

    tabbedComponent.reset (new juce::TabbedComponent (juce::TabbedButtonBar::TabsAtTop));
    addAndMakeVisible (tabbedComponent.get());
    tabbedComponent->setTabBarDepth (40);
    tabbedComponent->addTab (TRANS("Status"), juce::Colour (0x00d3d3d3), new StatusTab(), true);
    tabbedComponent->addTab (TRANS("Sound"), juce::Colour (0x00d3d3d3), new SoundChooser(), true);
    tabbedComponent->addTab (TRANS("Settings"), juce::Colour (0x00d3d3d3), new TuningTab(), true);
    tabbedComponent->addTab (TRANS("Gesture"), juce::Colour (0x00d3d3d3), new GestureTab(), true);
    tabbedComponent->addTab (TRANS("System"), juce::Colour (0x00d3d3d3), new SystemTab(), true);
    tabbedComponent->setCurrentTabIndex (0);

    www.reset (new juce::HyperlinkButton (TRANS("vindormusic.com"),
                                          URL ("http://www.vindormusic.com")));
    addAndMakeVisible (www.get());
    www->setTooltip (TRANS("http://www.vindormusic.com"));
    www->setButtonText (TRANS("vindormusic.com"));

    saveFlash.reset (new juce::TextButton ("saveflash"));
    addAndMakeVisible (saveFlash.get());
    saveFlash->setTooltip (TRANS("Save settings permanently on Vindor."));
    saveFlash->setButtonText (TRANS("Save Between Restarts"));
    saveFlash->addListener (this);


    //[UserPreSize]

#if 0
#if JUCE_MAC || JUCE_WINDOWS || JUCE_LINUX
    tabbedComponent->addTab (TRANS("SD Card"), Colours::lightgrey, new ConvertFiles(), true);
#endif
#endif

    deviceControl->setCallback(callback, this);
    tabbedComponent->setLookAndFeel(&tabLook);
#ifdef DEBUG
    tabbedComponent->setEnabled(true);
#else
    tabbedComponent->setEnabled(false);
#endif

    //[/UserPreSize]

    setSize (400, 650);


    //[Constructor] You can add your own custom stuff here..
    saveFlash->setEnabled(false);

    startTimer(5000);
    deviceControl->addChangeListener(this);
    //[/Constructor]
}

NewComponent::~NewComponent()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

    tabbedComponent = nullptr;
    www = nullptr;
    saveFlash = nullptr;


    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void NewComponent::paint (juce::Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (juce::Colour (0xff323e44));

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void NewComponent::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    tabbedComponent->setBounds (0, 0, proportionOfWidth (1.0000f), getHeight() - 40);
    www->setBounds (10, getHeight() - 30, 110, 20);
    saveFlash->setBounds (getWidth() - 231, getHeight() - 30, 220, 24);
    //[UserResized] Add your own custom resize handling here..
//    www->setBounds(getParentWidth() - www->getWidth(), 0, www->getWidth(), www->getHeight());
#if JUCE_IOS
    auto platform = SystemStats::getDeviceDescription();
    if (platform.startsWith("iPhone")) {
        int idx = platform.indexOfChar(' ');
        if (idx <= 0) idx = 6;
        auto ver = platform.substring(idx).getCharPointer();
        int v = atoi(ver);
        if( v > 8)
        {
            // add some space for the iPhone X and later "cutout" on top of screen
            tabbedComponent->setBounds (0, 40, proportionOfWidth (1.0000f), getHeight() - 120);
            www->setBounds (10, getHeight() - 60, 110, 20);
            saveFlash->setBounds (getWidth() - 231, getHeight() - 60, 220, 24);
        }
    }
#endif
    //[/UserResized]
}

void NewComponent::buttonClicked (juce::Button* buttonThatWasClicked)
{
    //[UserbuttonClicked_Pre]
    //[/UserbuttonClicked_Pre]

    if (buttonThatWasClicked == saveFlash.get())
    {
        //[UserButtonCode_saveFlash] -- add your button handler code here..
        int result = AlertWindow::showOkCancelBox(AlertWindow::AlertIconType::QuestionIcon,
                                                  "Apply Settings",
                                                  "This will overwrite the power-on settings on the Vindor. "
                                                  "Is this what you want?", "Yes", "No", nullptr, nullptr);
        if (result == 1) {
            deviceControl->sendSaveToFirmware();
        }
        //[/UserButtonCode_saveFlash]
    }

    //[UserbuttonClicked_Post]
    //[/UserbuttonClicked_Post]
}

bool NewComponent::keyPressed (const juce::KeyPress& key)
{
    //[UserCode_keyPressed] -- Add your code here...
    if (key.getModifiers() == ModifierKeys::ctrlModifier) { // Ctrl key
        if (key.isKeyCode(77)) { // Ctrl-M

#if JUCE_MAC || JUCE_WINDOWS || JUCE_LINUX
            StringArray inp;
            for (auto& v : deviceControl->inlist) {
                inp.add(v.name);
            }

            StringArray outp;
            for (auto& v : deviceControl->outlist) {
                inp.add(v.name);
            }

            AlertWindow::showMessageBox(AlertWindow::AlertIconType::InfoIcon, "MIDI",
                                        "IN:"+inp.joinIntoString(",") +
                                        "\nOUT:"+outp.joinIntoString(","));
#endif
            return true;
        }
        else if (key.isKeyCode(70)) { // Ctlr-F
#if JUCE_MAC || JUCE_WINDOWS || JUCE_LINUX
            deviceControl->updateFirmware();
            return true;
#endif
        }
        else if (key.isKeyCode(82)) { // Ctlr-R
            deviceControl->openVindor();
            return true;
        }
#if 0
        else if (key.isKeyCode(84)) { // Ctlr-T
            deviceControl->remoteFile.openFile("/test.txt", 2);
            char buf[1024];
            for(int i=0; i<1024; i++) {
                buf[i] = 'A' + i%20;
            }
            deviceControl->remoteFile.writeFile(buf, 1024);
            deviceControl->remoteFile.closeFile();
            return true;
        }
#endif
    }
    return false;  // Return true if your handler uses this key event, or false to allow it to be passed-on.
    //[/UserCode_keyPressed]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...
void NewComponent::callback(VindorControl *obj, void *data)
{
//    NewComponent *me = (NewComponent*)data;
//    int sz = me->tabbedComponent->getNumTabs();
//    for(int i=0; i<sz; i++) {
//        Component *c = me->tabbedComponent->getTabContentComponent(i);
//        if (c->isVisible()) {
//            c->repaint();
//        }
//    }
}

void NewComponent::timerCallback()
{
    if (deviceControl->isValid()) {
        stopTimer();
#if JUCE_MAC || JUCE_WINDOWS || JUCE_LINUX
        deviceControl->testUpdate();
#endif
    }
}

void NewComponent::changeListenerCallback (ChangeBroadcaster *source) {
// comment out to enable changes when disconnected for testing
#if 1
    if (deviceControl->isValid()) {
        tabbedComponent->setEnabled(true);
        saveFlash->setEnabled(true);
    }
    else {
        tabbedComponent->setCurrentTabIndex(0);
        tabbedComponent->setEnabled(false);
        saveFlash->setEnabled(false);
    }
#endif
}

//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Projucer information section --

    This is where the Projucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="NewComponent" componentName=""
                 parentClasses="public Component, public Timer, public ChangeListener"
                 constructorParams="" variableInitialisers="" snapPixels="8" snapActive="1"
                 snapShown="1" overlayOpacity="0.330" fixedSize="0" initialWidth="400"
                 initialHeight="650">
  <METHODS>
    <METHOD name="keyPressed (const KeyPress&amp; key)"/>
  </METHODS>
  <BACKGROUND backgroundColour="ff323e44"/>
  <TABBEDCOMPONENT name="MainTab" id="96ea4966f11b40aa" memberName="tabbedComponent"
                   virtualName="" explicitFocusOrder="0" pos="0 0 100% 40M" orientation="top"
                   tabBarDepth="40" initialTab="0">
    <TAB name="Status" colour="d3d3d3" useJucerComp="0" contentClassName="StatusTab"
         constructorParams="" jucerComponentFile="InfoTab.cpp"/>
    <TAB name="Sound" colour="d3d3d3" useJucerComp="0" contentClassName="SoundChooser"
         constructorParams="" jucerComponentFile=""/>
    <TAB name="Settings" colour="d3d3d3" useJucerComp="0" contentClassName="TuningTab"
         constructorParams="" jucerComponentFile=""/>
    <TAB name="Gesture" colour="d3d3d3" useJucerComp="0" contentClassName="GestureTab"
         constructorParams="" jucerComponentFile=""/>
    <TAB name="System" colour="d3d3d3" useJucerComp="0" contentClassName="SystemTab"
         constructorParams="" jucerComponentFile=""/>
  </TABBEDCOMPONENT>
  <HYPERLINKBUTTON name="www" id="451cda8e4fab584d" memberName="www" virtualName=""
                   explicitFocusOrder="0" pos="10 30R 110 20" tooltip="http://www.vindormusic.com"
                   buttonText="vindormusic.com" connectedEdges="0" needsCallback="0"
                   radioGroupId="0" url="http://www.vindormusic.com"/>
  <TEXTBUTTON name="saveflash" id="fb65c7187748f054" memberName="saveFlash"
              virtualName="" explicitFocusOrder="0" pos="231R 30R 220 24" tooltip="Save settings permanently on Vindor."
              buttonText="Save Between Restarts" connectedEdges="0" needsCallback="1"
              radioGroupId="0"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]

