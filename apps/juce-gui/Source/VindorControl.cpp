/*
 * Copyright 2013-2018 by Fernando Trias. All rights reserved.
 */
 
#include "VindorWrap.h"
#include "MidiControl.h"

#include <stdlib.h>
#include <string>
#include <sstream>
#include <iostream>

#if 0
#if defined(__WINDOWS_MM__)
#include <windows.h>
#define SLEEP(milliseconds) Sleep((DWORD)milliseconds)
#else // Unix variants
#include <unistd.h>
#define SLEEP(milliseconds) usleep((unsigned long)(milliseconds * 1000.0))
#endif  
#endif // 0


/*************/

int MidiFileHandler::sendMessage(unsigned char *data, int len)
{
    deviceControl->sendMessage(data, len);
    return len;
}
int MidiFileHandler::readMessage(unsigned char *data, int len)
{
    return 0;
}

/*************/

class setCallback;
std::string toHexString(std::vector<unsigned char> &message)
{
    std::string status;
	static char hexDigit[] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
	status.push_back('[');
	for(unsigned int i=0; i<message.size(); i++) {
		//if (i!=0) status.push_back(':');
		if (i>7 && message[i]>='0' && message[i]<='z') {
			status.push_back(message[i]);
		}
		else {
			status.push_back('[');
			status.push_back(hexDigit[message[i] >> 4]);
			status.push_back(hexDigit[message[i] & 0x0F]);
			status.push_back(']');
		}
	}
	status.push_back(']');
	return status;
}

void dumpMessage(MidiSerial *ser)
{
	unsigned char *start = ser->getBufferData();
	std::vector<unsigned char> message(start, start+256);
	DBG("DUMP");
	DBG(toHexString(message).data());
//    printf("DUMP:%s\n", toHexString(message).data());
}

int16_t finger_data[SettingsData::fingerlist_custom_max];

void VindorInfo::clear()
{
    invalidProtocol = 0;

    Name.clear();
    Desc.clear();
    Revision = 0;
    Platform = 0;
    Firmware = 0;
    CPUguid.clear();
    CompileTime.clear();
    CPUmodel.clear();

//    DevSettings.defaultsettings();

    WifiEnabled = 0;
    WifiSSID.clear();
    WifiPASS.clear();
    UsbMode = 0;

    Info.clear();
    Setting.clear();
}

void VindorInfo::setInt(const char *key, int number)
{
	Setting[key] = number;
}

int VindorInfo::getInt(const char *key)
{
	return Setting[key];
}

void VindorInfo::setStr(const char *key, const char *value)
{
    SettingStr[key] = value;
}

std::string VindorInfo::getStr(const char *key)
{
    return SettingStr[key];
}

int VindorInfo::hasSettings()
{
	if (Setting.size()>0) return 1;
	return 0;
}

#include <stdlib.h>

bool VindorInfo::setInfo(MidiSerial *ser)
{
	char str[256];
	Name = ser->getString(str);
	Desc = ser->getString(str);
	Revision = ser->getWord();
    if (Revision==1) { // old beta, must extract from Desc
        Revision = atoi(Desc.substr(Desc.size() - 2, 2).c_str());
    }
	Platform = ser->getWord();
	Firmware = ser->getWord();
    
    if (Firmware < VindorControl::MinimumFirmware) {
        invalidProtocol = 1;
//        deviceControl->triggerDataChange(5);
        return false;
    }
    else {
        invalidProtocol = 0;
    }

    CPUguid = ser->getString(str);
    CompileTime = ser->getString(str);
    CPUmodel = ser->getString(str);
    ser->rev = Revision;
	unsigned char msg = 0x10;
	while(msg != 0x00) {
		msg = ser->getByte();
		if (msg == 0x00) break;
		if (msg == 0x11) {
			ser->getString(str);
			int v = ser->getWord();
			Info[str] = v;
		}
		if (msg == 0x12) {
			ser->getString(str);
			float v = ser->getFloat();
			Info[str] = v*100;
		}
	}
    
    return true;
}

void VindorInfo::setWifi(MidiSerial *ser)
{
    char str[256];
    WifiEnabled = ser->getByte();
    WifiSSID = ser->getString(str);
    WifiPASS = ser->getString(str);
    UsbMode = ser->getByte();
}

