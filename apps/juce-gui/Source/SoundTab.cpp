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

#include "SoundTab.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...
#include "LookStyle.h"
#include "VindorWrap.h"
class SimpleLayout {
public:
    enum { ORIGIN=0, OVER=1, RIGHT=2, BELOW=4, NOMARGIN=8 };
private:
    int margin = 10;
    Component *lastComponent;
    int lastPosition;
    void first(Component &obj) {
        obj.setBounds(0, 0, obj.getWidth(), obj.getHeight());
        lastComponent = &obj;
    }
public:
    void add(Component &obj, int pos)
    {
        if (pos == ORIGIN) {
            first(obj);
        }
        else {
            add(obj, pos, *lastComponent);
        }
    }
    void add(Component &obj, int pos, Component &relative)
    {
        int m = (pos & NOMARGIN)?0:margin;
        if (pos & RIGHT) {
            obj.setBounds(relative.getX() + relative.getWidth() + m, relative.getY(), obj.getWidth(), obj.getHeight());
        }
        else if (pos & BELOW) {
            obj.setBounds(relative.getX(), relative.getY() + relative.getHeight() + m, obj.getWidth(), obj.getHeight());
        }
        else if (pos & OVER) {
            obj.setBounds(relative.getX() + margin, relative.getY() + margin, obj.getWidth(), obj.getHeight());
        }
        lastComponent = &obj;
    }
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
//[/MiscUserDefs]

//==============================================================================
SoundTab::SoundTab ()
{
    //[Constructor_pre] You can add your own custom stuff here..
    //[/Constructor_pre]

    groupComponent.reset (new juce::GroupComponent ("new group",
                                                    TRANS("Modulation")));
    addAndMakeVisible (groupComponent.get());

    groupComponent->setBounds (-3, 286, 371, 109);

    groupComponent2.reset (new juce::GroupComponent ("new group",
                                                     TRANS("Effects")));
    addAndMakeVisible (groupComponent2.get());

    groupComponent2->setBounds (136, 401, 232, 122);

    groupComponent4.reset (new juce::GroupComponent ("new group",
                                                     TRANS("Low Pass Filter")));
    addAndMakeVisible (groupComponent4.get());

    groupComponent4->setBounds (-3, 399, 131, 124);

    resonance.reset (new juce::Slider ("resonance"));
    addAndMakeVisible (resonance.get());
    resonance->setTooltip (TRANS("Adjust the resonance at the cutoff frequency."));
    resonance->setRange (0.1, 5, 0.1);
    resonance->setSliderStyle (juce::Slider::Rotary);
    resonance->setTextBoxStyle (juce::Slider::NoTextBox, false, 80, 20);
    resonance->addListener (this);

    resonance->setBounds (64, 414, 60, 60);

    osc1.reset (new OscComponent());
    addAndMakeVisible (osc1.get());
    osc1->setName ("new component");

    osc1->setBounds (64, 8, 64, 280);

    cutoff.reset (new juce::Slider ("cutoff"));
    addAndMakeVisible (cutoff.get());
    cutoff->setTooltip (TRANS("Maximum cutoff of the filter."));
    cutoff->setRange (0, 24000, 100);
    cutoff->setSliderStyle (juce::Slider::Rotary);
    cutoff->setTextBoxStyle (juce::Slider::NoTextBox, false, 80, 20);
    cutoff->addListener (this);

    cutoff->setBounds (5, 415, 60, 60);

    osc2.reset (new OscComponent());
    addAndMakeVisible (osc2.get());
    osc2->setName ("new component");

    osc2->setBounds (144, 8, 64, 280);

    osc3.reset (new OscComponent());
    addAndMakeVisible (osc3.get());
    osc3->setName ("new component");

    osc3->setBounds (224, 8, 64, 280);

    noise.reset (new juce::Slider ("noise"));
    addAndMakeVisible (noise.get());
    noise->setTooltip (TRANS("Level of white noise to mix into the sound."));
    noise->setRange (0, 1, 0.01);
    noise->setSliderStyle (juce::Slider::Rotary);
    noise->setTextBoxStyle (juce::Slider::NoTextBox, false, 80, 20);
    noise->addListener (this);

    noise->setBounds (144, 414, 60, 60);

    label.reset (new juce::Label ("new label",
                                  TRANS("Noise")));
    addAndMakeVisible (label.get());
    label->setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    label->setJustificationType (juce::Justification::centred);
    label->setEditable (false, false, false);
    label->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    label->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    label->setBounds (152, 470, 40, 24);

    label2.reset (new juce::Label ("new label",
                                   TRANS("Cutoff \n"
                                   "(kHz)")));
    addAndMakeVisible (label2.get());
    label2->setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    label2->setJustificationType (juce::Justification::centred);
    label2->setEditable (false, false, false);
    label2->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    label2->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    label2->setBounds (8, 472, 56, 32);

    label3.reset (new juce::Label ("new label",
                                   TRANS("Res.")));
    addAndMakeVisible (label3.get());
    label3->setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    label3->setJustificationType (juce::Justification::centred);
    label3->setEditable (false, false, false);
    label3->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    label3->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    label3->setBounds (72, 470, 48, 24);

    lfoFreq.reset (new juce::Slider ("mod rate"));
    addAndMakeVisible (lfoFreq.get());
    lfoFreq->setTooltip (TRANS("Frequency of the Low Frequency Oscillator, which adjusts the frequency of the other oscillators."));
    lfoFreq->setRange (0, 20, 0.1);
    lfoFreq->setSliderStyle (juce::Slider::Rotary);
    lfoFreq->setTextBoxStyle (juce::Slider::NoTextBox, false, 80, 20);
    lfoFreq->addListener (this);

    lfoFreq->setBounds (185, 294, 88, 80);

    lfoDelay.reset (new juce::Slider ("mod delay"));
    addAndMakeVisible (lfoDelay.get());
    lfoDelay->setTooltip (TRANS("Delay after new note startes before engaging Low Frequency Oscillator. "));
    lfoDelay->setRange (0, 10000, 1);
    lfoDelay->setSliderStyle (juce::Slider::Rotary);
    lfoDelay->setTextBoxStyle (juce::Slider::NoTextBox, false, 80, 20);
    lfoDelay->addListener (this);

    lfoDelay->setBounds (279, 294, 88, 80);

    label10.reset (new juce::Label ("new label",
                                    TRANS("Rate (Hz)")));
    addAndMakeVisible (label10.get());
    label10->setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    label10->setJustificationType (juce::Justification::centred);
    label10->setEditable (false, false, false);
    label10->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    label10->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    label10->setBounds (185, 368, 88, 24);

    label12.reset (new juce::Label ("new label",
                                    TRANS("Delay (s)")));
    addAndMakeVisible (label12.get());
    label12->setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    label12->setJustificationType (juce::Justification::centred);
    label12->setEditable (false, false, false);
    label12->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    label12->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    label12->setBounds (279, 368, 88, 24);

    attack.reset (new juce::ToggleButton ("attack"));
    addAndMakeVisible (attack.get());
    attack->setTooltip (TRANS("Add brief emphasis to the beginning of every note that starts from silence."));
    attack->setButtonText (TRANS("Attack"));
    attack->addListener (this);

    attack->setBounds (280, 424, 80, 24);

    lfoWave.reset (new juce::Slider ("mod wave"));
    addAndMakeVisible (lfoWave.get());
    lfoWave->setTooltip (TRANS("Type of wave for the Low Frequency Oscillator."));
    lfoWave->setRange (0, 4, 1);
    lfoWave->setSliderStyle (juce::Slider::Rotary);
    lfoWave->setTextBoxStyle (juce::Slider::NoTextBox, false, 80, 20);
    lfoWave->addListener (this);

    lfoWave->setBounds (-3, 294, 88, 80);

    lfoLevel.reset (new juce::Slider ("mod level"));
    addAndMakeVisible (lfoLevel.get());
    lfoLevel->setTooltip (TRANS("Intensity level of the modulation effect."));
    lfoLevel->setRange (0, 1, 0.01);
    lfoLevel->setSliderStyle (juce::Slider::Rotary);
    lfoLevel->setTextBoxStyle (juce::Slider::NoTextBox, false, 80, 20);
    lfoLevel->addListener (this);

    lfoLevel->setBounds (91, 294, 88, 80);

    label11.reset (new juce::Label ("new label",
                                    TRANS("Level")));
    addAndMakeVisible (label11.get());
    label11->setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    label11->setJustificationType (juce::Justification::centred);
    label11->setEditable (false, false, false);
    label11->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    label11->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    label11->setBounds (91, 368, 88, 24);

    label13.reset (new juce::Label ("new label",
                                    TRANS("Wave")));
    addAndMakeVisible (label13.get());
    label13->setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    label13->setJustificationType (juce::Justification::centred);
    label13->setEditable (false, false, false);
    label13->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    label13->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    label13->setBounds (-4, 368, 88, 24);

    page1.reset (new juce::ImageButton ("new button"));
    addAndMakeVisible (page1.get());
    page1->setButtonText (juce::String());
    page1->addListener (this);

    page1->setImages (false, true, true,
                      juce::ImageCache::getFromMemory (BinaryData::arrowleft_png, BinaryData::arrowleft_pngSize), 1.000f, juce::Colour (0x00000000),
                      juce::ImageCache::getFromMemory (BinaryData::arrowleft_png, BinaryData::arrowleft_pngSize), 1.000f, juce::Colour (0x00854444),
                      juce::ImageCache::getFromMemory (BinaryData::arrowleft_png, BinaryData::arrowleft_pngSize), 1.000f, juce::Colour (0x00000000));
    page1->setBounds (312, 8, 32, 32);

    page2.reset (new juce::ImageButton ("new button"));
    addAndMakeVisible (page2.get());
    page2->setButtonText (juce::String());
    page2->addListener (this);

    page2->setImages (false, true, true,
                      juce::ImageCache::getFromMemory (BinaryData::arrowleft_png, BinaryData::arrowleft_pngSize), 1.000f, juce::Colour (0x00000000),
                      juce::ImageCache::getFromMemory (BinaryData::arrowleft_png, BinaryData::arrowleft_pngSize), 1.000f, juce::Colour (0x00000000),
                      juce::ImageCache::getFromMemory (BinaryData::arrowleft_png, BinaryData::arrowleft_pngSize), 1.000f, juce::Colour (0x00000000));
    page2->setBounds (344, 8, 32, 32);

    label5.reset (new juce::Label ("new label",
                                   TRANS("Wave")));
    addAndMakeVisible (label5.get());
    label5->setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    label5->setJustificationType (juce::Justification::centredRight);
    label5->setEditable (false, false, false);
    label5->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    label5->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    label5->setBounds (0, 26, 60, 24);

    label6.reset (new juce::Label ("new label",
                                   TRANS("Octave")));
    addAndMakeVisible (label6.get());
    label6->setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    label6->setJustificationType (juce::Justification::centredRight);
    label6->setEditable (false, false, false);
    label6->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    label6->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    label6->setBounds (0, 82, 60, 24);

    label7.reset (new juce::Label ("new label",
                                   TRANS("Detune")));
    addAndMakeVisible (label7.get());
    label7->setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    label7->setJustificationType (juce::Justification::centredRight);
    label7->setEditable (false, false, false);
    label7->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    label7->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    label7->setBounds (0, 139, 60, 24);

    label8.reset (new juce::Label ("new label",
                                   TRANS("Mix")));
    addAndMakeVisible (label8.get());
    label8->setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    label8->setJustificationType (juce::Justification::centredRight);
    label8->setEditable (false, false, false);
    label8->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    label8->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    label8->setBounds (0, 216, 60, 24);

    chorus.reset (new juce::Slider ("chorus"));
    addAndMakeVisible (chorus.get());
    chorus->setTooltip (TRANS("Number of chorus channels"));
    chorus->setRange (0, 4, 1);
    chorus->setSliderStyle (juce::Slider::Rotary);
    chorus->setTextBoxStyle (juce::Slider::NoTextBox, false, 80, 20);
    chorus->addListener (this);

    chorus->setBounds (216, 414, 60, 60);

    label4.reset (new juce::Label ("new label",
                                   TRANS("Chorus")));
    addAndMakeVisible (label4.get());
    label4->setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    label4->setJustificationType (juce::Justification::centred);
    label4->setEditable (false, false, false);
    label4->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    label4->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    label4->setBounds (216, 471, 56, 24);


    //[UserPreSize]
    osc1->setNumber(0);
    osc2->setNumber(1);
    osc3->setNumber(2);

#if 0
    for(int i=1; i<128; i++) {
        String s = String::formatted("%d %s", i, midiNames[i]);
        midiInstrument->addItem(s, i);
    }
#endif

#if JUCE_MAC || JUCE_WINDOWS || JUCE_LINUX
    page1->setVisible(false);
    page2->setVisible(false);
#else
    int num = getNumChildComponents();
    store_x = new int[num];
    store_y = new int[num];
    for(int i=0; i<num; i++) {
        Component *c = getChildComponent(i);
        Rectangle<int> r = c->getBounds();
        store_x[i] = r.getX();
        store_y[i] = r.getY();
    }
#endif

    //[/UserPreSize]

    setSize (600, 400);


    //[Constructor] You can add your own custom stuff here..
    resonance->setLookAndFeel(&resonanceLook);
    cutoff->setLookAndFeel(&cutoffLook);
    noise->setLookAndFeel(&unipolarLook);
    chorus->setLookAndFeel(&chorusLook);
//    echo->setLookAndFeel(&unipolarLook);
    lfoFreq->setLookAndFeel(&lfoFreqLook);
    lfoDelay->setLookAndFeel(&lfoDelayLook);
    lfoWave->setLookAndFeel(&lfoWaveLook);
    lfoLevel->setLookAndFeel(&lfoLevelLook);
//    midiInstrument->setLookAndFeel(&midiLook);

    deviceControl->addChangeListener(this);
    //[/Constructor]
}

SoundTab::~SoundTab()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

