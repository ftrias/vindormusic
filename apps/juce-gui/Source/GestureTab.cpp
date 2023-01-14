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

#include "GestureTab.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...
//[/MiscUserDefs]

//==============================================================================
GestureTab::GestureTab ()
{
    //[Constructor_pre] You can add your own custom stuff here..
    //[/Constructor_pre]

    midiGesture_cw.reset (new juce::Slider ("new slider"));
    addAndMakeVisible (midiGesture_cw.get());
    midiGesture_cw->setTooltip (TRANS("MIDI Control Code."));
    midiGesture_cw->setRange (-2, 127, 1);
    midiGesture_cw->setSliderStyle (juce::Slider::IncDecButtons);
    midiGesture_cw->setTextBoxStyle (juce::Slider::TextBoxLeft, false, 60, 20);
    midiGesture_cw->addListener (this);

    midiGesture_cw->setBounds (85, 139, 124, 24);

    label3.reset (new juce::Label ("new label",
                                   TRANS("CW")));
    addAndMakeVisible (label3.get());
    label3->setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    label3->setJustificationType (juce::Justification::centredRight);
    label3->setEditable (false, false, false);
    label3->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    label3->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    label3->setBounds (13, 139, 63, 24);

    label4.reset (new juce::Label ("new label",
                                   TRANS("MIDI\n"
                                   "Control*")));
    addAndMakeVisible (label4.get());
    label4->setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    label4->setJustificationType (juce::Justification::centred);
    label4->setEditable (false, false, false);
    label4->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    label4->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    label4->setBounds (105, 88, 80, 48);

    label9.reset (new juce::Label ("new label",
                                   TRANS("CCW")));
    addAndMakeVisible (label9.get());
    label9->setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    label9->setJustificationType (juce::Justification::centredRight);
    label9->setEditable (false, false, false);
    label9->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    label9->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    label9->setBounds (13, 163, 63, 24);

    label10.reset (new juce::Label ("new label",
                                    TRANS("CCW + Up")));
    addAndMakeVisible (label10.get());
    label10->setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    label10->setJustificationType (juce::Justification::centredLeft);
    label10->setEditable (false, false, false);
    label10->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    label10->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    label10->setBounds (212, 235, 120, 24);

    label11.reset (new juce::Label ("new label",
                                    TRANS("CCW + Down")));
    addAndMakeVisible (label11.get());
    label11->setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    label11->setJustificationType (juce::Justification::centredLeft);
    label11->setEditable (false, false, false);
    label11->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    label11->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    label11->setBounds (212, 259, 120, 24);

    label12.reset (new juce::Label ("new label",
                                    TRANS("Tilt up")));
    addAndMakeVisible (label12.get());
    label12->setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    label12->setJustificationType (juce::Justification::centredRight);
    label12->setEditable (false, false, false);
    label12->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    label12->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    label12->setBounds (21, 187, 55, 24);

    label13.reset (new juce::Label ("new label",
                                    TRANS("Tilt down")));
    addAndMakeVisible (label13.get());
    label13->setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    label13->setJustificationType (juce::Justification::centredRight);
    label13->setEditable (false, false, false);
    label13->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    label13->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    label13->setBounds (5, 211, 71, 24);

    midiGesture_ccw.reset (new juce::Slider ("new slider"));
    addAndMakeVisible (midiGesture_ccw.get());
    midiGesture_ccw->setTooltip (TRANS("MIDI control code."));
    midiGesture_ccw->setRange (-2, 127, 1);
    midiGesture_ccw->setSliderStyle (juce::Slider::IncDecButtons);
    midiGesture_ccw->setTextBoxStyle (juce::Slider::TextBoxLeft, false, 60, 20);
    midiGesture_ccw->addListener (this);

    midiGesture_ccw->setBounds (85, 163, 124, 24);

    midiGesture_ccw_up2.reset (new juce::Slider ("new slider"));
    addAndMakeVisible (midiGesture_ccw_up2.get());
    midiGesture_ccw_up2->setTooltip (TRANS("MIDI control code."));
    midiGesture_ccw_up2->setRange (-2, 127, 1);
    midiGesture_ccw_up2->setSliderStyle (juce::Slider::IncDecButtons);
    midiGesture_ccw_up2->setTextBoxStyle (juce::Slider::TextBoxLeft, false, 60, 20);
    midiGesture_ccw_up2->addListener (this);

    midiGesture_ccw_up2->setBounds (85, 235, 124, 24);

    midiGesture_ccw_down2.reset (new juce::Slider ("new slider"));
    addAndMakeVisible (midiGesture_ccw_down2.get());
    midiGesture_ccw_down2->setTooltip (TRANS("MIDI control code."));
    midiGesture_ccw_down2->setRange (-2, 127, 1);
    midiGesture_ccw_down2->setSliderStyle (juce::Slider::IncDecButtons);
    midiGesture_ccw_down2->setTextBoxStyle (juce::Slider::TextBoxLeft, false, 60, 20);
    midiGesture_ccw_down2->addListener (this);

    midiGesture_ccw_down2->setBounds (85, 259, 124, 24);

    midiGesture_cw_up.reset (new juce::Slider ("new slider"));
    addAndMakeVisible (midiGesture_cw_up.get());
    midiGesture_cw_up->setTooltip (TRANS("MIDI control code."));
    midiGesture_cw_up->setRange (-2, 127, 1);
    midiGesture_cw_up->setSliderStyle (juce::Slider::IncDecButtons);
    midiGesture_cw_up->setTextBoxStyle (juce::Slider::TextBoxLeft, false, 60, 20);
    midiGesture_cw_up->addListener (this);

    midiGesture_cw_up->setBounds (85, 187, 124, 24);

    midiGesture_cw_down.reset (new juce::Slider ("new slider"));
    addAndMakeVisible (midiGesture_cw_down.get());
    midiGesture_cw_down->setTooltip (TRANS("MIDI control code."));
    midiGesture_cw_down->setRange (-2, 127, 1);
    midiGesture_cw_down->setSliderStyle (juce::Slider::IncDecButtons);
    midiGesture_cw_down->setTextBoxStyle (juce::Slider::TextBoxLeft, false, 60, 20);
    midiGesture_cw_down->addListener (this);

    midiGesture_cw_down->setBounds (85, 211, 124, 24);

    label14.reset (new juce::Label ("new label",
                                    TRANS("*Control: -1 = pitch up; -2 = pitch down.")));
    addAndMakeVisible (label14.get());
    label14->setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    label14->setJustificationType (juce::Justification::centredLeft);
    label14->setEditable (false, false, false);
    label14->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    label14->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    label14->setBounds (16, 291, 280, 24);

    label16.reset (new juce::Label ("new label",
                                    TRANS("When using button")));
    addAndMakeVisible (label16.get());
    label16->setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    label16->setJustificationType (juce::Justification::centredRight);
    label16->setEditable (false, false, false);
    label16->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    label16->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    label16->setBounds (8, 88, 80, 47);

    label17.reset (new juce::Label ("new label",
                                    TRANS("CW + Up ")));
    addAndMakeVisible (label17.get());
    label17->setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    label17->setJustificationType (juce::Justification::centredLeft);
    label17->setEditable (false, false, false);
    label17->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    label17->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    label17->setBounds (212, 187, 136, 24);

    label18.reset (new juce::Label ("new label",
                                    TRANS("CW + Down")));
    addAndMakeVisible (label18.get());
    label18->setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    label18->setJustificationType (juce::Justification::centredLeft);
    label18->setEditable (false, false, false);
    label18->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    label18->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    label18->setBounds (212, 211, 136, 24);

    label19.reset (new juce::Label ("new label",
                                    TRANS("Without button")));
    addAndMakeVisible (label19.get());
    label19->setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    label19->setJustificationType (juce::Justification::centredLeft);
    label19->setEditable (false, false, false);
    label19->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    label19->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    label19->setBounds (208, 88, 112, 48);

    label.reset (new juce::Label ("new label",
                                  TRANS("CW = Clockwise rotation\n"
                                  "CCW = Counter clockwise")));
    addAndMakeVisible (label.get());
    label->setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    label->setJustificationType (juce::Justification::centredLeft);
    label->setEditable (false, false, false);
    label->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    label->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    label->setBounds (8, 44, 239, 40);

    enableGesturesType.reset (new juce::ComboBox ("new combo box"));
    addAndMakeVisible (enableGesturesType.get());
    enableGesturesType->setEditableText (false);
    enableGesturesType->setJustificationType (juce::Justification::centredLeft);
    enableGesturesType->setTextWhenNothingSelected (juce::String());
    enableGesturesType->setTextWhenNoChoicesAvailable (TRANS("(no choices)"));
    enableGesturesType->addItem (TRANS("Button only"), 1);
    enableGesturesType->addItem (TRANS("Simple gestures on twist"), 2);
    enableGesturesType->addItem (TRANS("Complex gestures on twist"), 3);
    enableGesturesType->addItem (TRANS("Always on"), 4);
    enableGesturesType->addSeparator();
    enableGesturesType->addListener (this);

    enableGesturesType->setBounds (16, 16, 272, 24);


    //[UserPreSize]
    //[/UserPreSize]

    setSize (600, 400);


    //[Constructor] You can add your own custom stuff here..
    deviceControl->addChangeListener(this);
    //[/Constructor]
}