void VindorInfo::getWifi(MidiSerial *ser)
{
    ser->addByte(WifiEnabled);
    ser->addString(WifiSSID.data());
    ser->addString(WifiPASS.data());
    ser->addByte(UsbMode);
}

void VindorInfo::revert()
{
    Setting = SettingOrig;
    SettingStr = SettingStrOrig;
}

void VindorInfo::setSettings(MidiSerial *ser)
{
	char key[256];
	int number = ser->getWord();
	for(int i=0;i<number; i++) {
		int datatype = ser->getByte();
		if (datatype==0) break;
        ser->getString(key);
		if (datatype==0x11) { // set integer
			int value = ser->getWord();
			Setting[key] = value;
            SettingOrig[key] = value;
		}
        else if (datatype==0x12) { // set float
            float value = ser->getFloat() * 10.0;
            Setting[key] = value;
            SettingOrig[key] = value;
        }
        else if (datatype==0x13) { // set string
            char str[255];
            ser->getString(str);
            SettingStr[key] = str;
            SettingStrOrig[key] = str;
        }
	}
}

void VindorInfo::getSettingName(MidiSerial *ser, const char *name)
{
    auto search = SettingStr.find(name);
    if (search != SettingStr.end()) {
        ser->addByte(0x13); // string/int
        ser->addString(search->first.data());
        const char *v = search->second.c_str();
        ser->addString(v);
    }
    else {
        auto value = Setting[name];
        ser->addWord(1);
        ser->addByte(0x11); // string/int
        ser->addString(name);
        ser->addWord(value);
        ser->addByte(0x10); // the end
    }
}

void VindorInfo::getSettings(MidiSerial *ser)
{
	int number = (int)(Setting.size() + SettingStr.size());
	ser->addWord(number);
	for (std::map<std::string, int>::iterator iter = Setting.begin();
         iter != Setting.end(); ++iter) {
		ser->addByte(0x11); // string/int
		ser->addString(iter->first.data());
		ser->addWord(iter->second);
	}
    for (std::map<std::string, std::string>::iterator iter = SettingStr.begin();
         iter != SettingStr.end(); ++iter) {
        ser->addByte(0x13); // string/int
        ser->addString(iter->first.data());
        const char *v = iter->second.c_str();
        ser->addString(v);
    }
	ser->addByte(0x10); // the end
}

#if 0

const char hex[] = "01234567890ABCDEF";

int hex2int(const char x)
{
    if (x >= '0' || x <= '9') return (x - '0');
    if (x >= 'A' || x <= 'F') return (x - 'A' + 10);
    return 0;
}

void VindorInfo::setFingeringData(MidiSerial *ser)
{
    size_t len = FingerData.length();
    const char *buf = FingerData.c_str();
    for(int i=0; i < len; i+=2) {
        unsigned char b = hex2int(buf[i+1]) + (hex2int(buf[i]) << 4);
        ser->addByte(b);
    }
}

void VindorInfo::getFingeringData(MidiSerial *ser)
{
    int len = ser->getLength();
    char *str = new char[len * 2];
    unsigned char *buff = ser->getBufferData();
    for (int i=0; i < len; i++) {
        str[i*2+1] = hex[*buff & 0x0F];
        str[i*2] = hex[(*buff & 0xF0) >> 4];
    }
    FingerData = str;
}

#endif

VindorControl::~VindorControl()
{
    disconnect();
}

int VindorControl::initialize()
{
    begin();
	return 0;
}

int VindorControl::connect(int showmessages, int id)
{
    if (valid) {
        return 1;
    }
	return 0;
}

std::string VindorControl::getStatus()
{
    std::string status(valid?"STATUS OK":"NOT CONNECTED");
	return status;
}

int VindorControl::isValid()
{
	return valid;
}

int VindorControl::isBusy()
{
	if (isValid() || busy) {
		return 1;
	}
	return 0;
}

int VindorControl::needUpdate()
{
    if (settings.Firmware > 0) {
        if (settings.Firmware < MinimumFirmware) return 1;
    }
    return 0;
}

