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
#include "VindorWrap.h"
//[/Headers]

#include "SystemTab.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...
class FactorySettingsCallback : public ModalComponentManager::Callback {
public:
    FiatinoControlSub *deviceControl;
    FactorySettingsCallback(FiatinoControlSub *dp)
    {
        deviceControl = dp;
    }
    virtual void modalStateFinished (int returnValue)
    {
        if (returnValue == 1) {
            deviceControl->sendRestoreFactory();
        }
    }
};
//[/MiscUserDefs]

//==============================================================================
SystemTab::SystemTab ()
{
    //[Constructor_pre] You can add your own custom stuff here..
    //[/Constructor_pre]

    groupMIDI.reset (new juce::GroupComponent ("new group",
                                               TRANS("USB MIDI")));
    addAndMakeVisible (groupMIDI.get());

    groupMIDI->setBounds (8, 8, 256, 192);

    midiMessage.reset (new juce::ComboBox ("midi msg"));
    addAndMakeVisible (midiMessage.get());
    midiMessage->setTooltip (TRANS("The MIDI message to send for changes in breath pressure."));
    midiMessage->setEditableText (false);
    midiMessage->setJustificationType (juce::Justification::centredLeft);
    midiMessage->setTextWhenNothingSelected (juce::String());
    midiMessage->setTextWhenNoChoicesAvailable (TRANS("(no choices)"));
    midiMessage->addItem (TRANS("Breath Control"), 1);
    midiMessage->addItem (TRANS("Volume"), 2);
    midiMessage->addItem (TRANS("Expression"), 3);
    midiMessage->addItem (TRANS("Aftertouch"), 4);
    midiMessage->addListener (this);

    midiMessage->setBounds (144, 32, 104, 24);

    label5.reset (new juce::Label ("new label",
                                   TRANS("MIDI messages")));
    addAndMakeVisible (label5.get());
    label5->setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    label5->setJustificationType (juce::Justification::centredLeft);
    label5->setEditable (false, false, false);
    label5->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    label5->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    label5->setBounds (16, 32, 120, 24);

    label6.reset (new juce::Label ("new label",
                                   TRANS("USB interface")));
    addAndMakeVisible (label6.get());
    label6->setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    label6->setJustificationType (juce::Justification::centredLeft);
    label6->setEditable (false, false, false);
    label6->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    label6->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    label6->setBounds (16, 164, 120, 24);

    usbMode.reset (new juce::ComboBox ("usbmode"));
    addAndMakeVisible (usbMode.get());
    usbMode->setTooltip (TRANS("Support for only MIDI messages or to also be a USB Audio device for recording and playback."));
    usbMode->setEditableText (false);
    usbMode->setJustificationType (juce::Justification::centredLeft);
    usbMode->setTextWhenNothingSelected (juce::String());
    usbMode->setTextWhenNoChoicesAvailable (TRANS("(no choices)"));
    usbMode->addItem (TRANS("MIDI & Audio"), 1);
    usbMode->addItem (TRANS("MIDI only"), 2);
    usbMode->addListener (this);

    usbMode->setBounds (144, 164, 104, 24);

    label7.reset (new juce::Label ("new label",
                                   TRANS("MIDI rate (ms)")));
    addAndMakeVisible (label7.get());
    label7->setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    label7->setJustificationType (juce::Justification::centredLeft);
    label7->setEditable (false, false, false);
    label7->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    label7->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    label7->setBounds (16, 131, 120, 24);

    midiRate.reset (new juce::Slider ("midi rate"));
    addAndMakeVisible (midiRate.get());
    midiRate->setTooltip (TRANS("Maximum rate to send messages.  Millisconds between messages."));
    midiRate->setRange (1, 500, 1);
    midiRate->setSliderStyle (juce::Slider::IncDecButtons);
    midiRate->setTextBoxStyle (juce::Slider::TextBoxLeft, false, 40, 20);
    midiRate->addListener (this);

    midiRate->setBounds (144, 131, 104, 24);

    label8.reset (new juce::Label ("new label",
                                   TRANS("MIDI Hires")));
    addAndMakeVisible (label8.get());
    label8->setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    label8->setJustificationType (juce::Justification::centredLeft);
    label8->setEditable (false, false, false);
    label8->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    label8->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    label8->setBounds (16, 98, 120, 24);

    midiHires.reset (new juce::ToggleButton ("midi hires"));
    addAndMakeVisible (midiHires.get());
    midiHires->setTooltip (TRANS("Some synths support high resolution messages."));
    midiHires->setButtonText (TRANS("Enable Hires"));
    midiHires->addListener (this);

    midiHires->setBounds (144, 96, 104, 24);

    groupDevice.reset (new juce::GroupComponent ("new group",
                                                 TRANS("Device")));
    addAndMakeVisible (groupDevice.get());

    groupDevice->setBounds (8, 208, 256, 200);

    factory.reset (new juce::TextButton ("new button"));
    addAndMakeVisible (factory.get());
    factory->setTooltip (TRANS("Wipe out all settings on the Vindor and load the default factory settings."));
    factory->setButtonText (TRANS("Restore Factory Settings"));
    factory->addListener (this);

    factory->setBounds (32, 320, 208, 32);

    firmware.reset (new juce::TextButton ("new button"));
    addAndMakeVisible (firmware.get());
    firmware->setTooltip (TRANS("Manually update the firmware on the Vindor. Normally, you will be notified when a new firmware is available."));
    firmware->setButtonText (TRANS("Update firmware..."));
    firmware->addListener (this);

    firmware->setBounds (32, 360, 208, 32);

    label.reset (new juce::Label ("new label",
                                  TRANS("Idle minutes before shutdown")));
    addAndMakeVisible (label.get());
    label->setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    label->setJustificationType (juce::Justification::centredLeft);
    label->setEditable (false, false, false);
    label->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    label->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    label->setBounds (24, 232, 160, 24);

    idleSleep.reset (new juce::Slider ("new slider"));
    addAndMakeVisible (idleSleep.get());
    idleSleep->setTooltip (TRANS("Number of minutes of idle time before the Vindor enters shutdown mode to preserve battery life in case it is accentally left on."));
    idleSleep->setRange (0, 60, 1);
    idleSleep->setSliderStyle (juce::Slider::IncDecButtons);
    idleSleep->setTextBoxStyle (juce::Slider::TextBoxLeft, false, 60, 20);
    idleSleep->addListener (this);

    idleSleep->setBounds (32, 280, 128, 24);

    midiChannel.reset (new juce::Slider ("new slider"));
    addAndMakeVisible (midiChannel.get());
    midiChannel->setTooltip (TRANS("Channel number for messages from Vindor."));
    midiChannel->setRange (1, 16, 1);
    midiChannel->setSliderStyle (juce::Slider::IncDecButtons);
    midiChannel->setTextBoxStyle (juce::Slider::TextBoxLeft, false, 40, 20);
    midiChannel->addListener (this);

    midiChannel->setBounds (144, 66, 104, 24);

    label2.reset (new juce::Label ("new label",
                                   TRANS("MIDI channel")));
    addAndMakeVisible (label2.get());
    label2->setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    label2->setJustificationType (juce::Justification::centredLeft);
    label2->setEditable (false, false, false);
    label2->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    label2->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    label2->setBounds (16, 66, 120, 24);

    wifiGroupBox.reset (new juce::GroupComponent ("new group",
                                                  TRANS("WiFi")));
    addAndMakeVisible (wifiGroupBox.get());

    wifiGroupBox->setBounds (489, 24, 192, 184);

    wifiEnabled.reset (new juce::ToggleButton ("new toggle button"));
    addAndMakeVisible (wifiEnabled.get());
    wifiEnabled->setButtonText (TRANS("Enabled"));
    wifiEnabled->addListener (this);

    wifiEnabled->setBounds (500, 48, 150, 16);

    wifiSSID.reset (new juce::TextEditor ("new text editor"));
    addAndMakeVisible (wifiSSID.get());
    wifiSSID->setMultiLine (false);
    wifiSSID->setReturnKeyStartsNewLine (false);
    wifiSSID->setReadOnly (false);
    wifiSSID->setScrollbarsShown (true);
    wifiSSID->setCaretVisible (true);
    wifiSSID->setPopupMenuEnabled (true);
    wifiSSID->setText (juce::String());

    wifiSSID->setBounds (500, 96, 150, 16);

    wifiPass.reset (new juce::TextEditor ("new text editor"));
    addAndMakeVisible (wifiPass.get());
    wifiPass->setMultiLine (false);
    wifiPass->setReturnKeyStartsNewLine (false);
    wifiPass->setReadOnly (false);
    wifiPass->setScrollbarsShown (true);
    wifiPass->setCaretVisible (true);
    wifiPass->setPopupMenuEnabled (true);
    wifiPass->setText (juce::String());

    wifiPass->setBounds (500, 160, 150, 16);

    label15.reset (new juce::Label ("new label",
                                    TRANS("(0 for never)")));
    addAndMakeVisible (label15.get());
    label15->setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    label15->setJustificationType (juce::Justification::centredLeft);
    label15->setEditable (false, false, false);
    label15->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    label15->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    label15->setBounds (24, 256, 160, 24);


    //[UserPreSize]
    deviceControl->addChangeListener(this);
    wifiSSID->addListener(this);
    wifiPass->addListener(this);

    wifiGroupBox->setVisible(false);
    wifiEnabled->setVisible(false);
    wifiSSID->setVisible(false);
    wifiPass->setVisible(false);
    //[/UserPreSize]

    setSize (600, 400);


    //[Constructor] You can add your own custom stuff here..
#if JUCE_MAC || JUCE_WINDOWS || JUCE_LINUX
    firmware->setEnabled(true);
#else
    firmware->setEnabled(false);
    firmware->setVisible(false);
#endif

    //[/Constructor]
}

