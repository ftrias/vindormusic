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

//[Headers] You can add your own extra header files here...
#include "VindorWrap.h"
#include "DownloadBrowse.h"
#include "ConvertFiles.h"
//[/Headers]

#include "TuningTab.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...
//[/MiscUserDefs]

//==============================================================================
TuningTab::TuningTab ()
{
    //[Constructor_pre] You can add your own custom stuff here..
    //[/Constructor_pre]

    tune_a4.reset (new Slider ("tunea4"));
    addAndMakeVisible (tune_a4.get());
    tune_a4->setTooltip (TRANS("Frequency of A4 in Hz."));
    tune_a4->setRange (420, 460, 0.1);
    tune_a4->setSliderStyle (Slider::LinearHorizontal);
    tune_a4->setTextBoxStyle (Slider::TextBoxLeft, false, 80, 20);
    tune_a4->addListener (this);

    tune_a4->setBounds (143, 8, 177, 24);

    label.reset (new Label ("new label",
                            TRANS("A - Hz")));
    addAndMakeVisible (label.get());
    label->setFont (Font (15.00f, Font::plain).withTypefaceStyle ("Regular"));
    label->setJustificationType (Justification::centredLeft);
    label->setEditable (false, false, false);
    label->setColour (TextEditor::textColourId, Colours::black);
    label->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    label->setBounds (16, 8, 120, 24);

    label2.reset (new Label ("new label",
                             TRANS("Instrument key")));
    addAndMakeVisible (label2.get());
    label2->setFont (Font (15.00f, Font::plain).withTypefaceStyle ("Regular"));
    label2->setJustificationType (Justification::centredLeft);
    label2->setEditable (false, false, false);
    label2->setColour (TextEditor::textColourId, Colours::black);
    label2->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    label2->setBounds (16, 48, 120, 24);

    keyChoice.reset (new ComboBox ("key"));
    addAndMakeVisible (keyChoice.get());
    keyChoice->setTooltip (TRANS("Key for middle \"C\" fingering. This setting transposes the instrument."));
    keyChoice->setEditableText (false);
    keyChoice->setJustificationType (Justification::centredLeft);
    keyChoice->setTextWhenNothingSelected (String());
    keyChoice->setTextWhenNoChoicesAvailable (TRANS("(no choices)"));
    keyChoice->addListener (this);

    keyChoice->setBounds (143, 48, 80, 24);

    label3.reset (new Label ("new label",
                             TRANS("Octave shift")));
    addAndMakeVisible (label3.get());
    label3->setFont (Font (15.00f, Font::plain).withTypefaceStyle ("Regular"));
    label3->setJustificationType (Justification::centredLeft);
    label3->setEditable (false, false, false);
    label3->setColour (TextEditor::textColourId, Colours::black);
    label3->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    label3->setBounds (16, 115, 120, 24);

    octave.reset (new Slider ("octave"));
    addAndMakeVisible (octave.get());
    octave->setTooltip (TRANS("Number of octaves to shift instrument up or down."));
    octave->setRange (-10, 10, 1);
    octave->setSliderStyle (Slider::IncDecButtons);
    octave->setTextBoxStyle (Slider::TextBoxLeft, false, 80, 20);
    octave->addListener (this);

    octave->setBounds (143, 115, 168, 24);

    instrument.reset (new Slider ("instrument"));
    addAndMakeVisible (instrument.get());
    instrument->setTooltip (TRANS("Instrument profile to use to play notes."));
    instrument->setRange (1, 8, 1);
    instrument->setSliderStyle (Slider::IncDecButtons);
    instrument->setTextBoxStyle (Slider::TextBoxLeft, false, 80, 20);
    instrument->addListener (this);

    instrument->setBounds (143, 155, 168, 24);

    label4.reset (new Label ("new label",
                             TRANS("Intrument sound")));
    addAndMakeVisible (label4.get());
    label4->setFont (Font (15.00f, Font::plain).withTypefaceStyle ("Regular"));
    label4->setJustificationType (Justification::centredLeft);
    label4->setEditable (false, false, false);
    label4->setColour (TextEditor::textColourId, Colours::black);
    label4->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    label4->setBounds (16, 155, 120, 24);

    fingeringLabel.reset (new Label ("new label",
                                     TRANS("Fingering")));
    addAndMakeVisible (fingeringLabel.get());
    fingeringLabel->setFont (Font (15.00f, Font::plain).withTypefaceStyle ("Regular"));
    fingeringLabel->setJustificationType (Justification::centredLeft);
    fingeringLabel->setEditable (false, false, false);
    fingeringLabel->setColour (TextEditor::textColourId, Colours::black);
    fingeringLabel->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    fingeringLabel->setBounds (16, 229, 120, 24);

    fingeringButton.reset (new TextButton ("new button"));
    addAndMakeVisible (fingeringButton.get());
    fingeringButton->setTooltip (TRANS("Alter the fingering on the Vindor for saxophone, clarinet, and other instruments."));
    fingeringButton->setButtonText (TRANS("Change..."));
    fingeringButton->addListener (this);

    fingeringButton->setBounds (144, 229, 120, 24);

    label6.reset (new Label ("new label",
                             TRANS("Octave buttons")));
    addAndMakeVisible (label6.get());
    label6->setFont (Font (15.00f, Font::plain).withTypefaceStyle ("Regular"));
    label6->setJustificationType (Justification::centredLeft);
    label6->setEditable (false, false, false);
    label6->setColour (TextEditor::textColourId, Colours::black);
    label6->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    label6->setBounds (16, 81, 120, 24);

    octMode.reset (new ToggleButton ("new toggle button"));
    addAndMakeVisible (octMode.get());
    octMode->setButtonText (TRANS("Simplified 2 octaves"));
    octMode->addListener (this);

    octMode->setBounds (144, 82, 150, 24);

    label7.reset (new Label ("new label",
                             TRANS("Breath pressure")));
    addAndMakeVisible (label7.get());
    label7->setFont (Font (15.00f, Font::plain).withTypefaceStyle ("Regular"));
    label7->setJustificationType (Justification::centredLeft);
    label7->setEditable (false, false, false);
    label7->setColour (TextEditor::textColourId, Colours::black);
    label7->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    label7->setBounds (16, 192, 120, 24);

    pressure_sensitivity.reset (new Slider ("pressure_sensitivity"));
    addAndMakeVisible (pressure_sensitivity.get());
    pressure_sensitivity->setTooltip (TRANS("Bigger numbers require less pressure to add volume."));
    pressure_sensitivity->setRange (10, 120, 1);
    pressure_sensitivity->setSliderStyle (Slider::LinearHorizontal);
    pressure_sensitivity->setTextBoxStyle (Slider::TextBoxLeft, false, 80, 20);
    pressure_sensitivity->addListener (this);

    pressure_sensitivity->setBounds (144, 192, 177, 24);

    fingeringChartLink.reset (new HyperlinkButton (TRANS("charts"),
                                                   URL ("http://vindormusic.com/fingering")));
    addAndMakeVisible (fingeringChartLink.get());
    fingeringChartLink->setTooltip (TRANS("http://vindormusic.com/fingering"));
    fingeringChartLink->setButtonText (TRANS("charts"));

    fingeringChartLink->setBounds (272, 232, 64, 24);


    //[UserPreSize]
    std::string notes[] = {"C","C#/Db","D","D#/Eb","E","F","F#/Gb","G","G#/Ab","A","A#/Bb","B"};
    keyChoice->clear();
    for (int i=0; i < 12; i++) {
        keyChoice->addItem(notes[i], i+1);
    }

    deviceControl->addChangeListener(this);

    loadInfo();

    // for now, hide until work is complete
//    convertFilesButton->setVisible(false);

#if JUCE_MAC || JUCE_WINDOWS || JUCE_LINUX
#else
    fingeringButton->setVisible(false);
    fingeringLabel->setVisible(false);
	fingeringChartLink->setVisible(false);
#endif
    //[/UserPreSize]

    setSize (600, 400);


    //[Constructor] You can add your own custom stuff here..
    //[/Constructor]
}

