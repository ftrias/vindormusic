#ifndef Event_H
#define Event_H

#include <stdint.h>

/**
 * @brief Class to queue deferred MIDI messages. This is used for NoteOff events to
 * send repeated ones after a small delay in case the first message is lost.
 * 
 */
class MidiBuffer {
private:
    struct midi_event {
        unsigned int triggertime;
        int data;
    };
    const static int buffer_len = 10;
    struct midi_event buffer[buffer_len];
    int writeidx = 0;
    int readidx = 0;
public:
    MidiBuffer();
    void add(int expire, int in_data);
    int next(int *data);
    int size();
};

extern MidiBuffer midi_buffer;

#ifdef USE_WIFI

#include "Settings.h"

class WifiMidi {
    public:
    int isPresent = 0;
    int isResponding = 0;
    // int isEnabled = 0;
    int isConnected = 0;
    int isWifi = 0;
    int isInit = 0;
    int inByte;
    int byteCount = 0;
    int mstate = 0;
    
    // char wifi_ssid[256];
    // char wifi_pass[256];
    int ver_idx;
    char ver[16];
    char debug[255];
    int debug_idx;
    
    void begin();
    
    bool isEnabled() { return settings.enableWifi; }
    // void setLogin(const char *ssid, const char *pass);
    void sendCommand(int command, const char *str = NULL);
    int sendLogin();
    void shutdown();
    void readData();
};

extern WifiMidi midi_wifi;

#endif

/**
 * @brief Midi is used to send/receive MIDI messages.
 * 
 */
extern int enable_uart_midi;
extern int last_midi_time;
class Midi {
public:
    //
    // Section for MIDI signaling
    //
    
    static int isMidiReceiving();
    /**
     * Send a MIDI command to serial.
     *
     * @param cmd MIDI command
     * @param pitch MIDI second byte, the pitch for NoteOn/OFF
     * @param velocity optional MIDI third byte, the volume for NoteON/OFF
     */
    static void sendMIDI(int cmd, int pitch, int velocity = -1, int xtra = -1);
    
    static void midiFlush();
    
    static void midiActive();

    /**
     * Send SysEx MIDI control as array; currently used for debugging.
     */
    static void midiSysEx(int length, const uint8_t * data);
    /**
     * Send SysEx MIDI control as 16 bits; currently used for debugging.
     */
    static void midiSysEx2(const uint16_t data) ;
    static void midiSysExDecimal(const uint16_t data) ;
    static void midiSysExDecimal2(const int i, const uint16_t data) ;
    static void midiSysExDecimal2x(const int x, const int i, const int j) ;
    static void midiSysExBin(const uint16_t data) ;

    static void midiSetRate(int rate);
    
    /**
     * Send NoteOn event to USB and serial
     *
     * @param note MIDI note number
     * @param volume MIDI volume from 0 to 127
     * @param channel MIDI channel number from 0 to 127
     */
    static void midiNoteOn(int note, int volume, int channel, int realvol = -1);
    
    /**
     * Send NoteOff event to USB and serial
     *
     * @param note MIDI note number
     * @param volume MIDI volume from 0 to 127
     * @param channel MIDI channel number from 0 to 127
     */
    static void midiNoteOff(int note, int volume, int channel);
    
    /**
     * Send afterTouch event
     *
     * @param touch Touch value from 0 to 127
     * @param channel MIDI channel number
     */
    static void midiAfterTouch(unsigned int touch, int channel);
    
    /**
     * Send pitch bend
     */
    static void midiPitchBend(unsigned int bend);
    
    /**
     * Send generil MIDI control, used for volume, etc
     */
    static void midiControl(int number, int data, int channel);
    
    /*
     void midiControlExtra(int note, int volume, int channel)
     {
#ifdef RFMIDI
     if (rfmidi.enabled) rfmidi.sendMIDI(0x30+channel-1, note, volume, -1);
#endif
     }
     */
    
    static void midiProgramChange(int prognum, int channel);
    
    static void midiSendDebug(int pv);
    static void midiSendDebug2(uint16_t b);
    
    static void midiSendDebug3(int len, int *pv);
    
    static void testSysEx();
};

/**
 * @brief Higher level class to manage when notes start/stop/change and to send the appropriate
 * messages to the synth, MIDI, etc.
 * 
 */
class Event {
    
public:
    
    static int overrideControl;
    
    static int eventSendVolume(int volume, int changenote = 0);
    
    /**
     * Breath control has changed, so process it and send MIDI codes
     * volume from 0 to 1024
     */
    static void eventBreath(int volume);
    
    /**
     * Send new pitch bend to MIDI devices
     * from -100 to +100
     */
    static void setPitchBend(int bend);
    
    static void setPortamento(int port);

#if 0
    /**
     * Send new pitch bend to MIDI devices
     * from -100 to +100
     */
    static void eventPitchBend();
#endif
    
    /**
     * Send ModWheel changes
     */
    static void eventCC(int cc, int value);
    static void eventEffect(int cc, float value, float *prev);
    
    /**
     * Send NoteOff
     */
    static void eventNoteOff();
    
    /**
     * Send NoteOn
     */
    static void eventNoteOn(int note, int volume);
    
    /**
     * Send NoteOff/NoteOn
     */
    static void eventNoteChange(int note, int volume);
    
    static void processMidiBreath();
    static void processMidiSlow() ;
    static void processMidiAlive() ;
    
};

#endif