SystemTab::~SystemTab()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

    groupMIDI = nullptr;
    midiMessage = nullptr;
    label5 = nullptr;
    label6 = nullptr;
    usbMode = nullptr;
    label7 = nullptr;
    midiRate = nullptr;
    label8 = nullptr;
    midiHires = nullptr;
    groupDevice = nullptr;
    factory = nullptr;
    firmware = nullptr;
    label = nullptr;
    idleSleep = nullptr;
    midiChannel = nullptr;
    label2 = nullptr;
    wifiGroupBox = nullptr;
    wifiEnabled = nullptr;
    wifiSSID = nullptr;
    wifiPass = nullptr;
    label15 = nullptr;


    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void SystemTab::paint (juce::Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (juce::Colour (0xff323e44));

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void SystemTab::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void SystemTab::comboBoxChanged (juce::ComboBox* comboBoxThatHasChanged)
{
    //[UsercomboBoxChanged_Pre]
    //[/UsercomboBoxChanged_Pre]

    if (comboBoxThatHasChanged == midiMessage.get())
    {
        //[UserComboBoxCode_midiMessage] -- add your combo box handling code here..
        deviceControl->settings.setInt("MIDIVOL", midiMessage->getSelectedId());
        deviceControl->sendSettingName("MIDIVOL");
        //[/UserComboBoxCode_midiMessage]
    }
    else if (comboBoxThatHasChanged == usbMode.get())
    {
        //[UserComboBoxCode_usbMode] -- add your combo box handling code here..
        int value = usbMode->getSelectedId();
        deviceControl->settings.setInt("USBMODE", value);
        deviceControl->sendSettingName("USBMODE");
        //[/UserComboBoxCode_usbMode]
    }

    //[UsercomboBoxChanged_Post]
    //[/UsercomboBoxChanged_Post]
}

void SystemTab::sliderValueChanged (juce::Slider* sliderThatWasMoved)
{
    //[UsersliderValueChanged_Pre]
    //[/UsersliderValueChanged_Pre]

    if (sliderThatWasMoved == midiRate.get())
    {
        //[UserSliderCode_midiRate] -- add your slider handling code here..
        saveMidiCC("MIDIRATE", sliderThatWasMoved);
        //[/UserSliderCode_midiRate]
    }
    else if (sliderThatWasMoved == idleSleep.get())
    {
        //[UserSliderCode_idleSleep] -- add your slider handling code here..
        saveMidiCC("IDLESEC", sliderThatWasMoved);
        //[/UserSliderCode_idleSleep]
    }
    else if (sliderThatWasMoved == midiChannel.get())
    {
        //[UserSliderCode_midiChannel] -- add your slider handling code here..
        saveMidiCC("CHANNEL", sliderThatWasMoved);
        //[/UserSliderCode_midiChannel]
    }

    //[UsersliderValueChanged_Post]
    //[/UsersliderValueChanged_Post]
}

void SystemTab::buttonClicked (juce::Button* buttonThatWasClicked)
{
    //[UserbuttonClicked_Pre]
    //[/UserbuttonClicked_Pre]

    if (buttonThatWasClicked == midiHires.get())
    {
        //[UserButtonCode_midiHires] -- add your button handler code here..
        bool b = midiHires->getToggleState();
        deviceControl->settings.setInt("MIDIHIRES", b);
        deviceControl->sendSettingName("MIDIHIRES");
        //[/UserButtonCode_midiHires]
    }
    else if (buttonThatWasClicked == factory.get())
    {
        //[UserButtonCode_factory] -- add your button handler code here..
        FactorySettingsCallback *db = new FactorySettingsCallback(deviceControl.get());
        int result = AlertWindow::showOkCancelBox(AlertWindow::AlertIconType::QuestionIcon,
            "Factory Settings",
            "This will eliminate all your customizations are return the Vindor to it's factory configuration. "
            "Is this what you want?", "Yes", "No", this, db);
        if (result == 1) {
            deviceControl->sendRestoreFactory();
        }
        //[/UserButtonCode_factory]
    }
    else if (buttonThatWasClicked == firmware.get())
    {
        //[UserButtonCode_firmware] -- add your button handler code here..
#if JUCE_MAC || JUCE_WINDOWS || JUCE_LINUX
        deviceControl->updateFirmware();
#endif
        //[/UserButtonCode_firmware]
    }
    else if (buttonThatWasClicked == wifiEnabled.get())
    {
        //[UserButtonCode_wifiEnabled] -- add your button handler code here..
        bool b = buttonThatWasClicked->getToggleState();
        deviceControl->settings.setInt("WIFIEN", b);
        deviceControl->sendSettingName("WIFIEN");
        //[/UserButtonCode_wifiEnabled]
    }

    //[UserbuttonClicked_Post]
    //[/UserbuttonClicked_Post]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...

void SystemTab::textEditorFocusLost(TextEditor &text)
{
    String s = text.getText();
    const char *x = s.getCharPointer().getAddress();
    if (&text == wifiSSID.get()) {
        deviceControl->settings.setStr("WIFISSID", x);
        deviceControl->sendSettingName("WIFISSID");
    }
    else if (&text == wifiPass.get()) {
        deviceControl->settings.setStr("WIFIPASS", x);
        deviceControl->sendSettingName("WIFIPASS");
    }
}

void SystemTab::changeListenerCallback (ChangeBroadcaster* source)
{
    if (! deviceControl->isValid()) return;

    loadInfo();

#if 0
    if (deviceControl->isValid()) {
        firmware->setEnabled(true);
    }
    else {
        firmware->setEnabled(false);
    }
#endif

//    repaint();
}

void SystemTab::saveMidiCC(const char *name, Slider *slider, int offset) {
    int i = slider->getValue();
    deviceControl->settings.setInt(name, i + offset);
    deviceControl->sendSettingName(name);
}

void SystemTab::loadMidiCC(const char *name, Slider *slider, int offset) {
    slider->setValue(deviceControl->settings.getInt(name) - offset, dontSendNotification);
}

void SystemTab::loadInfo()
{
    midiChannel->setValue(deviceControl->settings.getInt("CHANNEL"), dontSendNotification);
    midiMessage->setSelectedId(deviceControl->settings.getInt("MIDIVOL"), NotificationType::dontSendNotification);
    usbMode->setSelectedId(deviceControl->settings.getInt("USBMODE"), dontSendNotification);
    //usbMode->setSelectedId(deviceControl->settings.UsbMode + 1, NotificationType::dontSendNotification);
    midiRate->setValue(deviceControl->settings.getInt("MIDIRATE"), NotificationType::dontSendNotification);
    int b = deviceControl->settings.getInt("MIDIHIRES");
    midiHires->setToggleState(b, dontSendNotification);
    idleSleep->setValue(deviceControl->settings.getInt("IDLESEC") / 60, dontSendNotification);

    char s[256];
    strcpy(s, deviceControl->settings.getStr("WIFISSID").c_str());
    wifiSSID->setText(s);
    strcpy(s, deviceControl->settings.getStr("WIFIPASS").c_str());
    wifiPass->setText(s);
    wifiEnabled->setToggleState(deviceControl->settings.getInt("WIFIEN"), dontSendNotification);

}
//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Projucer information section --

    This is where the Projucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="SystemTab" componentName=""
                 parentClasses="public Component, public ChangeListener, public TextEditor::Listener"
                 constructorParams="" variableInitialisers="" snapPixels="8" snapActive="1"
                 snapShown="1" overlayOpacity="0.330" fixedSize="0" initialWidth="600"
                 initialHeight="400">
  <BACKGROUND backgroundColour="ff323e44"/>
  <GROUPCOMPONENT name="new group" id="f41062441894a82b" memberName="groupMIDI"
                  virtualName="" explicitFocusOrder="0" pos="8 8 256 192" title="USB MIDI"/>
  <COMBOBOX name="midi msg" id="ad3a48873c2fcce9" memberName="midiMessage"
            virtualName="" explicitFocusOrder="0" pos="144 32 104 24" tooltip="The MIDI message to send for changes in breath pressure."
            editable="0" layout="33" items="Breath Control&#10;Volume&#10;Expression&#10;Aftertouch"
            textWhenNonSelected="" textWhenNoItems="(no choices)"/>
  <LABEL name="new label" id="f0756a7a4d00023d" memberName="label5" virtualName=""
         explicitFocusOrder="0" pos="16 32 120 24" edTextCol="ff000000"
         edBkgCol="0" labelText="MIDI messages" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="15.0" kerning="0.0" bold="0" italic="0" justification="33"/>
  <LABEL name="new label" id="68b6119c5bc3b57d" memberName="label6" virtualName=""
         explicitFocusOrder="0" pos="16 164 120 24" edTextCol="ff000000"
         edBkgCol="0" labelText="USB interface" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="15.0" kerning="0.0" bold="0" italic="0" justification="33"/>
  <COMBOBOX name="usbmode" id="e203388b7c6e7ab2" memberName="usbMode" virtualName=""
            explicitFocusOrder="0" pos="144 164 104 24" tooltip="Support for only MIDI messages or to also be a USB Audio device for recording and playback."
            editable="0" layout="33" items="MIDI &amp; Audio&#10;MIDI only"
            textWhenNonSelected="" textWhenNoItems="(no choices)"/>
  <LABEL name="new label" id="82825338dea1bba6" memberName="label7" virtualName=""
         explicitFocusOrder="0" pos="16 131 120 24" edTextCol="ff000000"
         edBkgCol="0" labelText="MIDI rate (ms)" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="15.0" kerning="0.0" bold="0" italic="0" justification="33"/>
  <SLIDER name="midi rate" id="300e692821f463b4" memberName="midiRate"
          virtualName="" explicitFocusOrder="0" pos="144 131 104 24" tooltip="Maximum rate to send messages.  Millisconds between messages."
          min="1.0" max="500.0" int="1.0" style="IncDecButtons" textBoxPos="TextBoxLeft"
          textBoxEditable="1" textBoxWidth="40" textBoxHeight="20" skewFactor="1.0"
          needsCallback="1"/>
  <LABEL name="new label" id="76095e968a0e6f3d" memberName="label8" virtualName=""
         explicitFocusOrder="0" pos="16 98 120 24" edTextCol="ff000000"
         edBkgCol="0" labelText="MIDI Hires" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15.0"
         kerning="0.0" bold="0" italic="0" justification="33"/>
  <TOGGLEBUTTON name="midi hires" id="d23502d19d818f35" memberName="midiHires"
                virtualName="" explicitFocusOrder="0" pos="144 96 104 24" tooltip="Some synths support high resolution messages."
                buttonText="Enable Hires" connectedEdges="0" needsCallback="1"
                radioGroupId="0" state="0"/>
  <GROUPCOMPONENT name="new group" id="ad24df91e8bfbdab" memberName="groupDevice"
                  virtualName="" explicitFocusOrder="0" pos="8 208 256 200" title="Device"/>
  <TEXTBUTTON name="new button" id="c488b69a4e84395d" memberName="factory"
              virtualName="" explicitFocusOrder="0" pos="32 320 208 32" tooltip="Wipe out all settings on the Vindor and load the default factory settings."
              buttonText="Restore Factory Settings" connectedEdges="0" needsCallback="1"
              radioGroupId="0"/>
  <TEXTBUTTON name="new button" id="e4a9509170ae3f3b" memberName="firmware"
              virtualName="" explicitFocusOrder="0" pos="32 360 208 32" tooltip="Manually update the firmware on the Vindor. Normally, you will be notified when a new firmware is available."
              buttonText="Update firmware..." connectedEdges="0" needsCallback="1"
              radioGroupId="0"/>
  <LABEL name="new label" id="1fa444870c7f2c7c" memberName="label" virtualName=""
         explicitFocusOrder="0" pos="24 232 160 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Idle minutes before shutdown" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="15.0" kerning="0.0" bold="0" italic="0" justification="33"/>
  <SLIDER name="new slider" id="f14b050380e0c641" memberName="idleSleep"
          virtualName="" explicitFocusOrder="0" pos="32 280 128 24" tooltip="Number of minutes of idle time before the Vindor enters shutdown mode to preserve battery life in case it is accentally left on."
          min="0.0" max="60.0" int="1.0" style="IncDecButtons" textBoxPos="TextBoxLeft"
          textBoxEditable="1" textBoxWidth="60" textBoxHeight="20" skewFactor="1.0"
          needsCallback="1"/>
  <SLIDER name="new slider" id="fa18ebb795dc9a90" memberName="midiChannel"
          virtualName="" explicitFocusOrder="0" pos="144 66 104 24" tooltip="Channel number for messages from Vindor."
          min="1.0" max="16.0" int="1.0" style="IncDecButtons" textBoxPos="TextBoxLeft"
          textBoxEditable="1" textBoxWidth="40" textBoxHeight="20" skewFactor="1.0"
          needsCallback="1"/>
  <LABEL name="new label" id="d7d72291da63f06b" memberName="label2" virtualName=""
         explicitFocusOrder="0" pos="16 66 120 24" edTextCol="ff000000"
         edBkgCol="0" labelText="MIDI channel" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="15.0" kerning="0.0" bold="0" italic="0" justification="33"/>
  <GROUPCOMPONENT name="new group" id="c36d8b25ee09435a" memberName="wifiGroupBox"
                  virtualName="" explicitFocusOrder="0" pos="489 24 192 184" title="WiFi"/>
  <TOGGLEBUTTON name="new toggle button" id="cd4a7dbb57d4c795" memberName="wifiEnabled"
                virtualName="" explicitFocusOrder="0" pos="500 48 150 16" buttonText="Enabled"
                connectedEdges="0" needsCallback="1" radioGroupId="0" state="0"/>
  <TEXTEDITOR name="new text editor" id="b40a00d617a0ec3b" memberName="wifiSSID"
              virtualName="" explicitFocusOrder="0" pos="500 96 150 16" initialText=""
              multiline="0" retKeyStartsLine="0" readonly="0" scrollbars="1"
              caret="1" popupmenu="1"/>
  <TEXTEDITOR name="new text editor" id="4c98c2f5e138714" memberName="wifiPass"
              virtualName="" explicitFocusOrder="0" pos="500 160 150 16" initialText=""
              multiline="0" retKeyStartsLine="0" readonly="0" scrollbars="1"
              caret="1" popupmenu="1"/>
  <LABEL name="new label" id="16a3fd8be0e653e" memberName="label15" virtualName=""
         explicitFocusOrder="0" pos="24 256 160 24" edTextCol="ff000000"
         edBkgCol="0" labelText="(0 for never)" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="15.0" kerning="0.0" bold="0" italic="0" justification="33"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]