TuningTab::~TuningTab()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

    tune_a4 = nullptr;
    label = nullptr;
    label2 = nullptr;
    keyChoice = nullptr;
    label3 = nullptr;
    octave = nullptr;
    instrument = nullptr;
    label4 = nullptr;
    fingeringLabel = nullptr;
    fingeringButton = nullptr;
    label6 = nullptr;
    octMode = nullptr;
    label7 = nullptr;
    pressure_sensitivity = nullptr;
    fingeringChartLink = nullptr;


    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void TuningTab::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (Colour (0xff323e44));

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void TuningTab::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void TuningTab::sliderValueChanged (Slider* sliderThatWasMoved)
{
    //[UsersliderValueChanged_Pre]
    //[/UsersliderValueChanged_Pre]

    if (sliderThatWasMoved == tune_a4.get())
    {
        //[UserSliderCode_tune_a4] -- add your slider handling code here..
        deviceControl->settings.setInt("BASEA4", round(tune_a4->getValue() * 10));
        deviceControl->sendSettingName("BASEA4");
        //[/UserSliderCode_tune_a4]
    }
    else if (sliderThatWasMoved == octave.get())
    {
        //[UserSliderCode_octave] -- add your slider handling code here..
        int baseOct = octave->getValue();
        deviceControl->settings.setInt("BASEOCT", baseOct + 16);
        deviceControl->sendSettingName("BASEOCT");
        //[/UserSliderCode_octave]
    }
    else if (sliderThatWasMoved == instrument.get())
    {
        //[UserSliderCode_instrument] -- add your slider handling code here..
        deviceControl->settings.setInt("INST", instrument->getValue() - 1);
        deviceControl->sendSettingName("INST");
        //[/UserSliderCode_instrument]
    }
    else if (sliderThatWasMoved == pressure_sensitivity.get())
    {
        //[UserSliderCode_pressure_sensitivity] -- add your slider handling code here..
        deviceControl->settings.setInt("PRESFAC", round(pressure_sensitivity->getValue()));
        deviceControl->sendSettingName("PRESFAC");
        //[/UserSliderCode_pressure_sensitivity]
    }

    //[UsersliderValueChanged_Post]
    //[/UsersliderValueChanged_Post]
}

