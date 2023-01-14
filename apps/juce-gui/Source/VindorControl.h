#ifndef VINDORCONTROL_H
#define VINDORCONTROL_H

#include <time.h>
#include <map>
#include <string>
#include "MidiSer.h"

#include "MidiControl.h"
#include "SynthSettings.h"
#include "Settings.h"

#include "MidiFileRemote.h"

class VindorControl;
typedef void (*DataCallbackFunction)( VindorControl *object, void *data );

class VindorInfo
{
	public:
    int invalidProtocol = 0;

	std::string Name;
	std::string Desc;
	int Revision;
    int Platform;
    int Firmware;
    std::string CPUguid;
    std::string CompileTime;
    std::string CPUmodel;
    
    SettingsData DevSettings;

    int WifiEnabled;
    std::string WifiSSID;
    std::string WifiPASS;
    int UsbMode;
    
	std::map<std::string, int> Info;
	
	std::map<std::string, int> Setting;
    std::map<std::string, int> SettingOrig;
    std::map<std::string, std::string> SettingStr;
    std::map<std::string, std::string> SettingStrOrig;

    void clear();
    void revert();
    
	void setInt(const char *key, int number);
	int getInt(const char *key);
    void setStr(const char *key, const char *value);
    std::string getStr(const char *key);
	int hasSettings();

	bool setInfo(MidiSerial *ser);
	void setWifi(MidiSerial *ser);
	void getWifi(MidiSerial *ser);
	
	void setSettings(MidiSerial *ser);
	void getSettings(MidiSerial *ser);
    void getSettingName(MidiSerial *ser, const char *name);

//    void setFingeringData(MidiSerial *ser);
//    void getFingeringData(MidiSerial *ser);
};

class VindorControl : public MidiControl
{
private:
    int valid = 0;
    int lastSendTime;

	int portNumberIn;
	int portNumberOut;
    
    int diagnostic = 0;
    unsigned char diagbuffer[256];
    
    DataCallbackFunction callbackFunction;
    void *callbackData;
    virtual void dataCallback(double timeStamp, std::vector<unsigned char> *message);
    virtual void onConnect();
    virtual void onDisconnect();

	static void dataCallback(double timeStamp, std::vector<unsigned char> *message, void *userData);
    static void pingCallback(double timeStamp, std::vector<unsigned char> *message, void *userData);

    
public:
    const static int MinimumFirmware = 2;

	~VindorControl();

    MidiFileRemote remoteFile;

	std::string midiName;
	std::string lastError;
	std::string lastMessage;
	time_t lastStatusTime;
	int lastMessageType;
    int busy;
    int active = 0;
    int connectState = 0;
    
    SynthSettings synth;
	VindorInfo settings;
    int16_t backup_fingerlist_custom[1024];
        
	int initialize();
	int connect(int showmessages, int id=-1);
	
    int identifyDevice();
    
	int isValid();
    int isBusy();
    int needUpdate();
	int loadPort(int showmessages);
	static int isSysex(std::vector<unsigned char> &message);
	int getMessageType(std::vector<unsigned char> &message);
	int sendRequestAll();
    int sendRequestSynth(int inst);
	int sendRequest(int request, int data=0);
    
    void revert();
    
    void getFingerSerialize(SettingsData &s, MidiSerial *ser);
	int sendSettings();
    int sendSettingName(const char *name);
    int sendSettingsFingering();
	int sendWifi();
	    
    int sendReboot();

	void sendMidiSerial(MidiSerial *ser);

	int sendSaveToFirmware();
    int sendRestoreFactory();

    int sendFilterSetting(int setting, int value1=0, int value2=0, int value3=0);
    int sendFilterSettingFloat(int setting, int param, float value);
    int sendFilterSettingLong(int setting, int n, int bytes);
    int sendNoteOnOff(int state, int note, int volume=127);
    int sendProgramChange(int inst);

    int enterBootloader();
    int enterDiag();
    int exitDiag();
    int sendDiag(unsigned char *data, int len);
    int getDiag(unsigned char *data);
    int testWifi();
	
	int setCallback(DataCallbackFunction function, void *data);
	
	std::string getStatus();
    
    int setJSON(const char *json);
    std::string getJSON();
    
    virtual void triggerDataChange(int state) = 0;
};

#endif // VINDORCONTROL_H
