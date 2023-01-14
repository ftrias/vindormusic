/*
  ==============================================================================

    MidiControl.cpp
    Created: 31 Mar 2018 7:33:35pm
    Author:  Fernando Trias

  ==============================================================================
*/

#include "MidiControl.h"
#include <string>
#include <vector>

MidiControl::MidiControl()
{
//    deviceManager.addChangeListener(this);
}

MidiControl::~MidiControl()
{
}

void MidiControl::begin()
{
    if (! isTimerRunning()) {
        startTimer(500);
    }
}

int MidiControl::sendMessage(unsigned char *data, int numbytes)
{
    if (midiOutput==NULL) return -1;
    MidiMessage out(data, numbytes);
    midiOutput->sendMessageNow(out);
    return 0;
}

int MidiControl::sendMessage(std::vector<unsigned char> &msg)
{
    if (midiOutput==NULL) return -1;
    const unsigned char *data = msg.data();
    int sz = (int)msg.size();
    MidiMessage out(data, sz);
    midiOutput->sendMessageNow(out);
    return 0;
}

/*
void MidiControl::onConnect()
{
}
*/

void MidiControl::disconnect() {
    if (state > 0) {
        deviceManager.setMidiInputDeviceEnabled(deviceName, false);
        deviceManager.removeMidiInputDeviceCallback(deviceName, this);
        if (midiOutput) {
            midiOutput = nullptr;
        }
        if (midiInput) {
            midiInput = nullptr;
        }
        onDisconnect();
    }
    state = 0;
}

int MidiControl::openVindor()
{
	int stage = 0;
    inlist = MidiInput::getAvailableDevices();
    for (int i = 0; i < inlist.size(); ++i)
    {
        if (inlist[i].name.startsWith("Vindor")) {
            deviceName = inlist[i].name;
            deviceManager.setMidiInputDeviceEnabled(inlist[i].identifier, true);
            deviceManager.addMidiInputDeviceCallback(inlist[i].identifier, this);
			stage++;
            break;
        }
    }
    if (stage==0) return 0;
    
	outlist = MidiOutput::getAvailableDevices();
	for (int i = 0; i < outlist.size(); ++i)
	{
		if (outlist[i].name.startsWith("Vindor")) {
			deviceName = outlist[i].name;
			midiOutput = MidiOutput::openDevice(outlist[i].identifier);
			stage++;
            break;
		}
	}
    
	if (stage==2) {
		onConnect();
		return 1;
	}
    
    return 0;
}

int MidiControl::closeVindor() {
	deviceManager.setMidiInputDeviceEnabled(deviceName, false);
	deviceManager.removeMidiInputDeviceCallback(deviceName, this);
	midiOutput.reset(NULL);
	deviceManager.closeAudioDevice();
	return 0;
}

int MidiControl::checkVindor() {
    inlist = MidiInput::getAvailableDevices();
    for (int i = 0; i < inlist.size(); ++i)
    {
        if (inlist[i].name.startsWith("Vindor")) {
            return 1;
        }
    }
    return 0;
}

const char *hex = "01234567890ABCDEF";

//void MidiControl::dataCallback(double timeStamp, std::vector<unsigned char> *message) { }

void MidiControl::handleIncomingMidiMessage (MidiInput* source, const MidiMessage& message)
{
    char str[256];
    const unsigned char *data = message.getRawData();
    for (int i=0, j=0; i<message.getRawDataSize(); i++) {
        str[j] = hex[data[i] >> 4];
        str[j+1] = hex[data[i] & 0x0F];
        j+=2;
    }
    debug.push_back(str);
    int sz = message.getRawDataSize();
    auto s = new std::vector<unsigned char>(data, data+sz);
    dataCallback(0, s);
}

void MidiControl::timerCallback()
{
    if (busy) return;
    busy = 1;
    if (state == 0) {
        int ret = openVindor();
        if (ret) {
            state = 1;
        }
    }
    else {
        int v = checkVindor();
        if (! v) {
            state = 0;
			closeVindor();
            onDisconnect();
        }
    }
    busy = 0;
}
