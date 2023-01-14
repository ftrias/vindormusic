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
#include <SFZero/SFZero/SF2Sound.h>
//[/Headers]

#include "Player.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...

struct MidiInstrumentCategory {
    int min;
    int max;
    const char *name;
};

MidiInstrumentCategory cat[] = {
    {1,8,"Piano"},
    {9,16,"Chromatic Percussion"},
    {17,24,"Organ"},
    {25,32,"Guitar"},
    {33,40,"Bass"},
    {41,47,"Strings"},
    {48,56,"Ensemble"},
    {57,64,"Brass"},
    {65,72,"Reed"},
    {73,80,"Pipe"},
    {81,88,"Synth Lead"},
    {89,96,"Synth Pad"},
    {97,104,"Synth Effects"},
    {105,112,"World"},
    {113,120,"Percussive"},
    {121,128,"Sound Effects"},
    {-1,-1,""}
};

const char *midiNames[] = {
    "Invalid",
    "Acoustic Grand Piano",
    "Bright Acoustic Piano",
    "Electric Grand Piano",
    "Honky-tonk Piano",
    "Electric Piano 1",
    "Electric Piano 2",
    "Harpsichord",
    "Clavi",
    "Celesta",
    "Glockenspiel",
    "Music Box",
    "Vibraphone",
    "Marimba",
    "Xylophone",
    "Tubular Bells",
    "Dulcimer",
    "Drawbar Organ",
    "Percussive Organ",
    "Rock Organ",
    "Church Organ",
    "Reed Organ",
    "Accordion",
    "Harmonica",
    "Tango Accordion",
    "Acoustic Guitar (nylon)",
    "Acoustic Guitar (steel)",
    "Electric Guitar (jazz)",
    "Electric Guitar (clean)",
    "Electric Guitar (muted)",
    "Overdriven Guitar",
    "Distortion Guitar",
    "Guitar harmonics",
    "Acoustic Bass",
    "Electric Bass (finger)",
    "Electric Bass (pick)",
    "Fretless Bass",
    "Slap Bass 1",
    "Slap Bass 2",
    "Synth Bass 1",
    "Synth Bass 2",
    "Violin",
    "Viola",
    "Cello",
    "Contrabass",
    "Tremolo Strings",
    "Pizzicato Strings",
    "Orchestral Harp",
    "Timpani",
    "String Ensemble 1",
    "String Ensemble 2",
    "SynthStrings 1",
    "SynthStrings 2",
    "Choir Aahs",
    "Voice Oohs",
    "Synth Voice",
    "Orchestra Hit",
    "Trumpet",
    "Trombone",
    "Tuba",
    "Muted Trumpet",
    "French Horn",
    "Brass Section",
    "SynthBrass 1",
    "SynthBrass 2",
    "Soprano Sax",
    "Alto Sax",
    "Tenor Sax",
    "Baritone Sax",
    "Oboe",
    "English Horn",
    "Bassoon",
    "Clarinet",
    "Piccolo",
    "Flute",
    "Recorder",
    "Pan Flute",
    "Blown Bottle",
    "Shakuhachi",
    "Whistle",
    "Ocarina",
    "Lead 1 (square)",
    "Lead 2 (sawtooth)",
    "Lead 3 (calliope)",
    "Lead 4 (chiff)",
    "Lead 5 (charang)",
    "Lead 6 (voice)",
    "Lead 7 (fifths)",
    "Lead 8 (bass + lead)",
    "Pad 1 (new age)",
    "Pad 2 (warm)",
    "Pad 3 (polysynth)",
    "Pad 4 (choir)",
    "Pad 5 (bowed)",
    "Pad 6 (metallic)",
    "Pad 7 (halo)",
    "Pad 8 (sweep)",
    "FX 1 (rain)",
    "FX 2 (soundtrack)",
    "FX 3 (crystal)",
    "FX 4 (atmosphere)",
    "FX 5 (brightness)",
    "FX 6 (goblins)",
    "FX 7 (echoes)",
    "FX 8 (sci-fi)",
    "Sitar",
    "Banjo",
    "Shamisen",
    "Koto",
    "Kalimba",
    "Bag pipe",
    "Fiddle",
    "Shanai",
    "Tinkle Bell",
    "Agogo",
    "Steel Drums",
    "Woodblock",
    "Taiko Drum",
    "Melodic Tom",
    "Synth Drum",
    "Reverse Cymbal",
    "Guitar Fret Noise",
    "Breath Noise",
    "Seashore",
    "Bird Tweet",
    "Telephone Ring",
    "Helicopter",
    "Applause",
    "Gunshot"
};

