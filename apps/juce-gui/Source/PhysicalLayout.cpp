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
//[/Headers]

#include "PhysicalLayout.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...
//[/MiscUserDefs]

//==============================================================================
PhysicalLayout::PhysicalLayout ()
{
    //[Constructor_pre] You can add your own custom stuff here..
    //[/Constructor_pre]

    tone.reset (new juce::Slider ("new slider"));
    addAndMakeVisible (tone.get());
    tone->setRange (0, 10, 0);
    tone->setSliderStyle (juce::Slider::Rotary);
    tone->setTextBoxStyle (juce::Slider::NoTextBox, false, 80, 20);
    tone->addListener (this);

    tone->setBounds (100, 128, 64, 64);

    smoothness.reset (new juce::Slider ("new slider"));
    addAndMakeVisible (smoothness.get());
    smoothness->setRange (0, 10, 0);
    smoothness->setSliderStyle (juce::Slider::Rotary);
    smoothness->setTextBoxStyle (juce::Slider::NoTextBox, false, 80, 20);
    smoothness->addListener (this);

    smoothness->setBounds (8, 128, 64, 64);

    echo.reset (new juce::Slider ("new slider"));
    addAndMakeVisible (echo.get());
    echo->setRange (0, 10, 0);
    echo->setSliderStyle (juce::Slider::Rotary);
    echo->setTextBoxStyle (juce::Slider::NoTextBox, false, 80, 20);
    echo->addListener (this);

    echo->setBounds (100, 8, 64, 64);

    label.reset (new juce::Label ("new label",
                                  TRANS("Tone")));
    addAndMakeVisible (label.get());
    label->setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    label->setJustificationType (juce::Justification::centredLeft);
    label->setEditable (false, false, false);
    label->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    label->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    label->setBounds (108, 192, 40, 24);

    label2.reset (new juce::Label ("new label",
                                   TRANS("Smooth\n"
                                   "Transitions")));
    addAndMakeVisible (label2.get());
    label2->setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    label2->setJustificationType (juce::Justification::centredLeft);
    label2->setEditable (false, false, false);
    label2->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    label2->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    label2->setBounds (8, 192, 96, 40);

    label3.reset (new juce::Label ("new label",
                                   TRANS("Echo")));
    addAndMakeVisible (label3.get());
    label3->setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    label3->setJustificationType (juce::Justification::centredLeft);
    label3->setEditable (false, false, false);
    label3->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    label3->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    label3->setBounds (108, 74, 40, 24);

    base.reset (new juce::Slider ("new slider"));
    addAndMakeVisible (base.get());
    base->setRange (1, 5, 1);
    base->setSliderStyle (juce::Slider::Rotary);
    base->setTextBoxStyle (juce::Slider::NoTextBox, false, 80, 20);
    base->addListener (this);

    base->setBounds (8, 8, 64, 64);

    label4.reset (new juce::Label ("new label",
                                   TRANS("Base")));
    addAndMakeVisible (label4.get());
    label4->setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    label4->setJustificationType (juce::Justification::centredLeft);
    label4->setEditable (false, false, false);
    label4->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    label4->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    label4->setBounds (16, 74, 72, 24);


    //[UserPreSize]
    //[/UserPreSize]

    setSize (600, 400);


    //[Constructor] You can add your own custom stuff here..

	tone->setLookAndFeel(&unipolarLook);
	smoothness->setLookAndFeel(&unipolarLook);
	echo->setLookAndFeel(&unipolarLook);
    base->setLookAndFeel(&chorusLook);
	deviceControl->addChangeListener(this);
    //[/Constructor]
}

PhysicalLayout::~PhysicalLayout()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

    tone = nullptr;
    smoothness = nullptr;
    echo = nullptr;
    label = nullptr;
    label2 = nullptr;
    label3 = nullptr;
    base = nullptr;
    label4 = nullptr;


    //[Destructor]. You can add your own custom destruction code here..
	// deviceControl->removeChangeListener(this);
    //[/Destructor]
}