    groupComponent = nullptr;
    groupComponent2 = nullptr;
    groupComponent4 = nullptr;
    resonance = nullptr;
    osc1 = nullptr;
    cutoff = nullptr;
    osc2 = nullptr;
    osc3 = nullptr;
    noise = nullptr;
    label = nullptr;
    label2 = nullptr;
    label3 = nullptr;
    lfoFreq = nullptr;
    lfoDelay = nullptr;
    label10 = nullptr;
    label12 = nullptr;
    attack = nullptr;
    lfoWave = nullptr;
    lfoLevel = nullptr;
    label11 = nullptr;
    label13 = nullptr;
    page1 = nullptr;
    page2 = nullptr;
    label5 = nullptr;
    label6 = nullptr;
    label7 = nullptr;
    label8 = nullptr;
    chorus = nullptr;
    label4 = nullptr;


    //[Destructor]. You can add your own custom destruction code here..
	// deviceControl->removeChangeListener(this);
#if JUCE_MAC || JUCE_WINDOWS || JUCE_LINUX
#else
    delete[] store_x;
    delete[] store_y;
#endif
    //[/Destructor]
}

//==============================================================================
void SoundTab::paint (juce::Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (juce::Colour (0xff323e44));

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void SoundTab::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    /*
    FlexBox oscillators;
    FlexItem left  (64, 300, *osc1);
    left.maxWidth = 100;
    FlexItem right (64, 300, *osc3);
    right.maxWidth = 100;
    FlexItem main  (64, 300, *osc2);
    main.maxWidth = 100;
    oscillators.items.addArray ( { left, main, right } );

    FlexBox top;
    FlexItem inst(150, 24, *instrument);
    FlexItem p1(20, 40, *page1);
    FlexItem p2(20, 40, *page2);
    FlexItem play(120, 24, *playStopButton);
    top.items.addArray({ inst, p1, p2, play } );

    FlexBox

    oscillators.performLayout (getLocalBounds().toFloat());
    */


    /*
    SimpleLayout layout;

    layout.add(*instrument, SimpleLayout::ORIGIN);

    layout.add(*page1, SimpleLayout::RIGHT);
    layout.add(*page2, SimpleLayout::RIGHT);
    layout.add(*playStopButton, SimpleLayout::RIGHT);

    layout.add(*label5, SimpleLayout::BELOW, *instrument);
    layout.add(*osc1, SimpleLayout::RIGHT);
    layout.add(*osc2, SimpleLayout::RIGHT);
    layout.add(*osc3, SimpleLayout::RIGHT);

    layout.add(*groupComponent4, SimpleLayout::RIGHT, *osc3);
    layout.add(*cutoff, SimpleLayout::OVER, *groupComponent4);
    layout.add(*label2, SimpleLayout::BELOW | SimpleLayout::NOMARGIN );
    layout.add(*resonance, SimpleLayout::RIGHT, *cutoff);
    layout.add(*label3, SimpleLayout::BELOW | SimpleLayout::NOMARGIN);

    layout.add(*groupComponent2, SimpleLayout::BELOW, *groupComponent4);
    layout.add(*noise, SimpleLayout::OVER, *groupComponent2);
    layout.add(*label, SimpleLayout::BELOW | SimpleLayout::NOMARGIN, *noise);
    layout.add(*attack, SimpleLayout::RIGHT, *noise);

    layout.add(*groupComponent, SimpleLayout::BELOW, *osc1);
    layout.add(*lfoWave, SimpleLayout::OVER, *groupComponent);
    layout.add(*label13, SimpleLayout::BELOW | SimpleLayout::NOMARGIN);
    layout.add(*lfoLevel, SimpleLayout::RIGHT, *lfoWave);
    layout.add(*label11, SimpleLayout::BELOW | SimpleLayout::NOMARGIN);
    layout.add(*lfoFreq, SimpleLayout::RIGHT, *lfoLevel);
    layout.add(*label10, SimpleLayout::BELOW | SimpleLayout::NOMARGIN);
    layout.add(*lfoDelay, SimpleLayout::RIGHT, *lfoFreq);
    layout.add(*label12, SimpleLayout::BELOW | SimpleLayout::NOMARGIN);
*/

//    scaleComponents(this, 400.0 / getWidth(), 600.0 / getHeight());
    //[/UserPreResize]

    //[UserResized] Add your own custom resize handling here..

    moveComponents(0, true);

#if JUCE_MAC || JUCE_WINDOWS || JUCE_LINUX
#else
    Image p1, p2;

    int landscape = getWidth() > getHeight();

    int w = page1->getWidth();

    if (landscape) {
        p1 = ImageCache::getFromMemory (BinaryData::arrowup_png, BinaryData::arrowup_pngSize);
        p2 = ImageCache::getFromMemory (BinaryData::arrowdown_png, BinaryData::arrowdown_pngSize);
        page1->setBounds (getWidth() - w-5, getHeight() - w*2-10, w, w);
        page2->setBounds (getWidth() - w-5, getHeight() - w-5, w, w);
    }
    else {
        p1 = ImageCache::getFromMemory (BinaryData::arrowleft_png, BinaryData::arrowleft_pngSize);
        p2 = ImageCache::getFromMemory (BinaryData::arrowright_png, BinaryData::arrowright_pngSize);
        page1->setBounds (getWidth() - w*2-10, getHeight() - w-5, w, w);
        page2->setBounds (getWidth() - w-5, getHeight() - w-5, w, w);
    }

    page1->setImages (false, true, true,
                      p1, 1.000f, Colour (0x00000000),
                      p1, 1.000f, Colour (0x00854444),
                      p1, 1.000f, Colour (0x00000000));

    page2->setImages (false, true, true,
                      p2, 1.000f, Colour (0x00000000),
                      p2, 1.000f, Colour (0x00854444),
                      p2, 1.000f, Colour (0x00000000));
#endif

    //[/UserResized]
}

