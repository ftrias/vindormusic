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

//[Headers] You can add your own extra header files here...
#include "VindorWrap.h"
//[/Headers]

#include "StatusTab.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...
//[/MiscUserDefs]

//==============================================================================
StatusTab::StatusTab ()
{
    //[Constructor_pre] You can add your own custom stuff here..
    //[/Constructor_pre]

    status.reset (new juce::Label ("status",
                                   TRANS("Status: Disconneced.\n"
                                   "Please connect Vindor ES to USB.")));
    addAndMakeVisible (status.get());
    status->setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    status->setJustificationType (juce::Justification::topLeft);
    status->setEditable (false, false, false);
    status->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    status->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    status->setBounds (8, 8, 296, 40);

    component.reset (new InfoTab());
    addAndMakeVisible (component.get());
    component->setName ("new component");


    //[UserPreSize]
    //[/UserPreSize]

    setSize (600, 400);


    //[Constructor] You can add your own custom stuff here..
    deviceControl->addChangeListener(this);
    //[/Constructor]
}

StatusTab::~StatusTab()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

    status = nullptr;
    component = nullptr;


    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void StatusTab::paint (juce::Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (juce::Colour (0xff323e44));

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void StatusTab::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    component->setBounds (0, 80, proportionOfWidth (1.0000f), getHeight() - 80);
    //[UserResized] Add your own custom resize handling here..
#if 0
    int y = status->getY() + status->getHeight() + 10;
#if JUCE_MAC || JUCE_WINDOWS || JUCE_LINUX
    const int margin = 4;
#else
    const int margin = 0;
#endif
    component->setBounds(margin, y, proportionOfWidth(1.0) - margin*2, getParentHeight() - y);
#endif
    //[/UserResized]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...
void StatusTab::changeListenerCallback (ChangeBroadcaster *source) {
    const char *stat;
    switch(deviceControl->connectState) {
        case 10: stat = "Status: Connecting..."; break;
        case 5: {
            stat = "Status: Firmware not supported";
            break;
        }
        case 4: stat = "Status: Timeout while connecting"; break;
        case 1: stat = "Status: Probing..."; break;
        case 0: {
            stat = "Status: Disconnected";
            break;
        }
        case 3: {
            if (deviceControl->hasUpdate()) {
                stat = "Status: Needs firmware update";
                break;
            }
            else if (deviceControl->isValid()) {
                stat = "Status: Connected";
                break;
            }
            else {
                stat = "Status: Error connecting";
                break;
            }
        }
        default: stat = "Status: Unknown"; break;
    }
    status->setText(stat, NotificationType::dontSendNotification);

    if (deviceControl->connectState==5 || deviceControl->needUpdate()) {

#if JUCE_ANDROID || JUCE_IOS
        AlertWindow::showMessageBoxAsync(AlertWindow::AlertIconType::WarningIcon, "Update Required",
            "The Vindor ES firmware is not compatible with this version of the software. Please run the desktop app to update the firmware.");
#else
        if (deviceControl->needUpdate()) {
            int r = AlertWindow::showOkCancelBox(AlertWindow::AlertIconType::QuestionIcon, String("Update Required"),
                     String("An update to the Vindor ES firmware is required. Would you like to download the update now?"),
                     String("Yes"), String("No"));
            if (r == 1) {
                bool res = deviceControl->performUpdate();
                if (! res) {
                    AlertWindow::showMessageBox(AlertWindow::AlertIconType::WarningIcon, "Update Error",
                                                deviceControl->LastError);
                }
            }
        }
        else {
            AlertWindow::showMessageBox(AlertWindow::AlertIconType::WarningIcon, "Update Required",
                 "The Vindor ES firmware is not compatible with this version of the software. Please contact support to update the firmware.");
        }
#endif
    }
}
//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Projucer information section --

    This is where the Projucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="StatusTab" componentName=""
                 parentClasses="public Component, public ChangeListener" constructorParams=""
                 variableInitialisers="" snapPixels="8" snapActive="1" snapShown="1"
                 overlayOpacity="0.330" fixedSize="0" initialWidth="600" initialHeight="400">
  <BACKGROUND backgroundColour="ff323e44"/>
  <LABEL name="status" id="5a6c1019e5f88622" memberName="status" virtualName=""
         explicitFocusOrder="0" pos="8 8 296 40" edTextCol="ff000000"
         edBkgCol="0" labelText="Status: Disconneced.&#10;Please connect Vindor ES to USB."
         editableSingleClick="0" editableDoubleClick="0" focusDiscardsChanges="0"
         fontname="Default font" fontsize="15.0" kerning="0.0" bold="0"
         italic="0" justification="9"/>
  <GENERICCOMPONENT name="new component" id="ff22007faab244ca" memberName="component"
                    virtualName="" explicitFocusOrder="0" pos="0 80 100% 80M" class="InfoTab"
                    params=""/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]