#include "stereo.h"

class MemBlockInputStream : public InputStream
{
private:
	const unsigned char **mdata;
	int64 mdata_len;
	int64 position;
	int block;
	int index;
	int blocklen;

public:
	MemBlockInputStream(void *data, const int64 data_len, const int block_len)
		: position(0), block(0), index(0)
	{
		mdata = (const unsigned char **)data;
		mdata_len = data_len;
		blocklen = block_len;
	}

	virtual int read(void * destBuffer, int 	maxBytesToRead)
	{
		int count = 0;
		unsigned char *dst = (unsigned char *)destBuffer;
		while (position < mdata_len) {
			if (count >= maxBytesToRead) break;
			if (index >= blocklen) {
				index = 0;
				block++;
			}
			const unsigned char *src = mdata[block];
			dst[count] = src[index];
			index++;
			count++;
			position++;
		}
		return count;
	}

	virtual int64 getTotalLength()
	{
		return mdata_len;
	}

	virtual bool isExhausted()
	{
		if (position >= mdata_len) return true;
		return false;
	}

	virtual int64 getPosition()
	{
		return position;
	}

	virtual bool 	setPosition(int64 newPosition)
	{
		if (newPosition >= mdata_len) return false;
		position = newPosition;
		block = newPosition / blocklen;
		index = newPosition % blocklen;
		return true;
	}
};

class SynthAudioSource : public AudioSource, public Thread, public ChangeBroadcaster
{
public:
    MidiBuffer incomingMidi;
    float gain;
    float mainGain;
    int loaded;

public:
    SynthAudioSource () :  Thread("load"), gain(1.0F), mainGain(1.0F), loaded(0)
    {
    }

    ~SynthAudioSource()
    {
        terminate();
    }

    void terminate()
    {
        synth.clearSounds();
        synth.clearVoices();
    }

    void run() override
    {
        load();
        loaded = 1;
        sendChangeMessage();
    }

    void load(const char *filename = NULL)
    {
        for(int i=0; i<128; i++) {
            synth.addVoice(new SFZero::SFZVoice());
        }
        AudioFormatManager formatManager;
        formatManager.registerFormat(new WavAudioFormat(), false);


//        auto sfzFile = File("/Users/ftrias/Documents/Source/Vindor/vindor/sounds/sf2/go÷ld.sf2");
//        auto sfzFile = File("/Users/ftrias/Documents/Source/Vindor/vindor/sounds/sf2/omega.sf2");
//        MemoryInputStream sfzFile(BinaryData::stereo_sf2, BinaryData::stereo_sf2Size, false);
        SFZero::SF2Sound *sound;
        MemBlockInputStream sfzFile(&stereo_sf2, stereo_sf2_len, stereo_sf2_block);
        File file;
        if (filename) {
            file = filename;
            sound = new SFZero::SF2Sound(file);
        }
        else {
            sound = new SFZero::SF2Sound(&sfzFile);
        }
        sound->loadRegions();
        sound->loadSamples(&formatManager);

//        synth.clearVoices();
        synth.addSound(sound);
    }

    void setInstrument(int num) {
        SFZero::SF2Sound *sound = (SFZero::SF2Sound*)synth.getSound(0).get();
        sound->useSubsound(num-1);
    }