//==============================================================================
void PhysicalLayout::paint (juce::Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (juce::Colour (0xff323e44));

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void PhysicalLayout::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void PhysicalLayout::sliderValueChanged (juce::Slider* sliderThatWasMoved)
{
    //[UsersliderValueChanged_Pre]
    //[/UsersliderValueChanged_Pre]

    if (sliderThatWasMoved == tone.get())
    {
        //[UserSliderCode_tone] -- add your slider handling code here..
        double v = tone->getValue();
        int x = (int)(v / 10.0 * 127.0);
        deviceControl->sendFilterSetting(18, x);
        //[/UserSliderCode_tone]
    }
    else if (sliderThatWasMoved == smoothness.get())
    {
        //[UserSliderCode_smoothness] -- add your slider handling code here..
        double v = smoothness->getValue();
        int x = (int)(v / 10.0 * 127.0);
        deviceControl->sendFilterSetting(19, x);
        //[/UserSliderCode_smoothness]
    }
    else if (sliderThatWasMoved == echo.get())
    {
        //[UserSliderCode_echo] -- add your slider handling code here..
        double v = echo->getValue();
        int x = (int)(v / 10.0 * 127.0);
        deviceControl->sendFilterSetting(20, x);
        //[/UserSliderCode_echo]
    }
    else if (sliderThatWasMoved == base.get())
    {
        //[UserSliderCode_base] -- add your slider handling code here..
        double x = base->getValue();
        deviceControl->sendFilterSetting(23, (int)x);
        //[/UserSliderCode_base]
    }

    //[UsersliderValueChanged_Post]
    //[/UsersliderValueChanged_Post]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...
void PhysicalLayout::loadSettings() {
	tone->setValue(deviceControl->synth.u.physical.tone / 12.7);
	smoothness->setValue(deviceControl->synth.u.physical.smoothness / 12.7);
	echo->setValue(deviceControl->synth.u.physical.echo / 12.7);
    base->setValue(deviceControl->synth.u.physical.table + 1);
}

void PhysicalLayout::changeListenerCallback(ChangeBroadcaster* source) {
	if (!deviceControl->isValid()) return;
	if (deviceControl->synth.synth_use != PHYSICAL_MODEL) {
		return;
	}
	loadSettings();
}
//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Projucer information section --

    This is where the Projucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="PhysicalLayout" componentName=""
                 parentClasses="public Component, public ChangeListener" constructorParams=""
                 variableInitialisers="" snapPixels="8" snapActive="1" snapShown="1"
                 overlayOpacity="0.330" fixedSize="0" initialWidth="600" initialHeight="400">
  <BACKGROUND backgroundColour="ff323e44"/>
  <SLIDER name="new slider" id="6e9dfe9991ef6b39" memberName="tone" virtualName=""
          explicitFocusOrder="0" pos="100 128 64 64" min="0.0" max="10.0"
          int="0.0" style="Rotary" textBoxPos="NoTextBox" textBoxEditable="1"
          textBoxWidth="80" textBoxHeight="20" skewFactor="1.0" needsCallback="1"/>
  <SLIDER name="new slider" id="84d79ec06e64560f" memberName="smoothness"
          virtualName="" explicitFocusOrder="0" pos="8 128 64 64" min="0.0"
          max="10.0" int="0.0" style="Rotary" textBoxPos="NoTextBox" textBoxEditable="1"
          textBoxWidth="80" textBoxHeight="20" skewFactor="1.0" needsCallback="1"/>
  <SLIDER name="new slider" id="fb2d935e12ef5909" memberName="echo" virtualName=""
          explicitFocusOrder="0" pos="100 8 64 64" min="0.0" max="10.0"
          int="0.0" style="Rotary" textBoxPos="NoTextBox" textBoxEditable="1"
          textBoxWidth="80" textBoxHeight="20" skewFactor="1.0" needsCallback="1"/>
  <LABEL name="new label" id="4d3cc0512d9d4246" memberName="label" virtualName=""
         explicitFocusOrder="0" pos="108 192 40 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Tone" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15.0"
         kerning="0.0" bold="0" italic="0" justification="33"/>
  <LABEL name="new label" id="9477883d009a0436" memberName="label2" virtualName=""
         explicitFocusOrder="0" pos="8 192 96 40" edTextCol="ff000000"
         edBkgCol="0" labelText="Smooth&#10;Transitions" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="15.0" kerning="0.0" bold="0" italic="0" justification="33"/>
  <LABEL name="new label" id="55f9b93fa239bf40" memberName="label3" virtualName=""
         explicitFocusOrder="0" pos="108 74 40 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Echo" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15.0"
         kerning="0.0" bold="0" italic="0" justification="33"/>
  <SLIDER name="new slider" id="65abca096f5ad325" memberName="base" virtualName=""
          explicitFocusOrder="0" pos="8 8 64 64" min="1.0" max="5.0" int="1.0"
          style="Rotary" textBoxPos="NoTextBox" textBoxEditable="1" textBoxWidth="80"
          textBoxHeight="20" skewFactor="1.0" needsCallback="1"/>
  <LABEL name="new label" id="fa3f361e2420835f" memberName="label4" virtualName=""
         explicitFocusOrder="0" pos="16 74 72 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Base" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15.0"
         kerning="0.0" bold="0" italic="0" justification="33"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]