class WaitForResponse : public Thread
{
public:
    WaitForResponse() : Thread("waitresponse")
    {
    }
private:
    virtual void run()
    {
        for(int i=0; i<10; i++) {
            deviceControl->sendRequest(0x17, 1);
            for(int j=0; j<10; j++) {
                Thread::sleep(100);
                if (deviceControl->settings.invalidProtocol) {
                    return;
                }
                if (deviceControl->active) {
                    return;
                }
            }
        }
        deviceControl->disconnect();
        deviceControl->triggerDataChange(4);
    }
} threadWaitResponse;

void VindorControl::onConnect()
{
    // trigger info exchange; when Vindor replies, request all data
    settings.clear();
    triggerDataChange(1);
    Thread::sleep(500);
    deviceControl->sendRequest(0x17, 1);
//    threadWaitResponse.startThread();
}

void VindorControl::onDisconnect()
{
    active = 0;
    settings.clear();
    valid = 0;
    triggerDataChange(0);
}

int pingReceived;

int VindorControl::identifyDevice() 
{
    initialize();
    return 1;
}

int VindorControl::loadPort(int showmessages) {
    return 1;
}

void VindorControl::revert()
{
    settings.revert();
    triggerDataChange(9);
}

MidiSerial globalStore;

void VindorControl::pingCallback(double timeStamp, std::vector<unsigned char> *message, void *userData) {
    if (isSysex(*message)) {
        if (message->size()==3 && message->at(1)==0x22) { // ping response
            pingReceived = 1;
        }
    }
}

void VindorControl::dataCallback(double timeStamp, std::vector<unsigned char> *message)
{
    if (settings.invalidProtocol) return;
    
    MidiSerial *store = &globalStore;
	VindorControl *me = this;
	if (me->isSysex(*message)) {
//        printf("Sysex message\n");

        if (me->diagnostic) {
//            printf("Diagnostic message\n");
            unsigned int i = 0;
            while(i < message->size()) {
                unsigned char item = message->at(i);
                me->diagbuffer[i] = item;
                if (item==0xF7) break;
                i++;
            }
            me->diagnostic = 2;
            return;
        }
        if (message->size()==3 && message->at(1)==0x22) { // ping response
            pingReceived = 1;
        }
		
		MidiChunk chunk(store);
		int ret = chunk.appendNextChunk(message->data());
        if (ret==1) {
//            printf("Need more data\n");
            return; // need more data
        }
		
		dumpMessage(store);
		
		me->lastMessage = toHexString(*message);
		int type = store->getType();
		me->lastMessageType = type;
		if (type==0x11) {
//            me->settings.getFingeringData(store);
			DBG("Got fingering");
		} else if (type==0x14) {
//            me->getInstrumentData(store);
			DBG("Got instruments");
		} else if (type==0x18) {
            if (! active) {
                active = 1;
                sendRequestAll();
            }
            else {
                active = 2;
                if (settings.invalidProtocol == 0) me->valid = 1;
                else me->valid = 0;
                time(&(me->lastStatusTime));
                triggerDataChange(3);
            }
		} else if (type==0x1A) {
            me->settings.setInfo(store);
		} else if (type==0x1C) {
			me->settings.setSettings(store);
		} else if (type==0x1D) {
			me->settings.setWifi(store);
        } else if (type==0x1E) {
            me->synth.setSerialize(store);
        }
		store->init();
        if (me->callbackFunction) {
            me->callbackFunction(me, me->callbackData);
        }
        
        globalStore.resetCurrent();
	}
	else {
		DBG("MIDI");
		DBG(toHexString(*message).data());
//        printf("MIDI:%s\n", toHexString(*message).data());
	}
}

int VindorControl::setCallback(DataCallbackFunction function, void *data)
{
    callbackFunction = function;
    callbackData = data;
	return 0;
}

int VindorControl::isSysex(std::vector<unsigned char> &message)
{
    int len = (int)message.size();
	if (len < 2) return 0;
    if (message[0] == 0xF0) return 1;
	return 0;
}

int VindorControl::getMessageType(std::vector<unsigned char> &message)
{
	size_t len = message.size();
	if (len < 6) return -1;
	if (message[0] != 0xF0) return -2;
	if (message[1] == MIDI_MFG1 &&
	    message[2] == MIDI_MFG2 &&
	    message[3] == MIDI_MFG3) {
		return message[4];
	}
    return -1;
}

