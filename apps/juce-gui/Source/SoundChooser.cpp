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

#include "SoundChooser.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...
# include "SoundTab.h"
#include "PhysicalLayout.h"
#include "WaveSF2Layout.h"
//[/MiscUserDefs]

//==============================================================================
SoundChooser::SoundChooser ()
{
    //[Constructor_pre] You can add your own custom stuff here..
    //[/Constructor_pre]

    modelChoice.reset (new juce::ComboBox ("new combo box"));
    addAndMakeVisible (modelChoice.get());
    modelChoice->setEditableText (false);
    modelChoice->setJustificationType (juce::Justification::centredLeft);
    modelChoice->setTextWhenNothingSelected (juce::String());
    modelChoice->setTextWhenNoChoicesAvailable (TRANS("(no choices)"));
    modelChoice->addItem (TRANS("Analog Model"), 1);
    modelChoice->addItem (TRANS("Physical Model"), 2);
    modelChoice->addItem (TRANS("Sax Model"), 3);
    modelChoice->addListener (this);

    modelChoice->setBounds (110, 6, 138, 24);

    instrumentChoice.reset (new juce::ComboBox ("new combo box"));
    addAndMakeVisible (instrumentChoice.get());
    instrumentChoice->setEditableText (false);
    instrumentChoice->setJustificationType (juce::Justification::centredLeft);
    instrumentChoice->setTextWhenNothingSelected (juce::String());
    instrumentChoice->setTextWhenNoChoicesAvailable (TRANS("(no choices)"));
    instrumentChoice->addListener (this);

    instrumentChoice->setBounds (5, 6, 99, 24);

    playButton.reset (new juce::TextButton ("new button"));
    addAndMakeVisible (playButton.get());
    playButton->setButtonText (TRANS("Play A4"));
    playButton->addListener (this);

    playButton->setBounds (260, 5, 67, 24);

    enableSpeakerToggle.reset (new juce::ToggleButton ("new toggle button"));
    addAndMakeVisible (enableSpeakerToggle.get());
    enableSpeakerToggle->setButtonText (TRANS("<("));
    enableSpeakerToggle->addListener (this);

    enableSpeakerToggle->setBounds (337, 5, 55, 24);


    //[UserPreSize]
    //[/UserPreSize]

    setSize (600, 400);


    //[Constructor] You can add your own custom stuff here..

    physicalModel.reset(new PhysicalLayout());
	addChildComponent(physicalModel.get());
    physicalModel->setBounds(4, 30, 664, 576);

    waveSF2Model.reset(new WaveSF2Layout());
    addChildComponent(waveSF2Model.get());
    waveSF2Model->setBounds(4, 30, 664, 576);

    analogModel.reset(new SoundTab());
	addChildComponent(analogModel.get());
	analogModel->setBounds(4, 30, 664, 576);

	for (int i = 1; i <= 9; i++) {
		String name = "Sound ";
		name += i;
		instrumentChoice->addItem(name, i);
	}
	instrumentChoice->setSelectedId(1);

	deviceControl->addChangeListener(this);

	// enableSpeakerToggle->setButtonText(String::fromUTF8(u8"\u1F508"));

    //[/Constructor]
}

SoundChooser::~SoundChooser()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

    modelChoice = nullptr;
    instrumentChoice = nullptr;
    playButton = nullptr;
    enableSpeakerToggle = nullptr;


    //[Destructor]. You can add your own custom destruction code here..
	// deviceControl->removeChangeListener(this);
    //[/Destructor]
}

