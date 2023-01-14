#ifndef Manage_H
#define Manage_H

#include "MidiSer.h"
#include "MidiFileRemote.h"

class Manage {
    //
    // The code section below is for communicating with the management software using MIDI
    //
public:
    static MidiFileDevice file;

    static void sendSerial(MidiSerial *serial);
    /*
     * Send all the active fingering information to the management app
     */

    static void sendFingeringToMIDI();
    static void readFingeringFromMIDI(MidiSerial &ser);
    static void sendSynthSettings(MidiSerial &ser);
    static void readSynthSettings(MidiSerial &ser);
    static const char *getCPUModel();
    
    static void sendInformation();
    static void sendMidiStatus(int statusid);
    /*
     void sendInformation(int statusid) {
     MidiSerial *ser = MidiSerial::getInstance(true);
     ser->setType(0x1C);
     ser->addByte(statusid);
     sendSerial(ser);
     }
     */
    
    static void addKeyValue(MidiSerial &ser, const char * string, int value);
    static void addKeyFloat(MidiSerial &ser, const char * string, float value);
    static void addKeyStr(MidiSerial &ser, const char * string, const char *value);
    
    static void sendSettings();
    // void setinstrument(int inst);
    
    static void readSettings(MidiSerial &ser);
    static void sendWifi();
    static void readWifi(MidiSerial &ser);
    static void pingMidi(int ditem=0);
    
    static void processMIDIControl() ;
    
    static void readMIDI();
    
};

class HostFile {
public:
    void open(const char *fn, const char *fmode);
    void write(const uint8_t *data, int length);
    int read(uint8_t *buffer, int maxlen) ;
    void close();
};

class SimpleProfile {
public:
    static SimpleProfile *list[256];
    static int idx;
    
    const char *name;
    int callcount;
    uint32_t stime;
    long tcount;
    
    SimpleProfile(const char *fxn);
    void start() ;
    void end();
    void count() ;
    
    static void append(SimpleProfile *p);
    static void dump();
};

class SimpleProfileScope {
    SimpleProfile *me;
    SimpleProfileScope(SimpleProfile *p) {
        me = p;
        me->start();
    }
    ~SimpleProfileScope() {
        me->end();
        me->count();
    }
};

#define PROF_FUNC \
static SimpleProfile(__FXN__) static_prof_ ## __FXN__; \
SimpleProfileScope(&static_prof_ ## __FXN__);

#endif
