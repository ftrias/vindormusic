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
//[/Headers]

#include "OscComponent.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...
#include "LookStyle.h"
#include "VindorWrap.h"
//[/MiscUserDefs]

//==============================================================================
OscComponent::OscComponent ()
{
    //[Constructor_pre] You can add your own custom stuff here..
    //[/Constructor_pre]

    wave2.reset (new Slider ("osc wave"));
    addAndMakeVisible (wave2.get());
    wave2->setTooltip (TRANS("Rotate to choose the waveform shape for this oscillator."));
    wave2->setRange (0, 4, 1);
    wave2->setSliderStyle (Slider::Rotary);
    wave2->setTextBoxStyle (Slider::NoTextBox, false, 80, 20);
    wave2->addListener (this);

    wave2->setBounds (0, 0, 64, 64);

    octave.reset (new Slider ("osc octshift"));
    addAndMakeVisible (octave.get());
    octave->setTooltip (TRANS("The octave. 0 is for the natural frequency of the note. Positive number for higher; negative for lower."));
    octave->setRange (-4, 4, 1);
    octave->setSliderStyle (Slider::Rotary);
    octave->setTextBoxStyle (Slider::NoTextBox, false, 80, 20);
    octave->addListener (this);

    octave->setBounds (0, 56, 64, 80);

    frequency.reset (new Slider ("osc freq"));
    addAndMakeVisible (frequency.get());
    frequency->setTooltip (TRANS("Adjust frequency up or down by number of half-steps."));
    frequency->setRange (-7, 7, 0.1);
    frequency->setSliderStyle (Slider::Rotary);
    frequency->setTextBoxStyle (Slider::NoTextBox, false, 80, 20);
    frequency->addListener (this);

    frequency->setBounds (0, 114, 64, 80);

    volume.reset (new Slider ("osc vol"));
    addAndMakeVisible (volume.get());
    volume->setTooltip (TRANS("Volume of this oscillator relative to other oscillators."));
    volume->setRange (0, 1, 0.01);
    volume->setSliderStyle (Slider::LinearVertical);
    volume->setTextBoxStyle (Slider::NoTextBox, false, 80, 20);
    volume->addListener (this);

    volume->setBounds (0, 160, 64, 110);


    //[UserPreSize]
    //[/UserPreSize]

    setSize (600, 400);


    //[Constructor] You can add your own custom stuff here..

    wave2->setLookAndFeel(&waveLook);
    volume->setLookAndFeel(&slideLook);
    frequency->setLookAndFeel(&detuneLook);
    octave->setLookAndFeel(&octaveLook);

    //[/Constructor]
}