int VindorControl::sendRequestAll()
{
	lastMessage = "SENDREQUESTALL:";
    
	sendRequest(0x19); // info
	sendRequest(0x1B); // settings
    sendRequest(0x22); // synth settings
//    sendRequest(0x12); // fingering
//    sendRequest(0x1D); // wifi
//    sendRequest(0x13); // instruments
    // status must be last because it triggers update
    sendRequest(0x17); // status
    
	return 0;
}

int VindorControl::sendRequestSynth(int inst)
{
    lastMessage = "SENDREQUESTSYNTH:";
    sendRequest(0x22, inst); // synth settings
    sendRequest(0x17); // status
    return 0;
}

int VindorControl::sendRequest(int request, int data)
{
	MidiSerial ser;
	ser.setType(request);
    ser.addByte(data);
	sendMidiSerial(&ser);
	return 0;
}

int VindorControl::sendSaveToFirmware()
{
    sendRequest(0x24); // save settings on instrument
    return 0;
}

int VindorControl::sendRestoreFactory()
{
    sendRequest(0x25); // send factory reset on instrument
    disconnect();
    return 0;
}

void VindorControl::sendMidiSerial(MidiSerial *ser)
{
	MidiChunk chunk(ser);
	int len;
	int ret;
	unsigned char *data;
	while(1) {
		ret = chunk.loadNextChunk();
		data = chunk.getChunk(&len);
        std::vector<unsigned char> message(data, data+len);
		DBG(toHexString(message).data());
        sendMessage(message);
		if (ret==0) break;
	}
}

int VindorControl::sendNoteOnOff(int newstate, int note, int volume)
{
    unsigned char dd[3];
    dd[0] = newstate?0x90:0x80;
    dd[1] = note & 0x7F;
    dd[2] = volume & 0x7F;
    std::vector<unsigned char> d(dd, dd+sizeof(dd));
    sendMessage(d);
    return 0;
}

int VindorControl::sendProgramChange(int inst)
{
    unsigned char dd[2];
    dd[0] = 0xC0;
    dd[1] = inst & 0x7F;
    std::vector<unsigned char> d(dd, dd+sizeof(dd));
    sendMessage(d);
    return 0;
}

int VindorControl::enterBootloader() 
{
    unsigned char dd[] = {0xF0, MIDI_MFG1, MIDI_MFG2, MIDI_MFG3, 0x17, 0x53, 0xF7};
    std::vector<unsigned char> d(dd, dd+sizeof(dd));
    sendMessage(d);
//    diagnostic = 1;
    return 1;
}

int VindorControl::enterDiag() 
{
    unsigned char dd[] = {0xF0, MIDI_MFG1, MIDI_MFG2, MIDI_MFG3, 0x17, 0x34, 0xF7};
    std::vector<unsigned char> d(dd, dd+sizeof(dd));
    sendMessage(d);
    diagnostic = 1;
    return 1;
}

int VindorControl::sendFilterSettingLong(int setting, int n, int bytes)
{
//    char dbg[256];
//    sprintf(dbg, "SETL %d: %d",setting, n);
//    DBG(dbg);
    
    unsigned char dd[] = {0xF0, MIDI_MFG1, MIDI_MFG2, MIDI_MFG3, 0x17, 0x40};
    std::vector<unsigned char> d(dd, dd+sizeof(dd));
    d.push_back((unsigned char)setting);
    for(int i=0; i<bytes; i++) {
        d.push_back(n & 0x7F);
        n = n >> 7;
    }
    d.push_back(0xF7);
    sendMessage(d);
    return 1;
}

int VindorControl::sendFilterSetting(int setting, int value1, int value2, int value3) 
{
//    char dbg[256];
//    sprintf(dbg, "SET %d: %d %d %d",setting, value1, value2, value3);
//    DBG(dbg);

    unsigned char dd[] = {0xF0, MIDI_MFG1, MIDI_MFG2, MIDI_MFG3, 0x17, 0x40};
    std::vector<unsigned char> d(dd, dd+sizeof(dd));
    d.push_back((unsigned char)setting);
    d.push_back((unsigned char)value1);
    d.push_back((unsigned char)value2);
    d.push_back((unsigned char)value3);
    d.push_back(0xF7);
    sendMessage(d);
    return 1;
}

