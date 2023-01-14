#include "firmware.h"

#include <Audio.h>
#include <usb_desc.h>
#include <usb_dev.h>

#include "Event.h"

#include "SoundSynthesis.h"

#include "OutputMode.h"

#include "Settings.h"
#include "Display.h"
#include "Accelerometer.h"

#include "Dispatch.h"
extern int dispatchMidiId;
extern Dispatch dispatch;

extern volatile uint8_t usb_configuration;

int enable_uart_midi = 0;
int last_midi_time=0;
extern int playingNote;

int playingVolume = 0;
int Event::overrideControl = 0;
int newBend = 0;

MidiBuffer::MidiBuffer() {
    for(int i=0; i < buffer_len; i++) {
        buffer[i].triggertime = 0;
    }
}
void MidiBuffer::add(int expire, int in_data) {
    if (settings.midiNoteOffRepeat == 0) return;
    buffer[writeidx].data = in_data;
    buffer[writeidx].triggertime = millis() + expire;
    if (++writeidx >= buffer_len) writeidx = 0;
}
int MidiBuffer::next(int *data) {
    if (readidx == writeidx) return 0;
    if (buffer[readidx].triggertime <= millis()) {
        *data = buffer[readidx].data;
        // buffer[readidx].triggertime = 0;
        if (++readidx >= buffer_len) readidx = 0;
        return 1;
    }
    return 0;
}
int MidiBuffer::size() {
    if (readidx <= writeidx) return writeidx - readidx;
    return buffer_len - readidx + writeidx;
}

MidiBuffer midi_buffer;

#ifdef USE_WIFI

#include "Lights.h"

WifiMidi midi_wifi;

void WifiMidi::begin() {
#if 0
    pinMode(UART_SENSE_PIN_RX, INPUT_PULLDOWN);
    delay(1);
    bool r = digitalRead(UART_SENSE_PIN_RX)?1:0;
    int x = analogRead(UART_SENSE_PIN_RX);
    Serial.print("RX:");
    Serial.println(x);
    pinMode(UART_SENSE_PIN_RX, OUTPUT);

    if (r) isPresent = 1;
#endif

    // Uart.begin(31250); // DEBUG
    mstate = 0;
    /*
    Power::setPeripherals(0);
    delay(100);
    Power::setPeripherals(1);
    */
#ifdef DEBUGWIFI
    Serial.println("Wifi enabled");
#endif
}

/*
void WifiMidi::setLogin(const char *ssid, const char *pass) {
    strcpy(wifi_ssid, ssid);
    strcpy(wifi_pass, pass);
#ifdef DEBUGWIFI
    Serial.println("Set Wifi ssid/pass");
#endif
}
*/

#if 0
void sendCryptMidi(uint8_t value) {
    uint8_t v = (~value) & 0x7F;
    Uart.write(v);
}
#endif

void WifiMidi::sendCommand(int command, const char *str) {
    Uart.write(0xF1);
    Uart.write((uint8_t)command);
    if (str) {
        int len = strlen(str);
        Uart.write((uint8_t)len);
        for(int i = 0; i < len; i++) {
            Uart.write((uint8_t)str[i]);
        }
    }
    Uart.write(0xF7);
    Uart.flush();
}

int WifiMidi::sendLogin()
{
#ifdef DEBUGWIFI
    Serial.println("Wifi send login");
#endif        
    if (settings.ssid == NULL || settings.ssid[0] == 0) return 0;
#ifdef DEBUGWIFI
    Serial.println("Wifi login");
    Serial.println(settings.ssid);
    Serial.println(settings.pass);
#endif
    sendCommand(1, settings.ssid);
    sendCommand(2, settings.pass);
    sendCommand(3);
    return 1;
}