OscComponent::~OscComponent()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

    wave2 = nullptr;
    octave = nullptr;
    frequency = nullptr;
    volume = nullptr;


    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void OscComponent::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (Colour (0xff323e44));

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void OscComponent::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    /*
    FlexBox fb;
    fb.flexDirection = (FlexBox::Direction::column);
    fb.items.addArray ({
        FlexItem(getWidth(), wave2->getHeight(), *wave2),
        FlexItem(getWidth(), octave->getHeight(), *octave),
        FlexItem(getWidth(), frequency->getHeight(), *frequency),
        FlexItem(getWidth(), volume->getHeight(), *volume),
        FlexItem(getWidth(), lfoVolume->getHeight(), *lfoVolume)
    });
    fb.performLayout (getLocalBounds().toFloat());
    */
    //[/UserPreResize]

    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void OscComponent::sliderValueChanged (Slider* sliderThatWasMoved)
{
    //[UsersliderValueChanged_Pre]
    //[/UsersliderValueChanged_Pre]

    if (sliderThatWasMoved == wave2.get())
    {
        //[UserSliderCode_wave2] -- add your slider handling code here..
        control.setSliderWave(sliderThatWasMoved);
        //[/UserSliderCode_wave2]
    }
    else if (sliderThatWasMoved == octave.get())
    {
        //[UserSliderCode_octave] -- add your slider handling code here..
        control.setSliderOctave(sliderThatWasMoved);
        //[/UserSliderCode_octave]
    }
    else if (sliderThatWasMoved == frequency.get())
    {
        //[UserSliderCode_frequency] -- add your slider handling code here..
        control.setSliderFrequency(sliderThatWasMoved);
        //[/UserSliderCode_frequency]
    }
    else if (sliderThatWasMoved == volume.get())
    {
        //[UserSliderCode_volume] -- add your slider handling code here..
        control.setSliderVolume(sliderThatWasMoved);
        //[/UserSliderCode_volume]
    }

    //[UsersliderValueChanged_Post]
    //[/UsersliderValueChanged_Post]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...
void OscComponent::setNumber(int n)
{
    number = n;
    control.number = n;
    if (number == 0) {
        frequency->setVisible(false);
        frequency->setValue(0, NotificationType::dontSendNotification);
    }
}

#include <math.h>

void OscComponent::loadSettings()
{
    int ww = deviceControl->synth.u.analog.fil_wave[number];
    int w = 0;
    switch(ww) {
        case WAVEFORM_SINE: w=0; break;
        case WAVEFORM_TRIANGLE: w=1; break;
        case WAVEFORM_SQUARE: w=2; break;
        case WAVEFORM_SAWTOOTH: w=3; break;
        case WAVEFORM_SAWTOOTH_REVERSE: w=4; break;
    }
    wave2->setValue(w, NotificationType::dontSendNotification);

    float mult = deviceControl->synth.u.analog.fil_freq[number];
    float oct = log(mult)/log(2);
    octave->setValue(oct, NotificationType::dontSendNotification);

    double v = deviceControl->synth.u.analog.fil_freq_bias[number];
    v = log(v) / log(2) * 12;
    frequency->setValue(v, NotificationType::dontSendNotification);
    volume->setValue(deviceControl->synth.u.analog.fil_vol[number], NotificationType::dontSendNotification);
}
//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Projucer information section --

    This is where the Projucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="OscComponent" componentName=""
                 parentClasses="public Component" constructorParams="" variableInitialisers=""
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="0" initialWidth="600" initialHeight="400">
  <BACKGROUND backgroundColour="ff323e44"/>
  <SLIDER name="osc wave" id="530d14e1159d193f" memberName="wave2" virtualName=""
          explicitFocusOrder="0" pos="0 0 64 64" tooltip="Rotate to choose the waveform shape for this oscillator."
          min="0.0" max="4.0" int="1.0" style="Rotary" textBoxPos="NoTextBox"
          textBoxEditable="1" textBoxWidth="80" textBoxHeight="20" skewFactor="1.0"
          needsCallback="1"/>
  <SLIDER name="osc octshift" id="a46fb35ce64c7e0d" memberName="octave"
          virtualName="" explicitFocusOrder="0" pos="0 56 64 80" tooltip="The octave. 0 is for the natural frequency of the note. Positive number for higher; negative for lower."
          min="-4.0" max="4.0" int="1.0" style="Rotary" textBoxPos="NoTextBox"
          textBoxEditable="1" textBoxWidth="80" textBoxHeight="20" skewFactor="1.0"
          needsCallback="1"/>
  <SLIDER name="osc freq" id="5de94b9511eaeae3" memberName="frequency"
          virtualName="" explicitFocusOrder="0" pos="0 114 64 80" tooltip="Adjust frequency up or down by number of half-steps."
          min="-7.0" max="7.0" int="0.1" style="Rotary" textBoxPos="NoTextBox"
          textBoxEditable="1" textBoxWidth="80" textBoxHeight="20" skewFactor="1.0"
          needsCallback="1"/>
  <SLIDER name="osc vol" id="ff1bcd190d241d9" memberName="volume" virtualName=""
          explicitFocusOrder="0" pos="0 160 64 110" tooltip="Volume of this oscillator relative to other oscillators."
          min="0.0" max="1.0" int="0.01" style="LinearVertical" textBoxPos="NoTextBox"
          textBoxEditable="1" textBoxWidth="80" textBoxHeight="20" skewFactor="1.0"
          needsCallback="1"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]