int VindorControl::sendFilterSettingFloat(int setting, int param, float value)
{
    unsigned char d[64];
    d[0] = 0xF0;
    d[1] = MIDI_MFG1; d[2] = MIDI_MFG2; d[3] = MIDI_MFG3;
    d[4] = 0x17; d[5] = 0x40;
    d[6] = setting;
    d[7] = param;
    int len = MidiSerial::formatFloat2Midi(value, &d[8]);
    d[8+len] = 0xF7;
    std::vector<unsigned char> ds(d, d+9+len);
    sendMessage(ds);
    return 1;
}

int VindorControl::exitDiag() 
{
    unsigned char dd[] = {0xF0, 0x31, 0xF7};
    std::vector<unsigned char> d (dd, dd+sizeof(dd));
    sendMessage(d);
    diagnostic = 0;
    return 1;
}

int VindorControl::sendDiag(unsigned char *data, int len)
{
    std::vector<unsigned char> message(data, data+len);
    sendMessage(message);
    *diagbuffer = 0;
    return 1;
}

int VindorControl::getDiag(unsigned char *data)
{
    unsigned char *d = diagbuffer;
    if (*d == 0) return 0;
    if (*d != 0xF0) return 0;
    while(*d != 0xF7) {
        *data++ = *d++;
    }
    return 1;
}

int VindorControl::testWifi()
{
    enterDiag();
    
    size_t l;
    unsigned char d[128];
    d[0] = 0xF0;
    d[1] = 0x0A;
    d[2] = 0x01;
    strcpy((char*)d+3, settings.WifiSSID.c_str());
    l = strlen((char*)d);
    d[l] = 0xF7;
    sendDiag(d, (int)l+1);
    
    d[0] = 0xF0;
    d[1] = 0x0A;
    d[2] = 0x02;
    strcpy((char*)d+3, settings.WifiPASS.c_str());
    l = strlen((char*)d);
    d[l] = 0xF7;
    sendDiag(d, (int)l+1);
    
    d[0] = 0xF0;
    d[1] = 0x0A;
    d[2] = 0x03;
    d[3] = 0xF7;
    sendDiag(d, 4);
    
    d[0] = 0xF0;
    d[1] = 0x0A;
    d[2] = 0x04;
    d[3] = 0xF7;
    sendDiag(d, 4);
    
    int status = 0;
    for(int i=0; i<20; i++) {
        // SLEEP(100);
        if (diagnostic == 2) {
            int r = getDiag(d);
            if (r && d[1] == 1) { // valid response
                if (d[2] == 1) { // ok
                    status = 1;
                    break;
                }
                else { // not ok
                }
            }
            diagnostic = 1;
        }
    }
    
    exitDiag();
    
    return status;
}

void VindorControl::getFingerSerialize(SettingsData &s, MidiSerial *ser)
{
    ser->addByte(1);
    ser->addWord2(s.fingerlist_custom_bits);
    ser->addWord4(s.fingerlist_custom_mask);
    ser->addWord2(s.fingerlist_custom_useoctave);
    ser->addWord(s.fingerlist_custom_count);
    for (int i=0; i<s.fingerlist_custom_count; i++) {
        ser->addWord2(backup_fingerlist_custom[i]);
    }
}

int VindorControl::sendSettingName(const char *name)
{
    Time::waitForMillisecondCounter(lastSendTime);
    MidiSerial ser;
    settings.getSettingName(&ser, name);
    ser.setType(0x1C);
    sendMidiSerial(&ser);
    lastSendTime = Time::getMillisecondCounter() + 100;
    return 0;
}

int VindorControl::sendSettings()
{
    Time::waitForMillisecondCounter(lastSendTime);
	MidiSerial ser;
	settings.getSettings(&ser);
	ser.setType(0x1C);
	sendMidiSerial(&ser);
    lastSendTime = Time::getMillisecondCounter() + 1000;
    return 0;
}

int VindorControl::sendSettingsFingering()
{
    MidiSerial ser;
    getFingerSerialize(settings.DevSettings, &ser);
    ser.setType(0x10);
    sendMidiSerial(&ser);
	return 0;
}

int VindorControl::sendWifi()
{
	MidiSerial ser;
	settings.getWifi(&ser);
	ser.setType(0x1E);
	sendMidiSerial(&ser);
	return 0;
}

int VindorControl::sendReboot()
{
    sendRequest(0x20);
    return 0;
}