void SoundTab::sliderValueChanged (juce::Slider* sliderThatWasMoved)
{
    //[UsersliderValueChanged_Pre]
    //[/UsersliderValueChanged_Pre]

    if (sliderThatWasMoved == resonance.get())
    {
        //[UserSliderCode_resonance] -- add your slider handling code here..
        double v = resonance->getValue();
        int x = v / 10.0 * 127.0;
        deviceControl->sendFilterSetting(2, x);
        //[/UserSliderCode_resonance]
    }
    else if (sliderThatWasMoved == cutoff.get())
    {
        //[UserSliderCode_cutoff] -- add your slider handling code here..
        double v = cutoff->getValue();
        int n = v / 100.0F;  // support test firmware, eliminate in future
        deviceControl->sendFilterSetting(1, n); // same as above
        deviceControl->sendFilterSettingLong(17, int(v), 3);
        //[/UserSliderCode_cutoff]
    }
    else if (sliderThatWasMoved == noise.get())
    {
        //[UserSliderCode_noise] -- add your slider handling code here..
        deviceControl->sendFilterSetting(12, noise->getValue() * 127.0);
        //[/UserSliderCode_noise]
    }
    else if (sliderThatWasMoved == lfoFreq.get())
    {
        //[UserSliderCode_lfoFreq] -- add your slider handling code here..
        double v = lfoFreq->getValue();
        int n = v * 100.0;
        int b1 = n & 0x7F;
        int b2 = (n >> 7) & 0x7F;
        int b3 = (n >> 14) & 0x7F;
        deviceControl->sendFilterSetting(7, b1, b2, b3);
        //[/UserSliderCode_lfoFreq]
    }
    else if (sliderThatWasMoved == lfoDelay.get())
    {
        //[UserSliderCode_lfoDelay] -- add your slider handling code here..
        double v = lfoDelay->getValue();
        int n = v;
        int b1 = n & 0x7F;
        int b2 = (n >> 7) & 0x7F;
        int b3 = (n >> 14) & 0x7F;
        deviceControl->sendFilterSetting(8, b1, b2, b3);
        //[/UserSliderCode_lfoDelay]
    }
    else if (sliderThatWasMoved == lfoWave.get())
    {
        //[UserSliderCode_lfoWave] -- add your slider handling code here..
        int r = id2waveSlide(lfoWave->getValue());
        deviceControl->sendFilterSetting(13, r);
        //[/UserSliderCode_lfoWave]
    }
    else if (sliderThatWasMoved == lfoLevel.get())
    {
        //[UserSliderCode_lfoLevel] -- add your slider handling code here..
        double v = lfoLevel->getValue();
        int b = v * 127.0;
        for(int i=0; i<3; i++) {
            deviceControl->sendFilterSetting(9, i, b);
        }
        //[/UserSliderCode_lfoLevel]
    }
    else if (sliderThatWasMoved == chorus.get())
    {
        //[UserSliderCode_chorus] -- add your slider handling code here..
        int v = chorus->getValue();
        deviceControl->sendFilterSetting(11, v);
        //[/UserSliderCode_chorus]
    }

    //[UsersliderValueChanged_Post]
    //[/UsersliderValueChanged_Post]
}