void TuningTab::comboBoxChanged (ComboBox* comboBoxThatHasChanged)
{
    //[UsercomboBoxChanged_Pre]
    //[/UsercomboBoxChanged_Pre]

    if (comboBoxThatHasChanged == keyChoice.get())
    {
        //[UserComboBoxCode_keyChoice] -- add your combo box handling code here..
        int id = keyChoice->getSelectedId();
        if (id>0) {
            int baseNote = (id - 1) + 32;
            deviceControl->settings.setInt("BASENOTE", baseNote);
            deviceControl->sendSettingName("BASENOTE");
        }
        //[/UserComboBoxCode_keyChoice]
    }

    //[UsercomboBoxChanged_Post]
    //[/UsercomboBoxChanged_Post]
}

void TuningTab::buttonClicked (Button* buttonThatWasClicked)
{
    //[UserbuttonClicked_Pre]
    //[/UserbuttonClicked_Pre]

    if (buttonThatWasClicked == fingeringButton.get())
    {
        //[UserButtonCode_fingeringButton] -- add your button handler code here..
        if (! deviceControl->isValid()) return;
#ifndef JUCE_ANDROID
        DownloadBrowse comp;
        DialogWindow::showModalDialog ("Fingering", &comp, nullptr, Colours::white, true, false, false);
#endif
        //[/UserButtonCode_fingeringButton]
    }
    else if (buttonThatWasClicked == octMode.get())
    {
        //[UserButtonCode_octMode] -- add your button handler code here..
        int v = octMode->getToggleState();
        deviceControl->settings.setInt("OCTMODE", v);
        deviceControl->sendSettingName("OCTMODE");
        //[/UserButtonCode_octMode]
    }

    //[UserbuttonClicked_Post]
    //[/UserbuttonClicked_Post]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...
void TuningTab::changeListenerCallback (ChangeBroadcaster* source)
{
    if (! deviceControl->isValid()) return;
    loadInfo();
//    repaint();
}

void TuningTab::loadInfo()
{
    float baseA4 = deviceControl->settings.getInt("BASEA4") / 10.0;
    tune_a4->setValue(baseA4, NotificationType::dontSendNotification);

    int baseNote = deviceControl->settings.getInt("BASENOTE");
    baseNote = (baseNote - 32) % 12;
    if (baseNote < 0) baseNote += 12;
    keyChoice->setSelectedId(baseNote + 1, NotificationType::dontSendNotification);

    int baseOct = deviceControl->settings.getInt("BASEOCT");
    octave->setValue(baseOct - 16, NotificationType::dontSendNotification);

    int b = deviceControl->settings.getInt("OCTMODE");
    octMode->setToggleState(b, dontSendNotification);

    instrument->setValue(deviceControl->settings.getInt("INST") + 1, NotificationType::dontSendNotification);

    int pf = deviceControl->settings.getInt("PRESFAC");
    pressure_sensitivity->setValue(pf, NotificationType::dontSendNotification);

//    int keyLayout = deviceControl->settings.getInt("KEYLAYOUT");
//    fingeringChoice->setSelectedId(keyLayout + 1);
}
//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Projucer information section --

    This is where the Projucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="TuningTab" componentName=""
                 parentClasses="public Component, public ChangeListener" constructorParams=""
                 variableInitialisers="" snapPixels="8" snapActive="1" snapShown="1"
                 overlayOpacity="0.330" fixedSize="0" initialWidth="600" initialHeight="400">
  <BACKGROUND backgroundColour="ff323e44"/>
  <SLIDER name="tunea4" id="4f7a4d0773b91960" memberName="tune_a4" virtualName=""
          explicitFocusOrder="0" pos="143 8 177 24" tooltip="Frequency of A4 in Hz."
          min="420.0" max="460.0" int="0.1" style="LinearHorizontal" textBoxPos="TextBoxLeft"
          textBoxEditable="1" textBoxWidth="80" textBoxHeight="20" skewFactor="1.0"
          needsCallback="1"/>
  <LABEL name="new label" id="c67b965f59953ff8" memberName="label" virtualName=""
         explicitFocusOrder="0" pos="16 8 120 24" edTextCol="ff000000"
         edBkgCol="0" labelText="A - Hz" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15.0"
         kerning="0.0" bold="0" italic="0" justification="33"/>
  <LABEL name="new label" id="f7e66bcce4cc38ee" memberName="label2" virtualName=""
         explicitFocusOrder="0" pos="16 48 120 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Instrument key" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="15.0" kerning="0.0" bold="0" italic="0" justification="33"/>
  <COMBOBOX name="key" id="7b9d8766f037d2e3" memberName="keyChoice" virtualName=""
            explicitFocusOrder="0" pos="143 48 80 24" tooltip="Key for middle &quot;C&quot; fingering. This setting transposes the instrument."
            editable="0" layout="33" items="" textWhenNonSelected="" textWhenNoItems="(no choices)"/>
  <LABEL name="new label" id="bf02b101193bb35e" memberName="label3" virtualName=""
         explicitFocusOrder="0" pos="16 115 120 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Octave shift" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="15.0" kerning="0.0" bold="0" italic="0" justification="33"/>
  <SLIDER name="octave" id="7a920f2fbd83376a" memberName="octave" virtualName=""
          explicitFocusOrder="0" pos="143 115 168 24" tooltip="Number of octaves to shift instrument up or down."
          min="-10.0" max="10.0" int="1.0" style="IncDecButtons" textBoxPos="TextBoxLeft"
          textBoxEditable="1" textBoxWidth="80" textBoxHeight="20" skewFactor="1.0"
          needsCallback="1"/>
  <SLIDER name="instrument" id="fa7c83f349b2b782" memberName="instrument"
          virtualName="" explicitFocusOrder="0" pos="143 155 168 24" tooltip="Instrument profile to use to play notes."
          min="1.0" max="8.0" int="1.0" style="IncDecButtons" textBoxPos="TextBoxLeft"
          textBoxEditable="1" textBoxWidth="80" textBoxHeight="20" skewFactor="1.0"
          needsCallback="1"/>
  <LABEL name="new label" id="476cce4cc303adc9" memberName="label4" virtualName=""
         explicitFocusOrder="0" pos="16 155 120 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Intrument sound" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="15.0" kerning="0.0" bold="0" italic="0" justification="33"/>
  <LABEL name="new label" id="bfff821d69ce8cde" memberName="fingeringLabel"
         virtualName="" explicitFocusOrder="0" pos="16 229 120 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Fingering" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15.0"
         kerning="0.0" bold="0" italic="0" justification="33"/>
  <TEXTBUTTON name="new button" id="990bd15e6f4f653a" memberName="fingeringButton"
              virtualName="" explicitFocusOrder="0" pos="144 229 120 24" tooltip="Alter the fingering on the Vindor for saxophone, clarinet, and other instruments."
              buttonText="Change..." connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <LABEL name="new label" id="dc160287af575826" memberName="label6" virtualName=""
         explicitFocusOrder="0" pos="16 81 120 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Octave buttons" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="15.0" kerning="0.0" bold="0" italic="0" justification="33"/>
  <TOGGLEBUTTON name="new toggle button" id="c81080a22f8051b7" memberName="octMode"
                virtualName="" explicitFocusOrder="0" pos="144 82 150 24" buttonText="Simplified 2 octaves"
                connectedEdges="0" needsCallback="1" radioGroupId="0" state="0"/>
  <LABEL name="new label" id="8b81111ad531b38" memberName="label7" virtualName=""
         explicitFocusOrder="0" pos="16 192 120 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Breath pressure" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="15.0" kerning="0.0" bold="0" italic="0" justification="33"/>
  <SLIDER name="pressure_sensitivity" id="e53f06faca5dca3e" memberName="pressure_sensitivity"
          virtualName="" explicitFocusOrder="0" pos="144 192 177 24" tooltip="Bigger numbers require less pressure to add volume."
          min="10.0" max="120.0" int="1.0" style="LinearHorizontal" textBoxPos="TextBoxLeft"
          textBoxEditable="1" textBoxWidth="80" textBoxHeight="20" skewFactor="1.0"
          needsCallback="1"/>
  <HYPERLINKBUTTON name="fingering chart link" id="465fbc46786b7131" memberName="fingeringChartLink"
                   virtualName="" explicitFocusOrder="0" pos="272 232 64 24" tooltip="http://vindormusic.com/fingering"
                   buttonText="charts" connectedEdges="0" needsCallback="0" radioGroupId="0"
                   url="http://vindormusic.com/fingering"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]