    void panic() {
        incomingMidi.clear();
        gain = 1.0F;
        synth.allNotesOff(1, 0);
    }

    StringArray getInstruments() {
        StringArray soundNames;
        SFZero::SF2Sound *sound = (SFZero::SF2Sound*)synth.getSound(0).get();
        if (sound==NULL) return soundNames;
        for(int i=0; i < sound->numSubsounds(); i++) {
            String name = sound->subsoundName(i);
            int idx = name.lastIndexOfChar(':');
            if (idx > 0) {
                name = name.substring(idx+2, (int)name.getNumBytesAsUTF8());
            }
            soundNames.add(name);
        }
        return soundNames;
    }

    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override
    {
        synth.setCurrentPlaybackSampleRate (sampleRate); // [3]
    }
    void releaseResources() override
    {
    }
    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override
    {
        static double prevGain;
        bufferToFill.clearActiveBufferRegion();
        synth.renderNextBlock (*bufferToFill.buffer, incomingMidi,
                               bufferToFill.startSample, bufferToFill.numSamples); // [5]
        double newGain = gain * mainGain;
        bufferToFill.buffer->applyGainRamp(0, bufferToFill.startSample, bufferToFill.numSamples,
                                           prevGain, newGain);
        if (bufferToFill.buffer->getNumChannels() > 1) {
            bufferToFill.buffer->applyGainRamp(1, bufferToFill.startSample, bufferToFill.numSamples,
                                           prevGain, newGain);
        }
        prevGain = newGain;
        incomingMidi.clear();
    }
//private:
public:
    SFZero::SFZSynth synth;
};

SynthAudioSource synthAudioSource;
ReverbAudioSource reverb(&synthAudioSource, false);

void Player::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
//    synthAudioSource.prepareToPlay (samplesPerBlockExpected, sampleRate);
    reverb.prepareToPlay (samplesPerBlockExpected, sampleRate);
}
void Player::getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill)
{
    if (synthAudioSource.loaded == 0) return;
//    synthAudioSource.getNextAudioBlock (bufferToFill);
    reverb.getNextAudioBlock (bufferToFill);
}
void Player::releaseResources()
{
//    synthAudioSource.releaseResources();
    reverb.releaseResources();
}

void Player::handleIncomingMidiMessage (MidiInput* source, const MidiMessage& message)
{
    static int bcstate = 0;
    const ScopedValueSetter<bool> scopedInputFlag (isAddingFromMidiInput, true);
    keyboardState.processNextMidiEvent (message);
    int v;
    if (message.isController()) {
        switch (message.getControllerNumber()) {
            case 2:
            case 7:
            case 11:
                v = message.getControllerValue();
                synthAudioSource.gain = float(v) / 127.0F;
                bcstate = 1;
                break;
            // HiRes messages below
            case 2+32:
            case 7+32:
            case 11+32:
                v = message.getControllerValue();
                synthAudioSource.gain += float(v) / 16129.0F;
                break;
        }
    }
    else if(message.isAftertouch()) {
        synthAudioSource.gain = float(message.getAfterTouchValue()) / 127.0F;
    }
    else if(message.isNoteOn()) {
         // if volume not controlled by BC/EXP/etc, use it; otherwise ignore it
        if (! bcstate) {
            synthAudioSource.gain = message.getVelocity() / 127.0F;
        }
        MidiMessage message2 = message;
        message2.setVelocity(1.0F);
        synthAudioSource.incomingMidi.addEvent(message2, 0);
    }
    else {
        synthAudioSource.incomingMidi.addEvent(message, 0);
    }
}

void Player::handleNoteOn (MidiKeyboardState*, int midiChannel, int midiNoteNumber, float velocity)
{
    if (! isAddingFromMidiInput)
    {
        synthAudioSource.gain = velocity;
        auto m = MidiMessage::noteOn (midiChannel, midiNoteNumber, 1.0F);
        synthAudioSource.incomingMidi.addEvent(m, 0);
    }
}