void SoundTab::buttonClicked (juce::Button* buttonThatWasClicked)
{
    //[UserbuttonClicked_Pre]
    //[/UserbuttonClicked_Pre]

    if (buttonThatWasClicked == attack.get())
    {
        //[UserButtonCode_attack] -- add your button handler code here..
        int v = attack->getToggleState();
        deviceControl->sendFilterSetting(14, v);
        //[/UserButtonCode_attack]
    }
    else if (buttonThatWasClicked == page1.get())
    {
        //[UserButtonCode_page1] -- add your button handler code here..
        moveComponents(+100);
        //[/UserButtonCode_page1]
    }
    else if (buttonThatWasClicked == page2.get())
    {
        //[UserButtonCode_page2] -- add your button handler code here..
        moveComponents(-100);
        //[/UserButtonCode_page2]
    }

    //[UserbuttonClicked_Post]
    //[/UserbuttonClicked_Post]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...
void SoundTab::changeListenerCallback (ChangeBroadcaster *source) {
    if (! deviceControl->isValid()) return;
    loadSettings();
#ifdef JUCE_DEBUG
    dumpSettings();
#endif
}

int SoundTab::waveSlide2Id(int value) {
    int w = 0;
    switch(value) {
        case WAVEFORM_SINE: w=0; break;
        case WAVEFORM_TRIANGLE: w=1; break;
        case WAVEFORM_SQUARE: w=2; break;
        case WAVEFORM_SAWTOOTH: w=3; break;
        case WAVEFORM_SAWTOOTH_REVERSE: w=4; break;
    }
    return w;
}

int SoundTab::id2waveSlide(int value) {
    int w = 0;
    switch(value) {
        case 0: return WAVEFORM_SINE;
        case 1: return WAVEFORM_TRIANGLE;
        case 2: return WAVEFORM_SQUARE;
        case 3: return WAVEFORM_SAWTOOTH;
        case 4: return WAVEFORM_SAWTOOTH_REVERSE;
    }
    return w;
}

void SoundTab::loadSettings()
{
	if (deviceControl->synth.synth_use != ANALOG_MODEL) {
		return;
	}

    // int inst = deviceControl->settings.getInt("INST");
    // instrument->setSelectedId(inst + 1, dontSendNotification);

    osc1->loadSettings();
    osc2->loadSettings();
    osc3->loadSettings();

    resonance->setValue(deviceControl->synth.u.analog.filterResonance, NotificationType::dontSendNotification);
    cutoff->setValue(deviceControl->synth.u.analog.filterFreq, NotificationType::dontSendNotification);
    noise->setValue(deviceControl->synth.u.analog.fil_vol_noise, NotificationType::dontSendNotification);
    chorus->setValue(deviceControl->synth.u.analog.fil_chorus1_num, NotificationType::dontSendNotification);
//    echo->setValue(deviceControl->synth.u.analog.fil_echo_num, NotificationType::dontSendNotification);
    lfoWave->setValue(deviceControl->synth.u.analog.fil_lfo_wave, NotificationType::dontSendNotification);
    lfoFreq->setValue(deviceControl->synth.u.analog.fil_lfo_freq, NotificationType::dontSendNotification);
    lfoDelay->setValue(deviceControl->synth.u.analog.fil_lfo_delay, NotificationType::dontSendNotification);
    lfoLevel->setValue(deviceControl->synth.u.analog.fil_lfo_mix[0], NotificationType::dontSendNotification);
    int w = waveSlide2Id(deviceControl->synth.u.analog.fil_lfo_wave);
    lfoWave->setValue(w, NotificationType::dontSendNotification);

    attack->setToggleState(deviceControl->synth.u.analog.playAttack, dontSendNotification);

//    midiInstrument->setSelectedId(deviceControl->synth.midiNumber, dontSendNotification);
}

void SoundTab::scaleComponents(Component *parent, float x, float y)
{
    if (prev_w==0) prev_w = 600;
    if (prev_h==0) prev_h = 400;

    float w = parent->getWidth();
    float h = parent->getHeight();

    if (w==0 || h==0) return;

    x = w / prev_w;
    y = h / prev_h;

    if (x==1 && y==1) return;

    if (x>10 || y>10) return;
    if (x<.5 || y<.5) return;

    int num = parent->getNumChildComponents();
    for(int i=0; i<num; i++) {
        Component *c = parent->getChildComponent(i);
        if (! (c == page1.get() || c == page2.get())) {
            Rectangle<int> r = c->getBounds();
            c->setBounds(r.getX() * x, r.getY() * y, r.getWidth() * x, r.getHeight() * y);
        }
    }

    prev_w = parent->getWidth();
    prev_h = parent->getHeight();
}

void SoundTab::moveComponents(int x, bool absolute)
{
#if JUCE_MAC || JUCE_WINDOWS || JUCE_LINUX
#else
    static int offset = 0;

    if (absolute) {
        int num = getNumChildComponents();
        for(int i=0; i<num; i++) {
            Component *c = getChildComponent(i);
            if (! (c == page1.get() || c == page2.get())) {
                Rectangle<int> r = c->getBounds();
                c->setBounds(store_x[i], store_y[i], r.getWidth(), r.getHeight());
            }
        }
        offset = 0;
    }

    int landscape = getWidth() > getHeight();

    offset -= x;
    if (offset < 0) {
        offset += x;
        return;
    }

    if (landscape) {
        if (offset >= getHeight()) {
            offset += x;
            return;
        }
    }
    else {
        if (offset >= getWidth()) {
            offset += x;
            return;
        }
    }

    int num = getNumChildComponents();
    for(int i=0; i<num; i++) {
        Component *c = getChildComponent(i);
        if (! (c == page1.get() || c == page2.get())) {
            Rectangle<int> r = c->getBounds();
            if (landscape) {
                c->setBounds(r.getX(), r.getY() + x, r.getWidth(), r.getHeight());
            }
            else {
                c->setBounds(r.getX() + x, r.getY(), r.getWidth(), r.getHeight());
            }
        }
    }
#endif
}

#ifdef JUCE_DEBUG

const char *waveName(int x) {
    switch(x) {
        case WAVEFORM_SINE: return "WAVEFORM_SINE";
        case WAVEFORM_TRIANGLE: return "WAVEFORM_TRIANGLE";
        case WAVEFORM_SQUARE: return "WAVEFORM_SQUARE";
        case WAVEFORM_SAWTOOTH: return "WAVEFORM_SAWTOOTH";
        case WAVEFORM_SAWTOOTH_REVERSE: return "WAVEFORM_SAWTOOTH_REVERSE";
    }
    return "UNK";
}

#define DF(x) sprintf(str, "%s = %f;", #x, s->u.analog.x); DBG(str);
#define DI(x) sprintf(str, "%s = %d;", #x, s->u.analog.x); DBG(str);
#define DW(x) sprintf(str, "%s = %s;", #x, waveName(s->u.analog.x)); DBG(str);

void SoundTab::dumpSettings()
{
    char str[1024];
    SynthSettings *s = &deviceControl->synth;

    DF(filterResonance);
    DF(filterFreq);
    DW(fil_wave[0]);
    DW(fil_wave[1]);
    DW(fil_wave[2]);
    DF(fil_vol[0]);
    DF(fil_vol[1]);
    DF(fil_vol[2]);
    DF(fil_freq[0]);
    DF(fil_freq[1]);
    DF(fil_freq[2]);
    DF(fil_freq_bias[0]);
    DF(fil_freq_bias[1]);
    DF(fil_freq_bias[2]);
    DF(fil_vol_noise);
    DI(playAttack);
    DW(fil_lfo_wave);
    DF(fil_lfo_freq);
    DF(fil_lfo_delay);
    DF(fil_lfo_mix[0]);
    DF(fil_lfo_mix[1]);
    DF(fil_lfo_mix[2]);
}

#endif

//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Projucer information section --

    This is where the Projucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="SoundTab" componentName=""
                 parentClasses="public Component, public ChangeListener" constructorParams=""
                 variableInitialisers="" snapPixels="8" snapActive="1" snapShown="1"
                 overlayOpacity="0.330" fixedSize="0" initialWidth="600" initialHeight="400">
  <BACKGROUND backgroundColour="ff323e44"/>
  <GROUPCOMPONENT name="new group" id="ff6fb957d0291eaa" memberName="groupComponent"
                  virtualName="" explicitFocusOrder="0" pos="-3 286 371 109" title="Modulation"/>
  <GROUPCOMPONENT name="new group" id="a590c993ca42501f" memberName="groupComponent2"
                  virtualName="" explicitFocusOrder="0" pos="136 401 232 122" title="Effects"/>
  <GROUPCOMPONENT name="new group" id="e8093f8600b4c396" memberName="groupComponent4"
                  virtualName="" explicitFocusOrder="0" pos="-3 399 131 124" title="Low Pass Filter"/>
  <SLIDER name="resonance" id="8104679847b9b6eb" memberName="resonance"
          virtualName="" explicitFocusOrder="0" pos="64 414 60 60" tooltip="Adjust the resonance at the cutoff frequency."
          min="0.1" max="5.0" int="0.1" style="Rotary" textBoxPos="NoTextBox"
          textBoxEditable="1" textBoxWidth="80" textBoxHeight="20" skewFactor="1.0"
          needsCallback="1"/>
  <GENERICCOMPONENT name="new component" id="3d9df87b0c51371e" memberName="osc1"
                    virtualName="" explicitFocusOrder="0" pos="64 8 64 280" class="OscComponent"
                    params=""/>
  <SLIDER name="cutoff" id="506fff5c63945e6b" memberName="cutoff" virtualName=""
          explicitFocusOrder="0" pos="5 415 60 60" tooltip="Maximum cutoff of the filter."
          min="0.0" max="24000.0" int="100.0" style="Rotary" textBoxPos="NoTextBox"
          textBoxEditable="1" textBoxWidth="80" textBoxHeight="20" skewFactor="1.0"
          needsCallback="1"/>
  <GENERICCOMPONENT name="new component" id="ca7f272d98cbc583" memberName="osc2"
                    virtualName="" explicitFocusOrder="0" pos="144 8 64 280" class="OscComponent"
                    params=""/>
  <GENERICCOMPONENT name="new component" id="967c4008edd405e3" memberName="osc3"
                    virtualName="" explicitFocusOrder="0" pos="224 8 64 280" class="OscComponent"
                    params=""/>
  <SLIDER name="noise" id="3a77141ac150587b" memberName="noise" virtualName=""
          explicitFocusOrder="0" pos="144 414 60 60" tooltip="Level of white noise to mix into the sound."
          min="0.0" max="1.0" int="0.01" style="Rotary" textBoxPos="NoTextBox"
          textBoxEditable="1" textBoxWidth="80" textBoxHeight="20" skewFactor="1.0"
          needsCallback="1"/>
  <LABEL name="new label" id="3d6aac5f5c828c28" memberName="label" virtualName=""
         explicitFocusOrder="0" pos="152 470 40 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Noise" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15.0"
         kerning="0.0" bold="0" italic="0" justification="36"/>
  <LABEL name="new label" id="f3dfcb3af7dfd4da" memberName="label2" virtualName=""
         explicitFocusOrder="0" pos="8 472 56 32" edTextCol="ff000000"
         edBkgCol="0" labelText="Cutoff &#10;(kHz)" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="15.0" kerning="0.0" bold="0" italic="0" justification="36"/>
  <LABEL name="new label" id="ee269eef93f9105f" memberName="label3" virtualName=""
         explicitFocusOrder="0" pos="72 470 48 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Res." editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15.0"
         kerning="0.0" bold="0" italic="0" justification="36"/>
  <SLIDER name="mod rate" id="de4abc7db1ddf580" memberName="lfoFreq" virtualName=""
          explicitFocusOrder="0" pos="185 294 88 80" tooltip="Frequency of the Low Frequency Oscillator, which adjusts the frequency of the other oscillators."
          min="0.0" max="20.0" int="0.1" style="Rotary" textBoxPos="NoTextBox"
          textBoxEditable="1" textBoxWidth="80" textBoxHeight="20" skewFactor="1.0"
          needsCallback="1"/>
  <SLIDER name="mod delay" id="d766e12da49cc1e1" memberName="lfoDelay"
          virtualName="" explicitFocusOrder="0" pos="279 294 88 80" tooltip="Delay after new note startes before engaging Low Frequency Oscillator. "
          min="0.0" max="10000.0" int="1.0" style="Rotary" textBoxPos="NoTextBox"
          textBoxEditable="1" textBoxWidth="80" textBoxHeight="20" skewFactor="1.0"
          needsCallback="1"/>
  <LABEL name="new label" id="6a74a543bb5388d5" memberName="label10" virtualName=""
         explicitFocusOrder="0" pos="185 368 88 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Rate (Hz)" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15.0"
         kerning="0.0" bold="0" italic="0" justification="36"/>
  <LABEL name="new label" id="486e6343c462b240" memberName="label12" virtualName=""
         explicitFocusOrder="0" pos="279 368 88 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Delay (s)" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15.0"
         kerning="0.0" bold="0" italic="0" justification="36"/>
  <TOGGLEBUTTON name="attack" id="7c821d8019941cd" memberName="attack" virtualName=""
                explicitFocusOrder="0" pos="280 424 80 24" tooltip="Add brief emphasis to the beginning of every note that starts from silence."
                buttonText="Attack" connectedEdges="0" needsCallback="1" radioGroupId="0"
                state="0"/>
  <SLIDER name="mod wave" id="552637a1308c9799" memberName="lfoWave" virtualName=""
          explicitFocusOrder="0" pos="-3 294 88 80" tooltip="Type of wave for the Low Frequency Oscillator."
          min="0.0" max="4.0" int="1.0" style="Rotary" textBoxPos="NoTextBox"
          textBoxEditable="1" textBoxWidth="80" textBoxHeight="20" skewFactor="1.0"
          needsCallback="1"/>
  <SLIDER name="mod level" id="1c5bff66823fc3eb" memberName="lfoLevel"
          virtualName="" explicitFocusOrder="0" pos="91 294 88 80" tooltip="Intensity level of the modulation effect."
          min="0.0" max="1.0" int="0.01" style="Rotary" textBoxPos="NoTextBox"
          textBoxEditable="1" textBoxWidth="80" textBoxHeight="20" skewFactor="1.0"
          needsCallback="1"/>
  <LABEL name="new label" id="5bee6d74fed37751" memberName="label11" virtualName=""
         explicitFocusOrder="0" pos="91 368 88 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Level" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15.0"
         kerning="0.0" bold="0" italic="0" justification="36"/>
  <LABEL name="new label" id="4d67cf72fef5eab3" memberName="label13" virtualName=""
         explicitFocusOrder="0" pos="-4 368 88 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Wave" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15.0"
         kerning="0.0" bold="0" italic="0" justification="36"/>
  <IMAGEBUTTON name="new button" id="ae922de4669d2d20" memberName="page1" virtualName=""
               explicitFocusOrder="0" pos="312 8 32 32" buttonText="" connectedEdges="0"
               needsCallback="1" radioGroupId="0" keepProportions="1" resourceNormal="BinaryData::arrowleft_png"
               opacityNormal="1.0" colourNormal="0" resourceOver="BinaryData::arrowleft_png"
               opacityOver="1.0" colourOver="854444" resourceDown="BinaryData::arrowleft_png"
               opacityDown="1.0" colourDown="0"/>
  <IMAGEBUTTON name="new button" id="9c19694a46171805" memberName="page2" virtualName=""
               explicitFocusOrder="0" pos="344 8 32 32" buttonText="" connectedEdges="0"
               needsCallback="1" radioGroupId="0" keepProportions="1" resourceNormal="BinaryData::arrowleft_png"
               opacityNormal="1.0" colourNormal="0" resourceOver="BinaryData::arrowleft_png"
               opacityOver="1.0" colourOver="0" resourceDown="BinaryData::arrowleft_png"
               opacityDown="1.0" colourDown="0"/>
  <LABEL name="new label" id="1303aaf1dc43616b" memberName="label5" virtualName=""
         explicitFocusOrder="0" pos="0 26 60 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Wave" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15.0"
         kerning="0.0" bold="0" italic="0" justification="34"/>
  <LABEL name="new label" id="17a0d5dabc78d87" memberName="label6" virtualName=""
         explicitFocusOrder="0" pos="0 82 60 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Octave" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15.0"
         kerning="0.0" bold="0" italic="0" justification="34"/>
  <LABEL name="new label" id="f65e4f2d615be0d" memberName="label7" virtualName=""
         explicitFocusOrder="0" pos="0 139 60 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Detune" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15.0"
         kerning="0.0" bold="0" italic="0" justification="34"/>
  <LABEL name="new label" id="9ef5e36462ac4356" memberName="label8" virtualName=""
         explicitFocusOrder="0" pos="0 216 60 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Mix" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15.0"
         kerning="0.0" bold="0" italic="0" justification="34"/>
  <SLIDER name="chorus" id="a14f83ad8f334f76" memberName="chorus" virtualName=""
          explicitFocusOrder="0" pos="216 414 60 60" tooltip="Number of chorus channels"
          min="0.0" max="4.0" int="1.0" style="Rotary" textBoxPos="NoTextBox"
          textBoxEditable="1" textBoxWidth="80" textBoxHeight="20" skewFactor="1.0"
          needsCallback="1"/>
  <LABEL name="new label" id="172be6e07b0f48e5" memberName="label4" virtualName=""
         explicitFocusOrder="0" pos="216 471 56 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Chorus" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15.0"
         kerning="0.0" bold="0" italic="0" justification="36"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]

