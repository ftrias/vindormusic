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
#define TSF_IMPLEMENTATION
#include "tsf.h"
//[/Headers]

#include "ConvertFiles.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...
void PlayCallback::start(void *in_sf, void *in_parent) {
    state = 0;
    sf = in_sf;
    parent = in_parent;
    deviceManager.initialise(0, 1, NULL, true);
    deviceManager.addAudioCallback(this);
}

void PlayCallback::play(const char *fn, int bank, int inst, int note) {
    if (state) {
        stop();
        return;
    }
    if (! sf) return;
    tsf_set_output((tsf*)sf, TSF_MONO, 44100, 0); //sample rate
    tsf_note_on((tsf*)sf, inst, note, 1.0f); //preset 0, middle C
    // tsf_bank_note_on((tsf*)sf, bank, inst, note, 1.0f); //preset 0, middle C
    state = 1;
}

void PlayCallback::stop() {
    tsf_note_off_all((tsf*)sf);
}

void PlayCallback::audioDeviceIOCallback    (   const float **     inputChannelData,
                               int     numInputChannels,
                               float **     outputChannelData,
                               int     numOutputChannels,
                               int     numSamples
                               )
{
    if (state && tsf_active_voice_count((tsf*)sf) == 0) {
        state = 0;
        return;
    }

    for(int c=0; c<numOutputChannels; c++) {
        if (state) {
            tsf_render_float((tsf*)sf, outputChannelData[c], numSamples, 0);
        }
        else {
            for(int i=0; i<numSamples; i++) {
                outputChannelData[c][i] = 0.0;
            }
        }
    }
}

void     PlayCallback::audioDeviceAboutToStart (AudioIODevice *device)
{

}

void     PlayCallback::audioDeviceStopped ()
{

}
//[/MiscUserDefs]

//==============================================================================
ConvertFiles::ConvertFiles ()
{
    //[Constructor_pre] You can add your own custom stuff here..
    //[/Constructor_pre]

    sf2list.reset (new TableListBox());
    addAndMakeVisible (sf2list.get());
    sf2list->setName ("new component");

    addFilesButton.reset (new juce::TextButton ("new button"));
    addAndMakeVisible (addFilesButton.get());
    addFilesButton->setButtonText (TRANS("Load SoundFont File"));
    addFilesButton->addListener (this);

    addFilesButton->setBounds (8, 48, 176, 24);

    label.reset (new juce::Label ("new label",
                                  TRANS("This wizard will help you convert SoundFont instruments into SD card.")));
    addAndMakeVisible (label.get());
    label->setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    label->setJustificationType (juce::Justification::centredLeft);
    label->setEditable (false, false, false);
    label->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    label->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    outputDir.reset (new juce::TextEditor ("new text editor"));
    addAndMakeVisible (outputDir.get());
    outputDir->setMultiLine (false);
    outputDir->setReturnKeyStartsNewLine (false);
    outputDir->setReadOnly (false);
    outputDir->setScrollbarsShown (true);
    outputDir->setCaretVisible (true);
    outputDir->setPopupMenuEnabled (true);
    outputDir->setText (juce::String());

    outputFilesButton.reset (new juce::TextButton ("new button"));
    addAndMakeVisible (outputFilesButton.get());
    outputFilesButton->setButtonText (TRANS("Browse..."));
    outputFilesButton->addListener (this);

    label3.reset (new juce::Label ("new label",
                                   TRANS("Directory/Folder of SD card")));
    addAndMakeVisible (label3.get());
    label3->setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    label3->setJustificationType (juce::Justification::centredLeft);
    label3->setEditable (false, false, false);
    label3->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    label3->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    startConversion.reset (new juce::TextButton ("new button"));
    addAndMakeVisible (startConversion.get());
    startConversion->setButtonText (TRANS("Start Conversion"));
    startConversion->addListener (this);

    playButton.reset (new juce::TextButton ("new button"));
    addAndMakeVisible (playButton.get());
    playButton->setButtonText (TRANS("Play test sound"));
    playButton->addListener (this);

    sf2chosen.reset (new TableListBox());
    addAndMakeVisible (sf2chosen.get());
    sf2chosen->setName ("new component");

    addButton.reset (new juce::TextButton ("new button"));
    addAndMakeVisible (addButton.get());
    addButton->setButtonText (TRANS("Add>"));
    addButton->addListener (this);

    removeButton.reset (new juce::TextButton ("new button"));
    addAndMakeVisible (removeButton.get());
    removeButton->setButtonText (TRANS("<Remove"));
    removeButton->addListener (this);


    //[UserPreSize]
    sf2list->getHeader().addColumn("Instrument", 1, 240);
//    sf2list->getHeader().setStretchToFitActive(true);
    sf2list->setModel(&model);
    sf2chosen->getHeader().addColumn("Convert", 1, 100);
    sf2chosen->setModel(&model_chosen);
    //[/UserPreSize]

    setSize (600, 400);


    //[Constructor] You can add your own custom stuff here..
    model.clear();
    sf2list->updateContent();
    sf2list->autoSizeAllColumns();
    sf = NULL;
    //[/Constructor]
}

