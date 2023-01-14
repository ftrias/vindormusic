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
//[/Headers]

#include "DownloadBrowse.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...
//[/MiscUserDefs]

//==============================================================================
DownloadBrowse::DownloadBrowse ()
{
    //[Constructor_pre] You can add your own custom stuff here..
    //[/Constructor_pre]

    choose.reset (new TableListBox());
    addAndMakeVisible (choose.get());
    choose->setName ("new component");

    ok.reset (new TextButton ("Ok"));
    addAndMakeVisible (ok.get());
    ok->addListener (this);

    cancel.reset (new TextButton ("Cancel"));
    addAndMakeVisible (cancel.get());
    cancel->addListener (this);


    //[UserPreSize]
    choose->getHeader().addColumn("Name", 1, 60);
    choose->getHeader().addColumn("Notes", 2, 100);
    choose->getHeader().setStretchToFitActive(true);
    choose->setModel(&model);
    //[/UserPreSize]

    setSize (600, 400);


    //[Constructor] You can add your own custom stuff here..
    updateData();
    //[/Constructor]
}

DownloadBrowse::~DownloadBrowse()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

    choose = nullptr;
    ok = nullptr;
    cancel = nullptr;


    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void DownloadBrowse::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (Colour (0xff323e44));

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void DownloadBrowse::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    choose->setBounds (0, 0, proportionOfWidth (1.0000f), getHeight() - 45);
    ok->setBounds ((getWidth() / 2) + -100, getHeight() - 35, 75, 24);
    cancel->setBounds ((getWidth() / 2) + 25, getHeight() - 35, 75, 24);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void DownloadBrowse::buttonClicked (Button* buttonThatWasClicked)
{
    //[UserbuttonClicked_Pre]
    //[/UserbuttonClicked_Pre]

    if (buttonThatWasClicked == ok.get())
    {
        //[UserButtonCode_ok] -- add your button handler code here..
        auto row = choose->getSelectedRow();
        int retval = 0;
        if (row >= 0) {
            String json = model.getRow(row);
            if (json == "file") {
                retval = loadFileDialog();
            }
            else if (json.startsWith("code:")) {
                String num = json.substring(5, 99);
                int x = num.getIntValue();
                deviceControl->settings.setInt("KEYLAYOUT", x); // set new fingering
                deviceControl->sendSettingName("KEYLAYOUT");
            }
            else {
                var data;
                Result res = JSON::parse(json, data);
                bool isok = deviceControl->updateFingeringData(data);
                if (isok) retval = 1;
            }
        }
        if (DialogWindow* dw = this->findParentComponentOfClass<DialogWindow>())
            dw->exitModalState (retval);
        //[/UserButtonCode_ok]
    }
    else if (buttonThatWasClicked == cancel.get())
    {
        //[UserButtonCode_cancel] -- add your button handler code here..
        if (DialogWindow* dw = this->findParentComponentOfClass<DialogWindow>())
            dw->exitModalState (0);
        //[/UserButtonCode_cancel]
    }

    //[UserbuttonClicked_Post]
    //[/UserbuttonClicked_Post]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...
#if JUCE_MAC || JUCE_WINDOWS || JUCE_LINUX

int DownloadBrowse::loadFileDialog()
{
    FileChooser dialogBox ("Please choose a file for update...",
                           File::getSpecialLocation (File::userHomeDirectory),
                           "*.vnd");

    if (dialogBox.browseForFileToOpen())
    {
        File selectedFile (dialogBox.getResult());
        var data = JSON::parse(selectedFile);
        var dtype = data.getProperty("type", "none");

        if (dtype.toString() != "fingering") {
            AlertWindow::showMessageBox(AlertWindow::AlertIconType::WarningIcon, "Error",
                                        "File does not contain fingering data.");
            return 0;
        }

        bool isok = deviceControl->updateFingeringData(data);
        if (isok) return 1;
        return 0;
    }
    return 0;
}

#else

int DownloadBrowse::loadFileDialog() {
    return 0;
}

#endif

bool DownloadBrowse::addJson(String json)
{
    var jsonReply;
    Result result = JSON::parse(json, jsonReply);

    if (auto kbArray = jsonReply.getProperty("kb", var()).getArray()) {
        for (auto& kb : *kbArray) {
            String title = kb.getProperty("name", String());
            String desc = kb.getProperty("desc", String("-"));
            if (kb.hasProperty("code")) {
                String code = kb.getProperty("code", String("0"));
                String info = "code:" + code;
                model.addRow(title, desc, info);
            }
            else {
                String info = JSON::toString(kb);
                model.addRow(title, desc, info);
            }
        }
    }
    return true;
}

bool DownloadBrowse::processStream(InputStream *in)
{
    StringArray data;

    int active = 0;
    while(! in->isExhausted()) {
        auto line = in->readNextLine();
        line = line.trim();
//        DBG(line);
        if (line.endsWith("<pre>")) {
            active = 1;
            continue;
        }
        if (line.startsWith("</pre>")) {
            break;
        }

        if (! active) continue;

//        DBG(line);

        data.add(line);
    }


    String json = data.joinIntoString("\n");
    addJson(json);

    return true;
}

void DownloadBrowse::updateData () {
    model.clear();

    addJson(BinaryData::keys_json);

    URL url("https://vindormusic.com/kbfiles");
    URL::InputStreamOptions options(URL::ParameterHandling::inAddress);
    auto in = url.createInputStream(options);
    if (in) {
        processStream(in.get());
    }

#if JUCE_MAC || JUCE_WINDOWS || JUCE_LINUX
    model.addRow("File...", "Choose custom file", "file");
#endif

    choose->updateContent();
    choose->autoSizeAllColumns();

//    repaint();
}
//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Projucer information section --

    This is where the Projucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="DownloadBrowse" componentName=""
                 parentClasses="public Component" constructorParams="" variableInitialisers=""
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="0" initialWidth="600" initialHeight="400">
  <BACKGROUND backgroundColour="ff323e44"/>
  <GENERICCOMPONENT name="new component" id="137b3f596c33b1b9" memberName="choose"
                    virtualName="" explicitFocusOrder="0" pos="0 0 100% 45M" class="TableListBox"
                    params=""/>
  <TEXTBUTTON name="Ok" id="61b061e595e776a1" memberName="ok" virtualName=""
              explicitFocusOrder="0" pos="-100C 35R 75 24" buttonText="Ok"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="Cancel" id="47fb2d34a75b1d9f" memberName="cancel" virtualName=""
              explicitFocusOrder="0" pos="25C 35R 75 24" buttonText="Cancel"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]