//==============================================================================
void SoundChooser::paint (juce::Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (juce::Colour (0xff323e44));

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void SoundChooser::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void SoundChooser::comboBoxChanged (juce::ComboBox* comboBoxThatHasChanged)
{
    //[UsercomboBoxChanged_Pre]
    //[/UsercomboBoxChanged_Pre]

    if (comboBoxThatHasChanged == modelChoice.get())
    {
        //[UserComboBoxCode_modelChoice] -- add your combo box handling code here..
        int model = modelChoice->getSelectedId();
        if (model == 2) {
			physicalModel.get()->setVisible(true);
			analogModel.get()->setVisible(false);
            waveSF2Model.get()->setVisible(false);
			if (deviceControl->synth.synth_use != PHYSICAL_MODEL) {
				deviceControl->synth.loadDefaults_physical();
				deviceControl->sendFilterSetting(21, PHYSICAL_MODEL);
				physicalModel->loadSettings();
			}
        }
        else if (model == 3){
            physicalModel.get()->setVisible(false);
            analogModel.get()->setVisible(false);
            waveSF2Model.get()->setVisible(true);
            if (deviceControl->synth.synth_use != WAVESF2_MODEL) {
                deviceControl->synth.loadDefaults_wavesf2();
                deviceControl->sendFilterSetting(21, WAVESF2_MODEL);
                analogModel->loadSettings();
            }
        }
        else if (model == 1){
			physicalModel.get()->setVisible(false);
			analogModel.get()->setVisible(true);
            waveSF2Model.get()->setVisible(false);
			if (deviceControl->synth.synth_use != ANALOG_MODEL) {
				deviceControl->synth.loadDefaults_analog();
				deviceControl->sendFilterSetting(21, ANALOG_MODEL);
				analogModel->loadSettings();
			}
        }
        else {
            // TODO internal error!
        }

        //[/UserComboBoxCode_modelChoice]
    }
    else if (comboBoxThatHasChanged == instrumentChoice.get())
    {
        //[UserComboBoxCode_instrumentChoice] -- add your combo box handling code here..
		int inst = instrumentChoice->getSelectedId() - 1;
		deviceControl->settings.setInt("INST", inst);
		//        deviceControl->sendProgramChange(inst);
		deviceControl->sendFilterSetting(15, inst);
		Thread::sleep(200);
		deviceControl->sendRequestSynth(inst);
        //[/UserComboBoxCode_instrumentChoice]
    }

    //[UsercomboBoxChanged_Post]
    //[/UsercomboBoxChanged_Post]
}

void SoundChooser::buttonClicked (juce::Button* buttonThatWasClicked)
{
    //[UserbuttonClicked_Pre]
    //[/UserbuttonClicked_Pre]

    if (buttonThatWasClicked == playButton.get())
    {
        //[UserButtonCode_playButton] -- add your button handler code here..
        static int state;
        if (state) {
            state = 0;
            playButton->setButtonText("Play");
        }
        else {
            state = 1;
            playButton->setButtonText("Stop");
        }
        deviceControl->sendNoteOnOff(state, 60, 32);
        if (state) { // needed to apply temporary filter settings
            deviceControl->sendFilterSetting(0);
        }
        //[/UserButtonCode_playButton]
    }
    else if (buttonThatWasClicked == enableSpeakerToggle.get())
    {
        //[UserButtonCode_enableSpeakerToggle] -- add your button handler code here..
        auto b = enableSpeakerToggle->getToggleState();
        deviceControl->sendFilterSetting(22, b?1:0);
        //[/UserButtonCode_enableSpeakerToggle]
    }

    //[UserbuttonClicked_Post]
    //[/UserbuttonClicked_Post]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...
void SoundChooser::changeListenerCallback(ChangeBroadcaster* source) {
	if (!deviceControl->isValid()) return;

    if (deviceControl->settings.Firmware < 4) {
        modelChoice->setVisible(false);
        modelChoice->setSelectedId(1);
    }
    else {
        modelChoice->setVisible(true);
        if (deviceControl->synth.synth_use == ANALOG_MODEL) {
            modelChoice->setSelectedId(1);
        }
        else if (deviceControl->synth.synth_use == PHYSICAL_MODEL) {
            modelChoice->setSelectedId(2);
        }
        else if (deviceControl->synth.synth_use == WAVESF2_MODEL) {
            modelChoice->setSelectedId(3);
        }
    }
}
//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Projucer information section --

    This is where the Projucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="SoundChooser" componentName=""
                 parentClasses="public Component, public ChangeListener" constructorParams=""
                 variableInitialisers="" snapPixels="8" snapActive="1" snapShown="1"
                 overlayOpacity="0.330" fixedSize="0" initialWidth="600" initialHeight="400">
  <BACKGROUND backgroundColour="ff323e44"/>
  <COMBOBOX name="new combo box" id="69515b0761094639" memberName="modelChoice"
            virtualName="" explicitFocusOrder="0" pos="110 6 138 24" editable="0"
            layout="33" items="Analog Model&#10;Physical Model&#10;Sax Model"
            textWhenNonSelected="" textWhenNoItems="(no choices)"/>
  <COMBOBOX name="new combo box" id="b6221a0f3dd5dde5" memberName="instrumentChoice"
            virtualName="" explicitFocusOrder="0" pos="5 6 99 24" editable="0"
            layout="33" items="" textWhenNonSelected="" textWhenNoItems="(no choices)"/>
  <TEXTBUTTON name="new button" id="4fcb7fddc79642e3" memberName="playButton"
              virtualName="" explicitFocusOrder="0" pos="260 5 67 24" buttonText="Play A4"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <TOGGLEBUTTON name="new toggle button" id="19de232271c9860d" memberName="enableSpeakerToggle"
                virtualName="" explicitFocusOrder="0" pos="337 5 55 24" buttonText="&lt;("
                connectedEdges="0" needsCallback="1" radioGroupId="0" state="0"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]