ConvertFiles::~ConvertFiles()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

    sf2list = nullptr;
    addFilesButton = nullptr;
    label = nullptr;
    outputDir = nullptr;
    outputFilesButton = nullptr;
    label3 = nullptr;
    startConversion = nullptr;
    playButton = nullptr;
    sf2chosen = nullptr;
    addButton = nullptr;
    removeButton = nullptr;


    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void ConvertFiles::paint (juce::Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (juce::Colour (0xff323e44));

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void ConvertFiles::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    sf2list->setBounds (8, 88, getWidth() - 271, getHeight() - 226);
    label->setBounds (0, 0, proportionOfWidth (0.9890f), 40);
    outputDir->setBounds (8, getHeight() - 74, getWidth() - 95, 24);
    outputFilesButton->setBounds (getWidth() - 79, getHeight() - 74, 71, 24);
    label3->setBounds (8, getHeight() - 98, 480, 24);
    startConversion->setBounds ((getWidth() / 2) - (176 / 2), getHeight() - 34, 176, 24);
    playButton->setBounds (8, getHeight() - 130, 176, 24);
    sf2chosen->setBounds (getWidth() - 151, 88, 136, getHeight() - 194);
    addButton->setBounds (getWidth() - 242, 175, 71, 24);
    removeButton->setBounds (getWidth() - 242, 215, 71, 24);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void ConvertFiles::buttonClicked (juce::Button* buttonThatWasClicked)
{
    //[UserbuttonClicked_Pre]
#if JUCE_MAC || JUCE_WINDOWS || JUCE_LINUX
    //[/UserbuttonClicked_Pre]

    if (buttonThatWasClicked == addFilesButton.get())
    {
        //[UserButtonCode_addFilesButton] -- add your button handler code here..
        addFiles();
        //[/UserButtonCode_addFilesButton]
    }
    else if (buttonThatWasClicked == outputFilesButton.get())
    {
        //[UserButtonCode_outputFilesButton] -- add your button handler code here..
        chooseOutput();
        //[/UserButtonCode_outputFilesButton]
    }
    else if (buttonThatWasClicked == startConversion.get())
    {
        //[UserButtonCode_startConversion] -- add your button handler code here..
        convertFiles();
        //[/UserButtonCode_startConversion]
    }
    else if (buttonThatWasClicked == playButton.get())
    {
        //[UserButtonCode_playButton] -- add your button handler code here..
        //        deviceManager.addAudioCallback(AudioIODeviceCallback *newCallback);
        if (player.state) {
            player.stop();
            playButton->setButtonText("Play");
        }
        else {
            int row = sf2list->getSelectedRow();
            if (row < 0) {
                AlertWindow::showMessageBox(AlertWindow::AlertIconType::WarningIcon, "Error",
                                            "Please select one instrument.");
                return;
            }
            int bank = model.getRowBank(row);
            int inst = model.getRowNumber(row);
            const char *fn = sfpath.getCharPointer();
            player.play(fn, bank, inst, 60);
            playButton->setButtonText("Stop");
        }

        //[/UserButtonCode_playButton]
    }
    else if (buttonThatWasClicked == addButton.get())
    {
        //[UserButtonCode_addButton] -- add your button handler code here..
        if (sf2chosen->getNumRows() >= 9) {
            AlertWindow::showMessageBox(AlertWindow::AlertIconType::WarningIcon, "Error",
                                        "Can only choose up to 9 instruments.");
            return;
        }
        int row = sf2list->getSelectedRow();
        if (row < 0) {
            AlertWindow::showMessageBox(AlertWindow::AlertIconType::WarningIcon, "Error",
                                        "Please select one instrument.");
            return;
        }
        model_chosen.addRow(model.getRow(row), model.getRowBank(row), model.getRowNumber(row));
        sf2chosen->updateContent();
        //[/UserButtonCode_addButton]
    }
    else if (buttonThatWasClicked == removeButton.get())
    {
        //[UserButtonCode_removeButton] -- add your button handler code here..
        int row = sf2chosen->getSelectedRow();
        if (row < 0) {
            AlertWindow::showMessageBox(AlertWindow::AlertIconType::WarningIcon, "Error",
                                        "Please select one instrument.");
            return;
        }
        model_chosen.delRow(row);
        sf2chosen->updateContent();
        //[/UserButtonCode_removeButton]
    }

    //[UserbuttonClicked_Post]
#endif
    //[/UserbuttonClicked_Post]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...
int ConvertFiles::addFiles() {
#if 0
    AudioPluginFormatManager manager;
    manager.addDefaultFormats();
    int c = manager.getNumFormats();
    for(int i=0; i < c; i++) {
        AudioPluginFormat *format = manager.getFormat(i);
        String name = format->getName();
        model.addRow(name, 0, i);
    }
    sf2list->updateContent();
    return 0;
#endif

#if JUCE_MAC || JUCE_WINDOWS || JUCE_LINUX

    FileChooser dialogBox ("SF2 file",
                           File::getSpecialLocation (File::userHomeDirectory),
                           "*.sf2");

    if (dialogBox.browseForFileToOpen())
    {
        if (sf) {
            tsf_close((tsf*)sf);
            sf = NULL;
        }

        File selectedFile (dialogBox.getResult());
        sfpath = selectedFile.getFullPathName();
        const char *fn = sfpath.getCharPointer();
        sf = tsf_load_filename(fn);

        if (sf == NULL) {
            AlertWindow::showMessageBox(AlertWindow::AlertIconType::WarningIcon, "Error",
                                        "File corrupted or format not supported.");
            return 0;
        }

        player.start(sf, this);
        int count = tsf_get_presetcount((tsf*)sf);

        for (int i=0; i<count; i++) {
            const char *s = tsf_get_presetname((tsf*)sf, i);
            model.addRow(s, 0, i);
        }
        sf2list->updateContent();
    }
#endif
    return 0;
}

int ConvertFiles::chooseOutput() {
#if JUCE_MAC || JUCE_WINDOWS || JUCE_LINUX
    FileChooser dialogBox ("Output Directory",
                           File::getSpecialLocation (File::userHomeDirectory));

    if (dialogBox.browseForDirectory()) {
        File selectedFile (dialogBox.getResult());
        outputDir->setText(selectedFile.getFullPathName());
    }
#endif
    return 0;
}

class ConvertFilesThread : public ThreadWithProgressWindow {
public:
    ConvertFiles *source;
    ConvertFilesThread(ConvertFiles *me) : ThreadWithProgressWindow("Convert SoundFont", true, false)
    {
        source = me;
    }
    void run() {
        source->convertAllFiles(this);
    }
};

int ConvertFiles::convertFiles() {
#if JUCE_MAC || JUCE_WINDOWS || JUCE_LINUX
    ConvertFilesThread thread(this);
    thread.runThread();
	// convertAllFiles();
#endif
    return 0;
}

int ConvertFiles::convertAllFiles(ThreadWithProgressWindow *progress) {
#if JUCE_MAC || JUCE_WINDOWS || JUCE_LINUX
    int numrow = sf2chosen->getNumRows();
    if (numrow <= 0) {
        AlertWindow::showMessageBox(AlertWindow::AlertIconType::WarningIcon, "Error",
                                    "Please select one instrument.");
        return 0;
    }

    String outDir(outputDir->getText());
    if (outDir == "") {
        AlertWindow::showMessageBox(AlertWindow::AlertIconType::WarningIcon, "Error",
                                    "Please select an output directory.");
        return 0;
    }

#if JUCE_MAC || JUCE_LINUX
    String dirSep = "/";
#else
    String dirSep = "\\";
#endif

    if (outDir[outDir.length()-1] != dirSep[0]) {
        outDir.append(dirSep, 1);
    }

//    double perc = 0.0;
//    ProgressBar progress(perc);
//    perc = (note - 24) / (110 - 24);

    for(int row=0; row<numrow; row++) {
		double perc1 = 1.0 / numrow;
		double perc0 = row * perc1;
		if (progress) {
			progress->setProgress(perc0);
		}
        String outPath = outDir;
        outPath << "vindor" << dirSep << "s" << row << dirSep;
        File dir(outPath);
        dir.createDirectory();
        convertFilesForRow(row, outPath, progress, perc0, perc1);
    }
#endif

    return 0;
}

class NoteConvertThread : public ThreadPoolJob {
public:
    void *sf;
    String outPath;
    int inst;
    int note;
    NoteConvertThread(void *soundfont, String path, int instrument, int midinote) : ThreadPoolJob("job") {
        sf = soundfont;
        outPath = path;
        inst = instrument;
        note = midinote;
    }
    JobStatus runJob() {
        const int blen = 1024;
        short buffer[blen];
        const int sustain_blocks = 44100 * 10 / blen; // 10 seconds of sustain

        String rawPath(outPath);
        rawPath += String::formatted("play%d.raw", note);
        File raw(rawPath);

        tsf_set_output((tsf*)sf, TSF_MONO, 44100, 0); //sample rate
        tsf_note_on((tsf*)sf, inst, note, 1.0f); //preset 0, middle C
        //        tsf_bank_note_on((tsf*)sf, bank, inst, note, 1.0f); //preset 0, middle C
        for(int i=0; i<sustain_blocks; i++) {
            tsf_render_short((tsf*)sf, buffer, blen, 0);
            raw.appendData(buffer, blen * 2);
            if (shouldExit()) return jobNeedsRunningAgain;
        }
        tsf_note_off_all((tsf*)sf);
        while(tsf_active_voice_count((tsf*)sf) > 0) {
            tsf_render_short((tsf*)sf, buffer, blen, 0);
            raw.appendData(buffer, blen * 2);
            if (shouldExit()) return jobNeedsRunningAgain;
        }
        return jobHasFinished;
    }
};

int ConvertFiles::convertFilesForNote(String outPath, int inst, int note) {
    const int blen = 1024;
    short buffer[blen];
    const int sustain_blocks = 44100 * 10 / blen; // 10 seconds of sustain

    String rawPath(outPath);
    rawPath += String::formatted("play%d.raw", note);
    File raw(rawPath);

    tsf_set_output((tsf*)sf, TSF_MONO, 44100, 0); //sample rate
    tsf_note_on((tsf*)sf, inst, note, 1.0f); //preset 0, middle C
    //        tsf_bank_note_on((tsf*)sf, bank, inst, note, 1.0f); //preset 0, middle C
    for(int i=0; i<sustain_blocks; i++) {
        tsf_render_short((tsf*)sf, buffer, blen, 0);
        raw.appendData(buffer, blen * 2);
    }
    tsf_note_off_all((tsf*)sf);
    while(tsf_active_voice_count((tsf*)sf) > 0) {
        tsf_render_short((tsf*)sf, buffer, blen, 0);
        raw.appendData(buffer, blen * 2);
    }
    return 0;
}

int ConvertFiles::convertFilesForRow(int row, String outPath,
	ThreadWithProgressWindow *progress, double perc0, double perc1) {
    String name = model_chosen.getRow(row);
//    int bank = model_chosen(row);
    int inst = model_chosen.getRowNumber(row);

    String infPath(outPath);
    infPath += "play.inf";
    File inf(infPath);
    inf.appendText(String::formatted("disp=%s\n", (const char *)name.getCharPointer()));
    inf.appendText("format=raw\n");

    ThreadPool pool;

    for(int note=24; note<110; note++) {
		if (progress) {
			double perc = perc0 + perc1 * (note - 24.0) / (110.0 - 24.0);
			progress->setProgress(perc);
		}
        inf.appendText(String::formatted("%d,0,0\n", note));

        NoteConvertThread *job = new NoteConvertThread(sf, outPath, inst, note);
        pool.addJob(job, true);

        while(pool.getNumJobs() >= 10) {
            Time::waitForMillisecondCounter(Time::getMillisecondCounter() + 1000);
        }
    }

    while(pool.getNumJobs()) {
        Time::waitForMillisecondCounter(Time::getMillisecondCounter() + 1000);
    }

    return 0;
}
//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Projucer information section --

    This is where the Projucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="ConvertFiles" componentName=""
                 parentClasses="public Component" constructorParams="" variableInitialisers=""
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="0" initialWidth="600" initialHeight="400">
  <BACKGROUND backgroundColour="ff323e44"/>
  <GENERICCOMPONENT name="new component" id="7ce3cfaa664c1809" memberName="sf2list"
                    virtualName="" explicitFocusOrder="0" pos="8 88 271M 226M" class="TableListBox"
                    params=""/>
  <TEXTBUTTON name="new button" id="ea4d3e951fca0a4d" memberName="addFilesButton"
              virtualName="" explicitFocusOrder="0" pos="8 48 176 24" buttonText="Load SoundFont File"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <LABEL name="new label" id="1f8e44f053fc7700" memberName="label" virtualName=""
         explicitFocusOrder="0" pos="0 0 98.898% 40" edTextCol="ff000000"
         edBkgCol="0" labelText="This wizard will help you convert SoundFont instruments into SD card."
         editableSingleClick="0" editableDoubleClick="0" focusDiscardsChanges="0"
         fontname="Default font" fontsize="15.0" kerning="0.0" bold="0"
         italic="0" justification="33"/>
  <TEXTEDITOR name="new text editor" id="ce77c5e6f76c5126" memberName="outputDir"
              virtualName="" explicitFocusOrder="0" pos="8 74R 95M 24" initialText=""
              multiline="0" retKeyStartsLine="0" readonly="0" scrollbars="1"
              caret="1" popupmenu="1"/>
  <TEXTBUTTON name="new button" id="3ea51d87af3b2104" memberName="outputFilesButton"
              virtualName="" explicitFocusOrder="0" pos="79R 74R 71 24" buttonText="Browse..."
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <LABEL name="new label" id="ac42c3abb7cea65e" memberName="label3" virtualName=""
         explicitFocusOrder="0" pos="8 98R 480 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Directory/Folder of SD card" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="15.0" kerning="0.0" bold="0" italic="0" justification="33"/>
  <TEXTBUTTON name="new button" id="ec16bb10f5fcdd09" memberName="startConversion"
              virtualName="" explicitFocusOrder="0" pos="0Cc 34R 176 24" buttonText="Start Conversion"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="new button" id="129d69f2e91ec604" memberName="playButton"
              virtualName="" explicitFocusOrder="0" pos="8 130R 176 24" buttonText="Play test sound"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <GENERICCOMPONENT name="new component" id="7532bcee6047d077" memberName="sf2chosen"
                    virtualName="" explicitFocusOrder="0" pos="151R 88 136 194M"
                    class="TableListBox" params=""/>
  <TEXTBUTTON name="new button" id="d79e5451bc99f38e" memberName="addButton"
              virtualName="" explicitFocusOrder="0" pos="242R 175 71 24" buttonText="Add&gt;"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="new button" id="bffb0406d4af8a16" memberName="removeButton"
              virtualName="" explicitFocusOrder="0" pos="242R 215 71 24" buttonText="&lt;Remove"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]

