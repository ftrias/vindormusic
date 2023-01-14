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

#pragma once

//[Headers]     -- You can add your own extra header files here --
#include "../JuceLibraryCode/JuceHeader.h"

class FilesModel : public TableListBoxModel
{
    StringArray paths;
    Array<int> bank;
    Array<int> number;

public:
    virtual int getNumRows() override
    {
        return paths.size();
    }

    void clear()
    {
        paths.clear();
    }

    void addRow(String path, int b, int n)
    {
        paths.add(path);
        bank.add(b);
        number.add(n);
    }

    void delRow(int row)
    {
        paths.remove(row);
        bank.remove(row);
        number.remove(row);
    }

    String getRow(int row)
    {
        return paths[row];
    }

    int getRowBank(int row)
    {
        return bank[row];
    }

    int getRowNumber(int row)
    {
        return number[row];
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
            sx = paths[rowNumber];
        }
        else{
            sx = paths[rowNumber];
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

class SFZFile {

};

class PlayCallback : public AudioIODeviceCallback {
private:
    AudioDeviceManager deviceManager;
    void *sf;
    void *parent;
public:
    int state = 0;
    void start(void *in_sf, void *in_parent);
    void test();
    void play(const char *fn, int bank, int inst, int note);
    void stop();
    void audioDeviceIOCallback    (   const float **     inputChannelData,
                                           int     numInputChannels,
                                           float **     outputChannelData,
                                           int     numOutputChannels,
                                           int     numSamples
                                   );
    void audioDeviceAboutToStart (AudioIODevice *device);
    void audioDeviceStopped ();
};
//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Projucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class ConvertFiles  : public Component,
                      public juce::Button::Listener
{
public:
    //==============================================================================
    ConvertFiles ();
    ~ConvertFiles() override;

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
    int addFiles();
    int chooseOutput();
    int convertFiles();
    int convertAllFiles(ThreadWithProgressWindow *progress = NULL);
    int convertFilesForRow(int row, String outPath, ThreadWithProgressWindow *progress, double perc0, double perc1);
    int convertFilesForNote(String outPath, int inst, int midinote);
    //[/UserMethods]

    void paint (juce::Graphics& g) override;
    void resized() override;
    void buttonClicked (juce::Button* buttonThatWasClicked) override;



private:
    //[UserVariables]   -- You can add your own custom variables in this section.
    FilesModel model;
    FilesModel model_chosen;
    String sfpath;
    PlayCallback player;
    void *sf;
    //[/UserVariables]

    //==============================================================================
    std::unique_ptr<TableListBox> sf2list;
    std::unique_ptr<juce::TextButton> addFilesButton;
    std::unique_ptr<juce::Label> label;
    std::unique_ptr<juce::TextEditor> outputDir;
    std::unique_ptr<juce::TextButton> outputFilesButton;
    std::unique_ptr<juce::Label> label3;
    std::unique_ptr<juce::TextButton> startConversion;
    std::unique_ptr<juce::TextButton> playButton;
    std::unique_ptr<TableListBox> sf2chosen;
    std::unique_ptr<juce::TextButton> addButton;
    std::unique_ptr<juce::TextButton> removeButton;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ConvertFiles)
};

//[EndFile] You can add extra defines here...
//[/EndFile]