GestureTab::~GestureTab()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

    midiGesture_cw = nullptr;
    label3 = nullptr;
    label4 = nullptr;
    label9 = nullptr;
    label10 = nullptr;
    label11 = nullptr;
    label12 = nullptr;
    label13 = nullptr;
    midiGesture_ccw = nullptr;
    midiGesture_ccw_up2 = nullptr;
    midiGesture_ccw_down2 = nullptr;
    midiGesture_cw_up = nullptr;
    midiGesture_cw_down = nullptr;
    label14 = nullptr;
    label16 = nullptr;
    label17 = nullptr;
    label18 = nullptr;
    label19 = nullptr;
    label = nullptr;
    enableGesturesType = nullptr;


    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void GestureTab::paint (juce::Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (juce::Colour (0xff323e44));

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void GestureTab::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void GestureTab::sliderValueChanged (juce::Slider* sliderThatWasMoved)
{
    //[UsersliderValueChanged_Pre]
    //[/UsersliderValueChanged_Pre]

    if (sliderThatWasMoved == midiGesture_cw.get())
    {
        //[UserSliderCode_midiGesture_cw] -- add your slider handling code here..
        saveMidiCC("MIDIXCW", sliderThatWasMoved, 5);
        //[/UserSliderCode_midiGesture_cw]
    }
    else if (sliderThatWasMoved == midiGesture_ccw.get())
    {
        //[UserSliderCode_midiGesture_ccw] -- add your slider handling code here..
        saveMidiCC("MIDIXCCW", sliderThatWasMoved, 5);
        //[/UserSliderCode_midiGesture_ccw]
    }
    else if (sliderThatWasMoved == midiGesture_ccw_up2.get())
    {
        //[UserSliderCode_midiGesture_ccw_up2] -- add your slider handling code here..
        saveMidiCC("MIDIXCCWU", sliderThatWasMoved, 5);
        //[/UserSliderCode_midiGesture_ccw_up2]
    }
    else if (sliderThatWasMoved == midiGesture_ccw_down2.get())
    {
        //[UserSliderCode_midiGesture_ccw_down2] -- add your slider handling code here..
        saveMidiCC("MIDIXCCWD", sliderThatWasMoved, 5);
        //[/UserSliderCode_midiGesture_ccw_down2]
    }
    else if (sliderThatWasMoved == midiGesture_cw_up.get())
    {
        //[UserSliderCode_midiGesture_cw_up] -- add your slider handling code here..
        saveMidiCC("MIDIXCWU", sliderThatWasMoved, 5);
        //[/UserSliderCode_midiGesture_cw_up]
    }
    else if (sliderThatWasMoved == midiGesture_cw_down.get())
    {
        //[UserSliderCode_midiGesture_cw_down] -- add your slider handling code here..
        saveMidiCC("MIDIXCWD", sliderThatWasMoved, 5);
        //[/UserSliderCode_midiGesture_cw_down]
    }

    //[UsersliderValueChanged_Post]
    //[/UsersliderValueChanged_Post]
}

void GestureTab::comboBoxChanged (juce::ComboBox* comboBoxThatHasChanged)
{
    //[UsercomboBoxChanged_Pre]
    //[/UsercomboBoxChanged_Pre]

    if (comboBoxThatHasChanged == enableGesturesType.get())
    {
        //[UserComboBoxCode_enableGesturesType] -- add your combo box handling code here..
        int gesture = enableGesturesType->getSelectedId();
        deviceControl->settings.setInt("GESTURES", gesture - 1);
        deviceControl->sendSettingName("GESTURES");
        //[/UserComboBoxCode_enableGesturesType]
    }

    //[UsercomboBoxChanged_Post]
    //[/UsercomboBoxChanged_Post]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...

void GestureTab::saveMidiCC(const char *name, Slider *slider, int offset) {
    int i = (int)slider->getValue();
    deviceControl->settings.setInt(name, i + offset);
    deviceControl->sendSettingName(name);
}

void GestureTab::loadMidiCC(const char *name, Slider *slider, int offset) {
    slider->setValue(deviceControl->settings.getInt(name) - offset, dontSendNotification);
}

void GestureTab::loadInfo()
{
    loadMidiCC("MIDIXCW", midiGesture_cw.get(), 5);
    loadMidiCC("MIDIXCCW", midiGesture_ccw.get(), 5);
    loadMidiCC("MIDIXCWU", midiGesture_cw_up.get(), 5);
    loadMidiCC("MIDIXCWD", midiGesture_cw_down.get(), 5);
    loadMidiCC("MIDIXCCWU", midiGesture_ccw_up2.get(), 5);
    loadMidiCC("MIDIXCCWD", midiGesture_ccw_down2.get(), 5);
    enableGesturesType->setSelectedId(deviceControl->settings.getInt("GESTURES") + 1);
}

void GestureTab::changeListenerCallback (ChangeBroadcaster* source)
{
    if (! deviceControl->isValid()) return;
    loadInfo();
}

//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Projucer information section --

    This is where the Projucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="GestureTab" componentName=""
                 parentClasses="public Component, public ChangeListener" constructorParams=""
                 variableInitialisers="" snapPixels="8" snapActive="1" snapShown="1"
                 overlayOpacity="0.330" fixedSize="0" initialWidth="600" initialHeight="400">
  <BACKGROUND backgroundColour="ff323e44"/>
  <SLIDER name="new slider" id="718b6a07722a5bec" memberName="midiGesture_cw"
          virtualName="" explicitFocusOrder="0" pos="85 139 124 24" tooltip="MIDI Control Code."
          min="-2.0" max="127.0" int="1.0" style="IncDecButtons" textBoxPos="TextBoxLeft"
          textBoxEditable="1" textBoxWidth="60" textBoxHeight="20" skewFactor="1.0"
          needsCallback="1"/>
  <LABEL name="new label" id="6bb0ee4f1bb60485" memberName="label3" virtualName=""
         explicitFocusOrder="0" pos="13 139 63 24" edTextCol="ff000000"
         edBkgCol="0" labelText="CW" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15.0"
         kerning="0.0" bold="0" italic="0" justification="34"/>
  <LABEL name="new label" id="9276a19ddcf1d2d2" memberName="label4" virtualName=""
         explicitFocusOrder="0" pos="105 88 80 48" edTextCol="ff000000"
         edBkgCol="0" labelText="MIDI&#10;Control*" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="15.0" kerning="0.0" bold="0" italic="0" justification="36"/>
  <LABEL name="new label" id="7efdf2686ecc4712" memberName="label9" virtualName=""
         explicitFocusOrder="0" pos="13 163 63 24" edTextCol="ff000000"
         edBkgCol="0" labelText="CCW" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15.0"
         kerning="0.0" bold="0" italic="0" justification="34"/>
  <LABEL name="new label" id="8a1d385f6b7b86d5" memberName="label10" virtualName=""
         explicitFocusOrder="0" pos="212 235 120 24" edTextCol="ff000000"
         edBkgCol="0" labelText="CCW + Up" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15.0"
         kerning="0.0" bold="0" italic="0" justification="33"/>
  <LABEL name="new label" id="e64c2ef224502b0d" memberName="label11" virtualName=""
         explicitFocusOrder="0" pos="212 259 120 24" edTextCol="ff000000"
         edBkgCol="0" labelText="CCW + Down" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15.0"
         kerning="0.0" bold="0" italic="0" justification="33"/>
  <LABEL name="new label" id="a1013cf438a1cdcb" memberName="label12" virtualName=""
         explicitFocusOrder="0" pos="21 187 55 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Tilt up" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15.0"
         kerning="0.0" bold="0" italic="0" justification="34"/>
  <LABEL name="new label" id="fde740b70fa6931c" memberName="label13" virtualName=""
         explicitFocusOrder="0" pos="5 211 71 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Tilt down" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15.0"
         kerning="0.0" bold="0" italic="0" justification="34"/>
  <SLIDER name="new slider" id="c4da487c65ba31b7" memberName="midiGesture_ccw"
          virtualName="" explicitFocusOrder="0" pos="85 163 124 24" tooltip="MIDI control code."
          min="-2.0" max="127.0" int="1.0" style="IncDecButtons" textBoxPos="TextBoxLeft"
          textBoxEditable="1" textBoxWidth="60" textBoxHeight="20" skewFactor="1.0"
          needsCallback="1"/>
  <SLIDER name="new slider" id="c82b36b41a6b93c8" memberName="midiGesture_ccw_up2"
          virtualName="" explicitFocusOrder="0" pos="85 235 124 24" tooltip="MIDI control code."
          min="-2.0" max="127.0" int="1.0" style="IncDecButtons" textBoxPos="TextBoxLeft"
          textBoxEditable="1" textBoxWidth="60" textBoxHeight="20" skewFactor="1.0"
          needsCallback="1"/>
  <SLIDER name="new slider" id="c59aabc791dd1a0f" memberName="midiGesture_ccw_down2"
          virtualName="" explicitFocusOrder="0" pos="85 259 124 24" tooltip="MIDI control code."
          min="-2.0" max="127.0" int="1.0" style="IncDecButtons" textBoxPos="TextBoxLeft"
          textBoxEditable="1" textBoxWidth="60" textBoxHeight="20" skewFactor="1.0"
          needsCallback="1"/>
  <SLIDER name="new slider" id="15cac80b5644145c" memberName="midiGesture_cw_up"
          virtualName="" explicitFocusOrder="0" pos="85 187 124 24" tooltip="MIDI control code."
          min="-2.0" max="127.0" int="1.0" style="IncDecButtons" textBoxPos="TextBoxLeft"
          textBoxEditable="1" textBoxWidth="60" textBoxHeight="20" skewFactor="1.0"
          needsCallback="1"/>
  <SLIDER name="new slider" id="d464f81a69ad5416" memberName="midiGesture_cw_down"
          virtualName="" explicitFocusOrder="0" pos="85 211 124 24" tooltip="MIDI control code."
          min="-2.0" max="127.0" int="1.0" style="IncDecButtons" textBoxPos="TextBoxLeft"
          textBoxEditable="1" textBoxWidth="60" textBoxHeight="20" skewFactor="1.0"
          needsCallback="1"/>
  <LABEL name="new label" id="15c311d96e841dfa" memberName="label14" virtualName=""
         explicitFocusOrder="0" pos="16 291 280 24" edTextCol="ff000000"
         edBkgCol="0" labelText="*Control: -1 = pitch up; -2 = pitch down."
         editableSingleClick="0" editableDoubleClick="0" focusDiscardsChanges="0"
         fontname="Default font" fontsize="15.0" kerning="0.0" bold="0"
         italic="0" justification="33"/>
  <LABEL name="new label" id="eb7702a499132f3a" memberName="label16" virtualName=""
         explicitFocusOrder="0" pos="8 88 80 47" edTextCol="ff000000"
         edBkgCol="0" labelText="When using button" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="15.0" kerning="0.0" bold="0" italic="0" justification="34"/>
  <LABEL name="new label" id="b440419a026249cc" memberName="label17" virtualName=""
         explicitFocusOrder="0" pos="212 187 136 24" edTextCol="ff000000"
         edBkgCol="0" labelText="CW + Up " editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15.0"
         kerning="0.0" bold="0" italic="0" justification="33"/>
  <LABEL name="new label" id="34e13a8255efb11" memberName="label18" virtualName=""
         explicitFocusOrder="0" pos="212 211 136 24" edTextCol="ff000000"
         edBkgCol="0" labelText="CW + Down" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15.0"
         kerning="0.0" bold="0" italic="0" justification="33"/>
  <LABEL name="new label" id="c92cea72677c6661" memberName="label19" virtualName=""
         explicitFocusOrder="0" pos="208 88 112 48" edTextCol="ff000000"
         edBkgCol="0" labelText="Without button" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="15.0" kerning="0.0" bold="0" italic="0" justification="33"/>
  <LABEL name="new label" id="7752d40402554811" memberName="label" virtualName=""
         explicitFocusOrder="0" pos="8 44 239 40" edTextCol="ff000000"
         edBkgCol="0" labelText="CW = Clockwise rotation&#10;CCW = Counter clockwise"
         editableSingleClick="0" editableDoubleClick="0" focusDiscardsChanges="0"
         fontname="Default font" fontsize="15.0" kerning="0.0" bold="0"
         italic="0" justification="33"/>
  <COMBOBOX name="new combo box" id="232b8675ff5f5caf" memberName="enableGesturesType"
            virtualName="" explicitFocusOrder="0" pos="16 16 272 24" editable="0"
            layout="33" items="Button only&#10;Simple gestures on twist&#10;Complex gestures on twist&#10;Always on&#10;"
            textWhenNonSelected="" textWhenNoItems="(no choices)"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]

