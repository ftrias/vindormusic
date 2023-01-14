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

#include "InfoTab.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...
//[/MiscUserDefs]

//==============================================================================
InfoTab::InfoTab ()
{
    //[Constructor_pre] You can add your own custom stuff here..
    //[/Constructor_pre]


    //[UserPreSize]
    list.reset(new TableListBox("Listbox1"));
    addAndMakeVisible (list.get());
    list->getHeader().addColumn("Name", 1, 20);
    list->getHeader().addColumn("Value", 2, 40);
    list->getHeader().setStretchToFitActive(true);
    list->setModel(&model);
    list->setBoundsRelative(0.0, 0.0, 1.0, 1.0);
    //[/UserPreSize]

    setSize (600, 400);


    //[Constructor] You can add your own custom stuff here..
    list->setBoundsRelative(0.0, 0.0, 1.0, 1.0);
    deviceControl->addChangeListener(this);
    //[/Constructor]
}

InfoTab::~InfoTab()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]



    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void InfoTab::paint (juce::Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void InfoTab::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    //[UserResized] Add your own custom resize handling here..
    list->setBounds (0, 0, proportionOfWidth (1.0000f), proportionOfWidth (1.0000f));
    //[/UserResized]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...
void InfoTab::changeListenerCallback (ChangeBroadcaster *source) {
    if (! deviceControl->isValid()) {
        model.clear();
        list->updateContent();
        return;
    }

    String sx;
    model.clear();
    model.addRow("Manufacturer", deviceControl->settings.Name);
    model.addRow("Model", deviceControl->settings.Desc);
    model.addRow("Motherboard", deviceControl->settings.Revision);
    model.addRow("Firmware", deviceControl->settings.Firmware);
    model.addRow("Serial", deviceControl->settings.CPUguid);
    model.addRow("CPU", deviceControl->settings.CPUmodel);
    model.addRow("Compiled", deviceControl->settings.CompileTime);
    // debug info below
//    model.addRow("USB", deviceControl->settings.UsbMode);

#ifdef DEBUGX
    for (auto const& x : deviceControl->settings.Info)
    {
        model.addRow(x.first.c_str(), x.second);
    }
    for (auto const& x :  deviceControl->settings.Setting)
    {
        model.addRow(x.first.c_str(), x.second);
    }
#endif

    list->updateContent();

    if (deviceControl->needUpdate()) {
        AlertWindow::showMessageBoxAsync(AlertWindow::AlertIconType::WarningIcon, "Firmware Update Required", "Your Vindor is running an older firmware and this App is not compatible. Some features may not work until you update your firmware. Please visit vindormusic.com for updates.");
    }

//    repaint();
}
//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Projucer information section --

    This is where the Projucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="InfoTab" componentName=""
                 parentClasses="public Component, public ChangeListener" constructorParams=""
                 variableInitialisers="" snapPixels="8" snapActive="1" snapShown="1"
                 overlayOpacity="0.330" fixedSize="0" initialWidth="600" initialHeight="400">
  <BACKGROUND backgroundColour="323e44"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]

