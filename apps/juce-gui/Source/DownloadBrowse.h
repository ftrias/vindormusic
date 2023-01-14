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

#pragma once

//[Headers]     -- You can add your own extra header files here --
#include "../JuceLibraryCode/JuceHeader.h"

class DownloadModel : public TableListBoxModel
{
    StringArray labels;
    StringArray desc;
    StringArray data;

public:
    virtual int getNumRows() override
    {
        return labels.size();
    }

    void clear()
    {
        labels.clear();
        desc.clear();
        data.clear();
    }

    void addRow(String label, String value, String info)
    {
        labels.add(label);
        desc.add(value);
        data.add(info);
    }

    String getRow(int row)
    {
        return data[row];
    }

    virtual void selectedRowsChanged(int lastRowSelected) override
    {
        DBG("sel!");
    }

    virtual void paintCell(Graphics & g    ,
                           int     rowNumber,
                           int     columnId,
                           int     width,
                           int     height,
                           bool     rowIsSelected
                           ) override
    {
        String sx;
        if (columnId==1) {
            sx = labels[rowNumber];
        }
        else{
            sx = desc[rowNumber];
        }
        g.setColour(Colours::white);
        g.drawText(sx, 5, 0, width, height, Justification::centredLeft, true);
    }

    void paintRowBackground    (    Graphics & g    ,
                                int     rowNumber,
                                int     width,
                                int     height,
                                bool     rowIsSelected
                                ) override
    {
        if (rowIsSelected) {
            g.setColour(Colours::grey);
            g.fillAll();
        }
    }
};
//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Projucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class DownloadBrowse  : public Component,
                        public Button::Listener
{
public:
    //==============================================================================
    DownloadBrowse ();
    ~DownloadBrowse() override;

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
    void updateData();
    bool processStream(InputStream *in);
    void updateFirmware(const char *purl);
    bool addJson(String json);
    int loadFileDialog();
    //[/UserMethods]

    void paint (Graphics& g) override;
    void resized() override;
    void buttonClicked (Button* buttonThatWasClicked) override;



private:
    //[UserVariables]   -- You can add your own custom variables in this section.
    DownloadModel model;
    std::unique_ptr<WebInputStream> webStream;
    //[/UserVariables]

    //==============================================================================
    std::unique_ptr<TableListBox> choose;
    std::unique_ptr<TextButton> ok;
    std::unique_ptr<TextButton> cancel;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DownloadBrowse)
};

//[EndFile] You can add extra defines here...
//[/EndFile]