void Player::handleNoteOff (MidiKeyboardState*, int midiChannel, int midiNoteNumber, float velocity)
{
    if (! isAddingFromMidiInput)
    {
        auto m = MidiMessage::noteOff (midiChannel, midiNoteNumber);
        synthAudioSource.incomingMidi.addEvent(m, 0);
    }
}

void Player::changeListenerCallback(ChangeBroadcaster* source)
{
    if (source == &synthAudioSource) {
        if (synthAudioSource.loaded == 1) {
            progress->setVisible(false);
            soundCategory->setSelectedItemIndex(8);
            synthAudioSource.loaded = 2;
        }
    }
}

void Player::timerCallback()
{
    queryMidiInputs();
}

void Player::queryMidiInputs()
{
//    unsigned char query[] = {0xF0, MIDI_MFG1, MIDI_MFG2, MIDI_MFG3, 0x17, 0x36, 0x00, 0xF7};
    auto midiInputs = MidiInput::getDevices();
    if (midiInputs.size() == midiInputArray.size()) {
        return;
    }

    midiInputArray = midiInputs;
    auto prev = midiInputList->getText();
    midiInputList->onChange = [this] { };
    midiInputList->clear();
    midiInputList->addItemList (midiInputs, 1);
    int idx = midiInputs.indexOf(prev);
    if (idx != -1) {
        setMidiInput(midiInputs.indexOf(prev));
        midiInputList->onChange = [this] { setMidiInput (midiInputList->getSelectedItemIndex()); };
    }
    else {
        // find the first enabled device and use that by default
        for (auto midiInput : midiInputs)
        {
//            if (deviceManager.isMidiInputEnabled (midiInput))
//            {
                if (midiInput.contains("Vindor")) {
                    setMidiInput (midiInputs.indexOf (midiInput));
                    break;
                }
//            }
        }
        midiInputList->onChange = [this] {
            setMidiInput (midiInputList->getSelectedItemIndex());
        };
    }
}

void Player::setMidiInput (int index)
{
    if (index<0) return;
    auto list = MidiInput::getDevices();
    deviceManager.removeMidiInputCallback (list[lastInputIndex], this);
    auto newInput = list[index];
    if (! deviceManager.isMidiInputEnabled (newInput))
        deviceManager.setMidiInputEnabled (newInput, true);
    deviceManager.addMidiInputCallback (newInput, this);
    midiInputList->setSelectedId (index + 1, dontSendNotification);
    lastInputIndex = index;
}
//[/MiscUserDefs]