void WifiMidi::shutdown() {
#ifdef DEBUGWIFI
    Serial.println("shutdown wifi");
#endif
    sendCommand(6);
    // Power::external(0);
    // isEnabled = false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void WifiMidi::readData()
{        
#ifdef DEBUGWIFI
    static int ttime;
    if (millis() - ttime > 5000) {
        Serial.println(".");
        ttime = millis();
    }
#endif
    if (Uart.available() > 0) {
        char inByte = Uart.read();
        // Display::displayDebug("WIFI:C:", (int)inByte);
        byteCount++;
#ifdef DEBUGWIFI
        Serial.print("[");
        Serial.print(inByte);
        Serial.print("]");
#endif
        if (mstate > 0) {
            if (byteCount == 2) {
                if (inByte == 'V') { // version
                    ver_idx = 0;
                    mstate = 4;
                }
                if (inByte == 'D') { // debug
                    debug_idx = 0;
                    mstate = 2;
                }
                else { // unknown or unsupported
                    mstate = 0;
                }
            }
            if (mstate == 4) {
                if (inByte == '\n' || inByte == '\r') {
                    isResponding = 1;
                    ver[ver_idx] = 0;
                    Serial.print("D:");
                    Serial.println(ver);
                    // Display::displayDebug("WIFI:VER:", ver);
                    mstate = 0;
                    ver_idx = 0;
                    byteCount = 0;
                }
                else {
                    ver[ver_idx++] = inByte;
                }
            }
            else if (mstate == 2) {
                if (inByte == '\n' || inByte == '\r') {
                    debug[debug_idx] = 0;
                    Serial.print("D:");
                    Serial.println(debug);
                    // Display::displayDebug("WIFI:D:", debug);
                    mstate = 0;
                    debug_idx = 0;
                    byteCount = 0;
                }
                else {
                    debug[debug_idx++] = inByte;
                }
            }
            else {
                mstate = 0;
            }
        }
        else {
            if (inByte == 'S') { // get status
                isPresent = 1;
                if (0 && usb_configuration) {
                    shutdown();
                    isInit = -1;
                    isWifi = false;
                    isConnected = false;
                    byteCount = -99;
                }
                else {
                    // isEnabled = true;
                    byteCount = 0;
                }
            }
            else if (byteCount == 1 && inByte == 'W') {
                // Display::displayDebug("WIFI:wifi");
                isWifi = true;
            }
            else if (byteCount == 1 && inByte == 'X' && isInit != -1) {
                isWifi = false;
                if (settings.enableWifi == 0) {
                    // Display::displayDebug("WIFI:shutdown");
                    shutdown(); // go to sleep
                    isInit = -1;
                }
                else if (isInit == 0) {
                    if (sendLogin()){
                        // Display::displayDebug("WIFI:login:ok");
                        isInit = millis();
                    } 
                    else {
                        // Display::displayDebug("WIFI:login:fail");
                    }
                }
                else if (millis() - isInit > 30000) { // 30 secs since init, but still not connected
                    // Display::displayDebug("WIFI:inactive");
                    shutdown(); // go to sleep immediately
                    isInit = -1;
                }
                // else if (millis() - isInit > 2000) { // 2 secs since init, but still not connected
                //     Display::displayDebug("WIFI:login2");
                //     sendLogin();
                // }
            }
            else if (byteCount == 1 && inByte == 'R') { // response
                mstate = 1;
            }
            else if (byteCount==2 && inByte == '1') { // get status
                // Display::displayDebug("WIFI:connected");
                isConnected = true;
                // Display::displayDebuug("WC=1");
            }
            else if (byteCount==2 && inByte == '0') { // get status
                isConnected = false;
                // Display::displayDebug("WC=0");
                if (isInit > 0 && millis() - isInit > 120000) { // 2 minutes since init, but still not connected
                    shutdown(); // go to sleep immediately
                    isWifi = false;
                    isInit = -1;
                }
            }
            else if (byteCount==3 && inByte == 'C') { // client connected to AP
                // isConnected = true;
            }
            else if (byteCount==3 && inByte == 'A') { // not client on AP
                // isConnected = false;
            }
            else byteCount = -99;
        }

        if (isConnected) led.sleep(4);
        else if (isWifi) led.sleep(1);
        else led.sleep(0);
    }
}

#endif

//
// Section for MIDI signaling
//

int Midi::isMidiReceiving() {
#ifdef USBMIDI
    if (! currentOutput.enableMidi) return 0;
    //static int prev_count = 0;
    int count = usb_tx_packet_count(MIDI_TX_ENDPOINT);
    if (count<5) return 1;// ok so far
    //prev_count = count;
#endif
    return 0;
}

// int debug_midi_count = 0;

/**
 * Send a MIDI command to serial.
 *
 * @param cmd MIDI command
 * @param pitch MIDI second byte, the pitch for NoteOn/OFF
 * @param velocity optional MIDI third byte, the volume for NoteON/OFF
 */
void Midi::sendMIDI(int cmd, int pitch, int velocity, int xtra) {
#ifdef USE_UART
    if (enable_uart_midi) {
        Uart.write(cmd);
        Uart.write(pitch);
        if (velocity>=0) {
            Uart.write(velocity);
        }
        last_midi_time = millis();
    }
#ifdef DEBUGMIDI
    else {
        Serial.println("UART disabled");
    }
    Serial.print("SENDMIDI ");
    Serial.print(cmd, HEX);
    Serial.print(":");
    Serial.print(pitch, HEX);
    Serial.print(":");
    Serial.print(velocity, HEX);
    Serial.println();
#endif
#endif
}

void Midi::midiFlush() {
#ifdef USBMIDI
    usbMIDI.send_now();
#endif
}

/**
 * Send SysEx MIDI control as array; currently used for debugging.
 */
void Midi::midiSysEx(int length, const uint8_t * data) {
#ifdef USBMIDI
    if (isMidiReceiving()) usbMIDI.sendSysEx(length, data, true);
#endif
#ifdef USE_UART
    if (! enable_uart_midi) return;
    Uart.write(0xF0);
    for(int i=0; i<length; i++) {
        Uart.write(data[i]);
    }
    Uart.write(0xF7);
#endif
}

void Midi::midiActive() {
#ifdef USBMIDI
    if (isMidiReceiving()) usbMIDI.sendRealTime(0xFE);
#endif
#ifdef USE_UART
    if (! enable_uart_midi) return;
    Uart.write(0xFE);
#endif
}

void Midi::midiSetRate(int rate) {
    settings.midiRate = rate;
    dispatch.setRate(dispatchMidiId, rate);
}

/************
 * 
 * Debugging output to MIDI
 * 
 ************/

/**
 * Send SysEx MIDI control as 16 bits; currently used for debugging.
 */
void Midi::midiSysEx2(const uint16_t data) {
    uint8_t d[6];
    d[0] = 0xF0;
    d[1] = (data >> 12) & 0x0F;
    d[2] = (data >> 8) & 0x0F;
    d[3] = (data >> 4) & 0x0F;
    d[4] = data & 0x0F;
    d[5] = 0xF7;
    midiSysEx(6, d);
}

void Midi::midiSysExDecimal(const uint16_t data) {
    uint8_t d[7];
    d[0] = 0xF0;
    d[1] = (data / 10000) % 10;
    d[2] = (data / 1000) % 10;
    d[3] = (data / 100) % 10;
    d[4] = (data / 10) % 10;
    d[5] = (data) % 10;
    d[6] = 0xF7;
    midiSysEx(7, d);
}

void debugOutputDecimal(const int data) {
    Midi::midiSysExDecimal(data);
}

void debugOutputDecimal2(const int x, const int i, const int j) {
    Midi::midiSysExDecimal2x(x, i, j);
}

void Midi::midiSysExDecimal2(const int i, const uint16_t data) {
    uint8_t d[8];
    d[0] = 0xF0;
    d[1] = i;
    d[2] = (data / 10000) % 10;
    d[3] = (data / 1000) % 10;
    d[4] = (data / 100) % 10;
    d[5] = (data / 10) % 10;
    d[6] = (data) % 10;
    d[7] = 0xF7;
    midiSysEx(8, d);
}

void Midi::midiSysExDecimal2x(const int x, int i, int j) {
    uint8_t d[16];
    d[0] = 0xF0;
    d[1] = x;
    if (i<0) { d[2] = 0x0F; i = -i; }
    else { d[2] = 0; }
    d[3] = (i / 10000) % 10;
    d[4] = (i / 1000) % 10;
    d[5] = (i / 100) % 10;
    d[6] = (i / 10) % 10;
    d[7] = i % 10;
    d[8] = 0x11;
    if (j<0) { d[9] = 0x0F; j = -j; }
    else { d[9] = 0; }
    d[10]= (j / 10000) % 10;
    d[11]= (j / 1000) % 10;
    d[12]= (j / 100) % 10;
    d[13]= (j / 10) % 10;
    d[14]= j % 10;
    d[15]= 0xF7;
    midiSysEx(16, d);
}

void Midi::midiSysExBin(const uint16_t data) {
    uint8_t d[18];
    d[0] = 0xF0;
    d[1] = data & (1<<15) ? 1 : 0;
    d[1] = data & (1<<15) ? 1 : 0;
    d[1] = data & (1<<15) ? 1 : 0;
    d[1] = data & (1<<15) ? 1 : 0;
    d[2] = data & (1<<14) ? 1 : 0;
    d[3] = data & (1<<13) ? 1 : 0;
    d[4] = data & (1<<12) ? 1 : 0;
    d[5] = data & (1<<11) ? 1 : 0;
    d[6] = data & (1<<10) ? 1 : 0;
    d[7] = data & (1<<9) ? 1 : 0;
    d[8] = data & (1<<8) ? 1 : 0;
    d[9] = data & (1<<7) ? 1 : 0;
    d[10] = data & (1<<6) ? 1 : 0;
    d[11] = data & (1<<5) ? 1 : 0;
    d[12] = data & (1<<4) ? 1 : 0;
    d[13] = data & (1<<3) ? 1 : 0;
    d[14] = data & (1<<2) ? 1 : 0;
    d[15] = data & (1<<1) ? 1 : 0;
    d[16] = data & (1) ? 1 : 0;
    d[17] = 0xF7;
    midiSysEx(18, d);
}


void Midi::midiSendDebug(int pv) {
    uint8_t px[12];
    px[0] = 0xF0;
    px[1] = px[2] = px[3] = 0;
    if (pv < 0) {
        pv = -pv;
        px[4] = 0x7F;
    }
    else {
        px[4] = 0;
    }
    px[5] = (pv / 100000) % 10;
    px[6] = (pv / 10000) % 10;
    px[7] = (pv / 1000) % 10;
    px[8] = (pv / 100) % 10;
    px[9] = (pv / 10) % 10;
    px[10] = pv % 10;
    px[11] = 0xF7;
    usbMIDI.sendSysEx(12, px, true);
}

void Midi::midiSendDebug2(uint16_t b) {
    uint8_t px[128];
    px[0] = 0xF0;
    px[1] = 1;
    px[2] = 2;
    px[3] = 3;
    for(int i=0; i<16; i++) {
        px[4+i] = (b >> (15-i)) & 1;
    }
    px[4+16] = 0xF7;
    usbMIDI.sendSysEx(5+16, px, true);
}

void Midi::midiSendDebug3(int len, int *pv) {
    uint8_t px[128];
    px[0] = 0xF0;
    px[1] = 1;
    px[2] = 2;
    px[3] = 3;
    for(int i=0; i<len; i++) {
        px[4+i] = pv[i] & 0x7F;
    }
    px[4+len] = 0xF7;
    usbMIDI.sendSysEx(5+len, px, true);
}

void Midi::testSysEx() {
#ifdef USBMIDI
    static unsigned char data[] = {0xF0, 0x11, 0xF7};
    usbMIDI.sendSysEx(sizeof(data), data, true);
#endif
    sendMIDI(0x70, 0x11, 0xF7);
}

/***********
 * 
 * MIDI events
 * 
 ***********/

/**
 * Send NoteOn event to USB and serial
 *
 * @param note MIDI note number
 * @param volume MIDI volume from 0 to 127
 * @param channel MIDI channel number from 0 to 127
 */
void Midi::midiNoteOn(int note, int volume, int channel, int realvol) {
#ifdef USBMIDI
    if (isMidiReceiving()) {
        usbMIDI.sendNoteOn(note, volume & 0x7F, channel);
    }
#endif
    sendMIDI(0x90 + channel - 1, note, volume & 0x7F, realvol);
}

/**
 * @brief Send NoteOff event to USB and serial
 *
 * @param note MIDI note number
 * @param volume MIDI volume from 0 to 127
 * @param channel MIDI channel number from 0 to 127
 */
void Midi::midiNoteOff(int note, int volume, int channel) {
    if (note <= 0) return;
#ifdef USBMIDI
    // need to send both because some synthesizers respond to one
    // but not the other.
    if (isMidiReceiving()) {
        usbMIDI.sendNoteOff(note, volume & 0x7F, channel);
    }
#endif
    sendMIDI(0x80 + channel - 1, note, volume & 0x7F);
}

/**
 * @brief Send afterTouch event
 *
 * @param touch Touch value from 0 to 127
 * @param channel MIDI channel number
 */
void Midi::midiAfterTouch(unsigned int touch, int channel) {
#ifdef USBMIDI
    if (isMidiReceiving()) {
        usbMIDI.sendAfterTouch(touch & 0x7F, channel);
    }
#endif
    sendMIDI(0xD0 + channel - 1, touch & 0x7F);
}

/**
 * @brief Send pitch bend
 * 
 * @param bend Bend +/-8000
 */
void Midi::midiPitchBend(unsigned int bend) {
    static unsigned int lastBendSent = 0;
    if (bend > 16383) {
        // return; // too big!
        bend = 16383;
    }
    if (bend == lastBendSent) return;
    lastBendSent = bend;
#ifdef USBMIDI
    if (isMidiReceiving()) {
        usbMIDI.send(0xE0, bend & 0x7F, bend >> 7, settings.channel, 0);
        // usbMIDI.sendPitchBend(bend, settings.channel);
        //usbMIDI.send_now();
    }
#endif
    sendMIDI(0xE0 + settings.channel - 1, bend & 0x7F, bend >> 7);
}

/**
 * @brief Send generil MIDI control, used for volume, etc
 * 
 * @param number Control number
 * @param data Data (0-127)
 * @param channel Channel (usually 0)
 * @return 
 */
void Midi::midiControl(int number, int data, int channel) {
#ifdef USBMIDI
    if (isMidiReceiving()) {
        usbMIDI.sendControlChange(number, data, channel);
        //usbMIDI.send_now();
    }
#endif
    sendMIDI(0xB0 + channel - 1, number, data);
}

/**
 * @brief Send program change (instrument change)
 * 
 * @param prognum Program number
 * @param channel Channel (usually 0)
 */
void Midi::midiProgramChange(int prognum, int channel) {
#ifdef USBMIDI
    if (isMidiReceiving()) {
        usbMIDI.sendProgramChange(prognum, channel);
    }
#endif
    sendMIDI(0xC0 + channel - 1, prognum);
}

/**********
 * 
 * Event handliing
 * 
 **********/

/**
 * @brief Volume change event
 * 
 * @param volume Volume (1-65535)
 * @param changenote Ignored
 * @return int MIDI volume (0-127)
 */
int Event::eventSendVolume(int volume, int changenote) {
    if (playingNote < 0)
        return 0;
    
    static int lastLSB;
    static int lastMSB;
    
    int midiMSB = (volume >> volume2midi) & 0x7F;
    
    if (settings.midiCC > 0) {
        if (lastMSB != midiMSB) {
            Midi::midiControl(settings.midiCC, midiMSB, settings.channel);
        }
        if (settings.midiHires) {
            int midiLSB = (volume >> volume2midi_hires) & 0x7F;
            if (lastLSB != midiLSB) {
                Midi::midiControl(settings.midiCC+32, midiLSB, settings.channel);
            }
            lastLSB = midiLSB;
        }
    }
    else if (settings.midiCC == 0) {
        if (lastMSB != midiMSB) {
            Midi::midiAfterTouch(midiMSB, settings.channel);
        }
    }
    
    lastMSB = midiMSB;    
    return midiMSB;
}

/**
 * @brief Breath control has changed, so process it and send MIDI codes
 * volume from 0 to 1024
 * 
 * @param volume Volume (0-65535)
 */
void Event::eventBreath(int volume) {
    if (volume > scalevolume)
        volume = scalevolume;
    if (volume == playingVolume)
        return;
    
    synth.changeVolume(volume);
    playingVolume = volume;
}

/**
 * @brief Handle new pitch bend to MIDI devices from -8000 to +8000
 * 
 * @param bend Bend (+/-8000)
 */
void Event::setPitchBend(int bend) {
    static int lastBend = 0;
    if (bend == lastBend) return;
    lastBend = bend;
    synth.setBend(bend);
    int pb = bend + 0x2000;
    Midi::midiPitchBend(pb);
}

/**
 * @brief Send portamento (MIDI only)
 * 
 * @param port 1 is on; 0 is off
 */
void Event::setPortamento(int port) {
    Midi::midiControl(0x41, port?127:0, settings.channel); // portamento on
}

/**
 * @brief Send ModWheel changes
 * 
 * @param cc Control code
 * @param value Data value (0-127)
 */
void Event::eventCC(int cc, int value) {
    if (value < 0) value = 0;
    if (value > 127) value = 127;
    // if (cc == settings.midiCC_accel_rotleft) {
    //     synth.setModulateVibrato(value);
    // }
    Midi::midiControl(cc, value, settings.channel);
}

/**
 * @brief Send effect changes (CC, pitch, AT)
 * 
 * @param value  Strength (0.0 - 1.0)
 * @param prev Pointer to previous values store
 */
void Event::eventEffect(int cc, float value, float *prev) {
    if (value < 0.0F) value = 0.0F;
    else if (value > 1.0F) value = 1.0F;
    if (value == *prev) return;
    *prev = value;
    switch(cc) {
        case -1: // pitch up
            setPitchBend(8191.0F * value);
            break;
        case -2: // pitch down
            setPitchBend(-8191.0F * value);
            break;
        case 0:
            Midi::midiAfterTouch(127.0F * value, settings.channel);
            break;
        case -3:
            break;
        default:
            eventCC(cc, 127.0F * value);
            break;
    }
}

/**
 * @brief Handle NoteOff
 */
void Event::eventNoteOff() {
#ifdef LED_PIN
    digitalWrite(LED_PIN, LOW);
#endif
    
    // silence internal synthesizer
    synth.stopNote(playingNote, 0);
    
    eventBreath(0);
    eventSendVolume(0);
    Midi::midiNoteOff(playingNote, playingVolume, settings.channel);

    midi_buffer.add(1000, playingNote);
    Midi::midiFlush();
    
    playingNote = -1;
}

/**
 * @brief Handle NoteOn
 * 
 * @param note MIDI note number
 * @param volume Volume (0-65535)
 */
void Event::eventNoteOn(int note, int volume) {
#ifdef LED_PIN
    digitalWrite(LED_PIN, HIGH);
#endif
    
    synth.startNote(note, volume, 1);
    Accelerometer::newNote();

    if (overrideControl) {
        playingNote = note;
        return;
    }
    
    if (playingNote > 0) {
        Midi::midiNoteOff(playingNote, 0, settings.channel); // just in case, turn prev note off
    }

    int midivolume = ((volume >> volume2midi) + 1);
    if (midivolume > 0x7F) midivolume = 0x7F; // from 10-bit to 7-bit

    eventSendVolume(volume);

    if (settings.midiNoteOnVol) {
        Midi::midiNoteOn(note, settings.midiNoteOnVol, settings.channel, midivolume);    
    }
    else {
        Midi::midiNoteOn(note, midivolume, settings.channel, midivolume);
    }

    Midi::midiFlush();
    
    playingNote = note;
}

/**
 * @brief Send NoteOff/NoteOn during note change
 * 
 * @param note MIDI note  number
 * @param volume Volume (0-65535)
 */
void Event::eventNoteChange(int note, int volume) {
#ifdef LED_PIN
    digitalWrite(LED_PIN, LOW);
#endif
    
#ifdef DEBUGEVENT
    Serial.print("eventNoteChange ");
    Serial.print(note);
    Serial.print(" ");
    Serial.println(volume);
#endif

    // change internal synthesizer
    synth.startNote(note, volume, 0);
    Accelerometer::newNote();

    int midivolume = (volume >> volume2midi) & 0x7F; // from 10-bit to 7-bit
    
    midi_buffer.add(1000, playingNote);
    if (settings.midiNoteOnVol) {
        Midi::midiNoteOn(note, settings.midiNoteOnVol, settings.channel, midivolume);
        Midi::midiNoteOff(playingNote, settings.midiNoteOnVol, settings.channel);
    }
    else {
        Midi::midiNoteOn(note, midivolume, settings.channel, midivolume);
        Midi::midiNoteOff(playingNote, midivolume, settings.channel);   
    }
    // eventSendVolume(midivolume, 1);
    Midi::midiFlush();
    
    playingNote = note;
    
    // AudioNoInterrupts();
    // Display::update();
    // AudioInterrupts();
}

/**
 * @brief Called periodically to send latest volume if it has changed.
 * 
 */
void Event::processMidiBreath() {
    static int lastVolumeSent = 0;
    if (lastVolumeSent != playingVolume) {
        Event::eventSendVolume(playingVolume);
        lastVolumeSent = playingVolume;
    }
}

/**
 * @brief Process slow events, like sending duplicate NoteOffs.
 * 
 */
void Event::processMidiSlow() {
    int data;
    if (midi_buffer.next(&data)) {
        if (data && data != playingNote) {
            Midi::midiNoteOff(data, 0, settings.channel);
        }
    }
}

/**
 * @brief Send MIDI alive message.
 * 
 */
void Event::processMidiAlive() {
    if (settings.midiActiveSense) {
        Midi::midiActive();
    }
}