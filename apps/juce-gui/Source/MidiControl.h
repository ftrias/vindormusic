/*
  ==============================================================================

    MidiControl.h
    Created: 31 Mar 2018 7:33:35pm
    Author:  Fernando Trias

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include <string>

class MidiControl : public MidiInputCallback,
                    // public ChangeListener,
                    public Timer
{
    public:
    MidiControl();
    ~MidiControl();
    
    AudioDeviceManager deviceManager;
    String deviceName;
    Array<MidiDeviceInfo> inlist;
    Array<MidiDeviceInfo> outlist;
                        
    void begin();
    void disconnect();
    int sendMessage(std::vector<unsigned char> &msg);
    int sendMessage(unsigned char *data, int numbytes);
    
    int state = 0;
    int busy = 0;
    
    std::vector<std::string> devices;
    std::vector<std::string> debug;

    std::unique_ptr<MidiInput> midiInput;
    std::unique_ptr<MidiOutput> midiOutput;
    
    int openVindor();
	int closeVindor();
    int checkVindor();
    void handleIncomingMidiMessage (MidiInput* source, const MidiMessage& message);
    void changeListenerCallback (ChangeBroadcaster *source);
                        
    void timerCallback();
    
    virtual void onConnect() = 0;
    virtual void onDisconnect() = 0;
    
    virtual void dataCallback(double timeStamp, std::vector<unsigned char> *message) = 0;
};