//==============================================================================
Player::Player ()
    : keyboardComponent (keyboardState, MidiKeyboardComponent::horizontalKeyboard),
      startTime (Time::getMillisecondCounterHiRes() * 0.001)
{
    //[Constructor_pre] You can add your own custom stuff here..
    //[/Constructor_pre]

    soundCategory.reset (new juce::ComboBox ("new combo box"));
    addAndMakeVisible (soundCategory.get());
    soundCategory->setEditableText (false);
    soundCategory->setJustificationType (juce::Justification::centredLeft);
    soundCategory->setTextWhenNothingSelected (juce::String());
    soundCategory->setTextWhenNoChoicesAvailable (TRANS("(no choices)"));
    soundCategory->addListener (this);

    soundCategory->setBounds (8, 81, 128, 32);

    soundList.reset (new juce::ComboBox ("new combo box"));
    addAndMakeVisible (soundList.get());
    soundList->setEditableText (false);
    soundList->setJustificationType (juce::Justification::centredLeft);
    soundList->setTextWhenNothingSelected (juce::String());
    soundList->setTextWhenNoChoicesAvailable (TRANS("(no choices)"));
    soundList->addListener (this);

    soundList->setBounds (144, 81, 128, 32);

    midiInputList.reset (new juce::ComboBox ("new combo box"));
    addAndMakeVisible (midiInputList.get());
    midiInputList->setEditableText (false);
    midiInputList->setJustificationType (juce::Justification::centredLeft);
    midiInputList->setTextWhenNothingSelected (juce::String());
    midiInputList->setTextWhenNoChoicesAvailable (TRANS("(no choices)"));
    midiInputList->addListener (this);

    midiInputList->setBounds (8, 33, 128, 32);

    volumeSlider.reset (new juce::Slider ("new slider"));
    addAndMakeVisible (volumeSlider.get());
    volumeSlider->setRange (0, 1, 0.01);
    volumeSlider->setSliderStyle (juce::Slider::Rotary);
    volumeSlider->setTextBoxStyle (juce::Slider::NoTextBox, false, 80, 20);
    volumeSlider->addListener (this);

    volumeSlider->setBounds (296, 33, 88, 80);

    roomSlider.reset (new juce::Slider ("new slider"));
    addAndMakeVisible (roomSlider.get());
    roomSlider->setRange (0, 1, 0.01);
    roomSlider->setSliderStyle (juce::Slider::Rotary);
    roomSlider->setTextBoxStyle (juce::Slider::NoTextBox, false, 80, 20);
    roomSlider->addListener (this);

    roomSlider->setBounds (392, 33, 88, 80);

    label.reset (new juce::Label ("new label",
                                  TRANS("Volume")));
    addAndMakeVisible (label.get());
    label->setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    label->setJustificationType (juce::Justification::centred);
    label->setEditable (false, false, false);
    label->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    label->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    label->setBounds (304, 105, 71, 24);

    label2.reset (new juce::Label ("new label",
                                   TRANS("Reverb")));
    addAndMakeVisible (label2.get());
    label2->setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    label2->setJustificationType (juce::Justification::centred);
    label2->setEditable (false, false, false);
    label2->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    label2->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    label2->setBounds (400, 105, 71, 24);

    panic.reset (new juce::TextButton ("new button"));
    addAndMakeVisible (panic.get());
    panic->setButtonText (TRANS("RESET"));
    panic->addListener (this);

    panic->setBounds (496, 57, 52, 24);

    www.reset (new juce::HyperlinkButton (TRANS("vindormusic.com"),
                                          URL ("http://www.vindormusic.com")));
    addAndMakeVisible (www.get());
    www->setTooltip (TRANS("http://www.vindormusic.com"));
    www->setButtonText (TRANS("vindormusic.com"));

    www->setBounds (376, 216, 128, 24);


    //[UserPreSize]
//    addAndMakeVisible (midiInputList);
    midiInputList->setTextWhenNoChoicesAvailable ("No MIDI Inputs Enabled");
    queryMidiInputs();

#if 0
    midiInputArray = MidiInput::getDevices();
    midiInputList->addItemList (midiInputArray, 1);
    midiInputList->onChange = [this] { setMidiInput (midiInputList->getSelectedItemIndex()); };
    // find the first enabled device and use that by default
    for (auto midiInput : midiInputArray)
    {
        if (deviceManager.isMidiInputEnabled (midiInput))
        {
            if (midiInput.contains("Vindor")) {
                setMidiInput (midiInputArray.indexOf (midiInput));
                break;
            }
        }
    }
#endif

    addAndMakeVisible (keyboardComponent);
    keyboardState.addListener (this);

    //[/UserPreSize]

    setSize (600, 200);


    //[Constructor] You can add your own custom stuff here..
#if JUCE_MAC || JUCE_WINDOWS || JUCE_LINUX
    setSize (600, 400);
#endif

    int w = www->getWidth();
    int h = www->getHeight();
    www->setBounds(600-w, 200-h, w, h);

    progress = new ProgressBar(progressValue);
    progress->setTextToDisplay("Loading instruments...");
    addAndMakeVisible (progress);
    progress->setBounds (150, 75, 300, 50);

    keyboardComponent.setKeyWidth(48);
    keyboardComponent.setLowestVisibleKey(60);

    setAudioChannels (0, 2); // no inputs, two outputs
    //    AudioDeviceManager::AudioDeviceSetup setup;
    //    deviceManager.getAudioDeviceSetup(setup);
    //    setup.sampleRate = 44100;
    //    deviceManager.initialise(0, 2, nullptr, true, "", &setup);

    for(int i=0; cat[i].min >= 0; i++) {
        soundCategory->addItem(cat[i].name, i+1);
    }
//    soundCategory->setSelectedItemIndex(8);

    volumeSlider->setLookAndFeel(&knobLook);
    volumeSlider->setValue(0.75F);
    roomSlider->setLookAndFeel(&knobLook);
    roomSlider->setValue(0.0F);

    reverb.setBypassed(true);

//    logo = ImageCache::getFromMemory(BinaryData::Vindor_png, BinaryData::Vindor_pngSize);

    synthAudioSource.addChangeListener(this);
    synthAudioSource.startThread();
//    synthAudioSource.run();

    startTimer(1000);
    //[/Constructor]
}

Player::~Player()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

    soundCategory = nullptr;
    soundList = nullptr;
    midiInputList = nullptr;
    volumeSlider = nullptr;
    roomSlider = nullptr;
    label = nullptr;
    label2 = nullptr;
    panic = nullptr;
    www = nullptr;


    //[Destructor]. You can add your own custom destruction code here..
    shutdownAudio();
    synthAudioSource.terminate();
    //[/Destructor]
}

//==============================================================================
void Player::paint (juce::Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (juce::Colour (0xff323e44));

    //[UserPaint] Add your own custom painting code here..
//    float ratio = float(getParentWidth()) / float(logo.getWidth());
//    Image dlogo = logo.rescaled(getParentWidth()-10, logo.getHeight() * ratio -10);
//    g.drawImageAt(dlogo, 5, 5);
    //[/UserPaint]
}

void Player::resized()
{
    //[UserPreResize] Add your own custom resize code here..
#if JUCE_IOS
    auto platform = SystemStats::getDeviceDescription();
    if (platform.startsWith("iPhone")) {
        int idx = platform.indexOfChar(' ');
        if (idx <= 0) idx = 6;
        auto ver = platform.substring(idx).getCharPointer();
        int v = atoi(ver);
        if( v > 8)
        {
            Desktop &d = Desktop::getInstance();
            auto o = d.getCurrentOrientation();
            // add some space for the iPhone X and later "cutout" on top of screen
            if (o == Desktop::DisplayOrientation::rotatedClockwise || o == Desktop::DisplayOrientation::rotatedAntiClockwise) {
                keyboardComponent.setBounds(40, 160, getParentWidth()-80, 148);
            }
            else {
                keyboardComponent.setBounds(0, 160, getParentWidth(), 148);
            }
        }
        else {
            keyboardComponent.setBounds(0, 160, getParentWidth(), 148);
        }
    }
    else {
        keyboardComponent.setBounds(0, 160, getParentWidth(), 148);
    }
#else
    keyboardComponent.setBounds(0, 160, getParentWidth(), 148);
#endif

    www->setBounds(20, getParentHeight() - www->getHeight()-20, www->getWidth(), www->getHeight());

#if 0
    FlexBox inst;
    inst.flexDirection = FlexBox::Direction::row;
    inst.alignContent = FlexBox::AlignContent::flexStart;
    inst.items.add(FlexItem(128, 32, *soundCategory).withMargin(5.0F));
    inst.items.add(FlexItem(128, 32, *soundList).withMargin(5.0F));

    FlexBox left;
    left.flexDirection = FlexBox::Direction::column;
    left.alignContent = FlexBox::AlignContent::flexStart;
    left.items.add(FlexItem(128, 32, *midiInputList).withMargin(5.0F));
    left.items.add(FlexItem(256, 32, inst).withMargin(5.0F));

    FlexBox vol;
    vol.flexDirection = FlexBox::Direction::column;
    vol.items.add(FlexItem(80, 80, *volumeSlider));
    vol.items.add(FlexItem(80, 24, *label));

    FlexBox rev;
    rev.flexDirection = FlexBox::Direction::column;
    rev.items.add(FlexItem(80, 80, *roomSlider));
    rev.items.add(FlexItem(80, 24, *label2));

    FlexBox top;
    top.flexDirection = FlexBox::Direction::row;
    top.items.add(FlexItem(256, 64, left));
    top.items.add(FlexItem(80, 80, vol));
    top.items.add(FlexItem(80, 80, rev));
    top.items.add(FlexItem(24, 24, *panic));

    FlexBox layout;
    layout.flexWrap= FlexBox::Wrap::noWrap;
    layout.alignContent = FlexBox::AlignContent::stretch;
    layout.alignItems = FlexBox::AlignItems::stretch;
    layout.justifyContent = FlexBox::JustifyContent::spaceBetween ;
    int w = getLocalBounds().getWidth();
    layout.flexDirection = FlexBox::Direction::column;
    layout.items.add(FlexItem(256, 104, top));
    layout.items.add(FlexItem(w, 64, keyboardComponent));

    layout.performLayout(getLocalBounds());
#endif

    //[/UserPreResize]

    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void Player::comboBoxChanged (juce::ComboBox* comboBoxThatHasChanged)
{
    //[UsercomboBoxChanged_Pre]
    //[/UsercomboBoxChanged_Pre]

    if (comboBoxThatHasChanged == soundCategory.get())
    {
        //[UserComboBoxCode_soundCategory] -- add your combo box handling code here..
        int id = soundCategory->getSelectedId();
        id--;
        StringArray soundNames = synthAudioSource.getInstruments();
        soundList->clear();
        for (int i = cat[id].min; i < soundNames.size(); ++i)
        {
            if (i > cat[id].max) break;
//            const String item (soundNames[i]);
            const String item (midiNames[i]);
            soundList->addItem(item, i);
        }
        if (id==8 || id==9) { // reed or pipe
            soundList->setSelectedItemIndex(1);
        }
        else {
            soundList->setSelectedItemIndex(0);
        }
        //[/UserComboBoxCode_soundCategory]
    }
    else if (comboBoxThatHasChanged == soundList.get())
    {
        //[UserComboBoxCode_soundList] -- add your combo box handling code here..
        int id = soundList->getSelectedId();
        synthAudioSource.setInstrument(id);
        //[/UserComboBoxCode_soundList]
    }
    else if (comboBoxThatHasChanged == midiInputList.get())
    {
        //[UserComboBoxCode_midiInputList] -- add your combo box handling code here..
        //[/UserComboBoxCode_midiInputList]
    }

    //[UsercomboBoxChanged_Post]
    //[/UsercomboBoxChanged_Post]
}

void Player::sliderValueChanged (juce::Slider* sliderThatWasMoved)
{
    //[UsersliderValueChanged_Pre]
    #define REVERB(p) \
        Reverb::Parameters reverbParam; \
        reverbParam.p = sliderThatWasMoved->getValue(); \
        reverb.setParameters(reverbParam);
    //[/UsersliderValueChanged_Pre]

    if (sliderThatWasMoved == volumeSlider.get())
    {
        //[UserSliderCode_volumeSlider] -- add your slider handling code here..
        synthAudioSource.mainGain = volumeSlider->getValue();
        //[/UserSliderCode_volumeSlider]
    }
    else if (sliderThatWasMoved == roomSlider.get())
    {
        //[UserSliderCode_roomSlider] -- add your slider handling code here..
        Reverb::Parameters reverbParam;
        double v = sliderThatWasMoved->getValue();
        if (v == 0.0F) {
            reverb.setBypassed(true);
        }
        else {
            reverb.setBypassed(false);
            reverbParam.roomSize = v;
            reverbParam.width = v;
            reverb.setParameters(reverbParam);
        }
        //[/UserSliderCode_roomSlider]
    }

    //[UsersliderValueChanged_Post]
    //[/UsersliderValueChanged_Post]
}

void Player::buttonClicked (juce::Button* buttonThatWasClicked)
{
    //[UserbuttonClicked_Pre]
    //[/UserbuttonClicked_Pre]

    if (buttonThatWasClicked == panic.get())
    {
        //[UserButtonCode_panic] -- add your button handler code here..
        synthAudioSource.panic();
        //[/UserButtonCode_panic]
    }

    //[UserbuttonClicked_Post]
    //[/UserbuttonClicked_Post]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...
//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Projucer information section --

    This is where the Projucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="Player" componentName=""
                 parentClasses="public AudioAppComponent, private MidiInputCallback, private MidiKeyboardStateListener, public Timer, public ChangeListener"
                 constructorParams="" variableInitialisers="keyboardComponent (keyboardState, MidiKeyboardComponent::horizontalKeyboard),&#10;    startTime (Time::getMillisecondCounterHiRes() * 0.001)"
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="0" initialWidth="600" initialHeight="200">
  <BACKGROUND backgroundColour="ff323e44"/>
  <COMBOBOX name="new combo box" id="18cf088c6b6a376c" memberName="soundCategory"
            virtualName="" explicitFocusOrder="0" pos="8 81 128 32" editable="0"
            layout="33" items="" textWhenNonSelected="" textWhenNoItems="(no choices)"/>
  <COMBOBOX name="new combo box" id="1a4c2d8e54ee1104" memberName="soundList"
            virtualName="" explicitFocusOrder="0" pos="144 81 128 32" editable="0"
            layout="33" items="" textWhenNonSelected="" textWhenNoItems="(no choices)"/>
  <COMBOBOX name="new combo box" id="bce869f6480ae950" memberName="midiInputList"
            virtualName="" explicitFocusOrder="0" pos="8 33 128 32" editable="0"
            layout="33" items="" textWhenNonSelected="" textWhenNoItems="(no choices)"/>
  <SLIDER name="new slider" id="c5ccf351c7897d" memberName="volumeSlider"
          virtualName="" explicitFocusOrder="0" pos="296 33 88 80" min="0.0"
          max="1.0" int="0.01" style="Rotary" textBoxPos="NoTextBox" textBoxEditable="1"
          textBoxWidth="80" textBoxHeight="20" skewFactor="1.0" needsCallback="1"/>
  <SLIDER name="new slider" id="1d4377867616ae43" memberName="roomSlider"
          virtualName="" explicitFocusOrder="0" pos="392 33 88 80" min="0.0"
          max="1.0" int="0.01" style="Rotary" textBoxPos="NoTextBox" textBoxEditable="1"
          textBoxWidth="80" textBoxHeight="20" skewFactor="1.0" needsCallback="1"/>
  <LABEL name="new label" id="d744ea3d38ba2961" memberName="label" virtualName=""
         explicitFocusOrder="0" pos="304 105 71 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Volume" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15.0"
         kerning="0.0" bold="0" italic="0" justification="36"/>
  <LABEL name="new label" id="45614b846b089e85" memberName="label2" virtualName=""
         explicitFocusOrder="0" pos="400 105 71 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Reverb" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15.0"
         kerning="0.0" bold="0" italic="0" justification="36"/>
  <TEXTBUTTON name="new button" id="36d101e5bac736de" memberName="panic" virtualName=""
              explicitFocusOrder="0" pos="496 57 52 24" buttonText="RESET"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <HYPERLINKBUTTON name="new hyperlink" id="f22e12834ace089" memberName="www" virtualName=""
                   explicitFocusOrder="0" pos="376 216 128 24" tooltip="http://www.vindormusic.com"
                   buttonText="vindormusic.com" connectedEdges="0" needsCallback="0"
                   radioGroupId="0" url="http://www.vindormusic.com"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]

