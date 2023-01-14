#include "firmware.h"

#include "Manage.h"

#include "Event.h"
#include "Settings.h"
#include "Power.h"
#include "Diagnostic.h"
#include "Display.h"
#include "Boot.h"

#include "Lights.h"

#include "ReadFingering.h"

#include "Dispatch.h"

#include "SoundSynthesis.h"
#include "OutputMode.h"

#include "Touch.h"
#include "Pressure.h"

#include "vdebug.h"

extern int dispatchMidiId;

extern char cpu_guid[];
extern const char *compile_time;

// last control message received (for debugging)
int lastMidiControl = 0;

// used to download/upload files to SD card
MidiFileDevice Manage::file;

/******
 * 
 * The code section below is for communicating with the management software using MIDI
 *
 */

void Manage::sendSerial(MidiSerial *serial) {
    MidiChunk chunk(serial);
    int len, ret;
    unsigned char *data;
    while (1) {
        ret = chunk.loadNextChunk();
        data = chunk.getChunk(&len);
#ifdef USBMIDI
        if (Midi::isMidiReceiving()) usbMIDI.sendSysEx(len, data, true);
#ifdef DEBUGPC
        else {
            Serial.println("USB MIDI disabled");
        }
#endif
#endif
        if (ret == 0)
            break;

        delay(100); // Needed for slow receivers
    }
}

/**
 * @brief Send all the active fingering information to the management app
 * 
 */
void Manage::sendFingeringToMIDI() {
    MidiSerial ser;
    settings.getFingerSerialize(&ser);
    ser.setType(0x11);
    sendSerial(&ser);
}

/**
 * @brief Read fingering data from host
 * 
 * @param ser Message containing data
 */
void Manage::readFingeringFromMIDI(MidiSerial &ser) {
    settings.setFingerSerialize(&ser);
}

/**
 * @brief Send synth settings to the connected host
 * 
 * @param inp 
 */
void Manage::sendSynthSettings(MidiSerial &inp) {
    if (inp.getLength()==0) {
        MidiSerial ser;
        settings.soundList[settings.currentInstrument].getSerialize(&ser);
        ser.setType(0x1E);
        sendSerial(&ser);
    }
    else {
        int num = inp.getByte();
        MidiSerial ser;
        settings.soundList[num].getSerialize(&ser);
        ser.setType(0x1E);
        sendSerial(&ser);
    }
}

/**
 * @brief Parse retrieved synth settings
 * 
 * @param ser 
 */
void Manage::readSynthSettings(MidiSerial &ser) {
    settings.soundList[settings.currentInstrument].setSerialize(&ser);
}

const char *Manage::getCPUModel() {
    static char x[16];
    int pins = SIM_SDID & 0b1111;
    switch(pins) {
        case 0b0101: return "64";
        case 0b0110: return "80";
        case 0b0111: return "81";
        case 0b1000: return "100";
        case 0b1001: return "121";
        case 0b1010: return "144";
    }
    sprintf(x, "%d", pins);
    return x;

#if 0
    int fam = (SIM_SDID & 0b1110000) >>4;
    int rev = (SIM_SDID & 0b1111000000000000) >> 12;
    int flash = (SIM_FCFG1 >> 24) & 0b1111);
    char *s0 = "UNK";
    int s1 = 0;
    switch(fam) {
        case 0b000: s0 = "K10"; break;
        case 0b001: s0 = "K20"; break;
        case 0b010: s0 = "K30"; break;
        case 0b100: s0 = "K40"; break;
        case 0b110: s0 = "K50"; break;
        case 0b111: s0 = "K51"; break;
    }
    switch(pins) {
        case 0b0110: s1 = 80; break;
        case 0b0111: s1 = 81; break;
        case 0b1001: s1 = 121; break;
        case 0b1010: s1 = 144; break;
    }
    switch(flash) {
        case 0b0111: s2 = 128; break;
        case 0b1001: s2 = 256; break;
        case 0b1011: s2 = 512; break;
    }
#endif
}

void Manage::sendInformation() {
    MidiSerial ser;
    ser.setType(0x1A);
    ser.addString("Vindor");
    char buf[256];
    sprintf(buf, "Vindor %s", MODEL);
    ser.addString(buf);
    ser.addWord(REVISION);
    ser.addWord(PLATFORM_VERSION);
    ser.addWord(FIRMWARE_VERSION);
    ser.addString(cpu_guid);
    ser.addString(compile_time);
    ser.addString(getCPUModel());
    ser.addByte(0x00); // terminate
    sendSerial(&ser);
}

void Manage::sendMidiStatus(int statusid) {
    MidiSerial ser;
    ser.setType(0x18);
    ser.addByte(statusid & 0x7F);
    sendSerial(&ser);
}

void Manage::addKeyValue(MidiSerial &ser, const char * string, int value) {
    ser.addByte(0x11);
    ser.addString(string);
    ser.addWord(value);
}

void Manage::addKeyFloat(MidiSerial &ser, const char * string, float value) {
    ser.addByte(0x11);
    ser.addString(string);
    ser.addWord(value * 10);
}

void Manage::addKeyStr(MidiSerial &ser, const char * string, const char *value) {
    ser.addByte(0x13);
    ser.addString(string);
    ser.addString(value);
}

void Manage::sendSettings() {
    MidiSerial ser;
    ser.setType(0x1C);

#ifdef USE_WIFI
    ser.addWord(31); // number of items
#else
    ser.addWord(29); // number of items
#endif

    addKeyValue(ser, "BASENOTE", settings.noteAdj + 32);
    addKeyValue(ser, "BASEOCT", settings.octAdj + 16);
    addKeyValue(ser, "OCTMODE", settings.octMode);

    addKeyValue(ser, "PRESFAC", settings.pressurefactor * 10.0);
    addKeyValue(ser, "KEYDELAY", settings.fingerChangeDelay);

    addKeyValue(ser, "SYNTHACT", settings.synthActive);
    addKeyValue(ser, "CODECVOL", settings.hpVolume * 100.0);
    addKeyValue(ser, "BASEA4", settings.baseFreqA4 * 10.0);
    addKeyValue(ser, "USBMODE", settings.usbMode + 1);
    addKeyValue(ser, "USEBEND", settings.useBend + 1);

    addKeyValue(ser, "INST", settings.currentInstrument);
    addKeyValue(ser, "TRANSP", settings.transposeSetting);

    addKeyValue(ser, "MIDIVOL", settings.useVolume);
    addKeyValue(ser, "MIDICC", settings.midiCC);

    addKeyValue(ser, "MIDIXCCWU", settings.midiCC_accel_ccw_up + 5);
    addKeyValue(ser, "MIDIXCCWD", settings.midiCC_accel_ccw_down + 5);
    addKeyValue(ser, "MIDIXCW", settings.midiCC_accel_cw + 5);
    addKeyValue(ser, "MIDIXCCW", settings.midiCC_accel_ccw + 5);
    addKeyValue(ser, "MIDIXCWU", settings.midiCC_accel_tilt_up + 5);
    addKeyValue(ser, "MIDIXCWD", settings.midiCC_accel_tilt_down + 5);

    addKeyValue(ser, "CHANNEL", settings.channel);
    addKeyValue(ser, "MIDIRATE", settings.midiRate);
    addKeyValue(ser, "MIDIHIRES", settings.midiHires);
    addKeyValue(ser, "MIDIREPEAT", settings.midiNoteOffRepeat + 1);
    addKeyValue(ser, "MIDIBEND", settings.midiBendFactor);

    addKeyValue(ser, "GESTURES", settings.useGestures);

    addKeyValue(ser, "NOTATION", settings.notation);
    addKeyValue(ser, "IDLESEC", settings.idleSleep_ms / 1000);
    addKeyValue(ser, "KEYLAYOUT", settings.keyLayout);

    addKeyFloat(ser, "VOLT", Power::volt());

#ifdef USE_WIFI
    addKeyValue(ser, "WIFIHW", midi_wifi.isPresent);
    addKeyValue(ser, "WIFIEN", settings.enableWifi);
    addKeyStr(ser, "WIFISSID", settings.ssid);
    addKeyStr(ser, "WIFIPASS", settings.pass);
#else
    addKeyValue(ser, "WIFIHW", 0);
    addKeyValue(ser, "WIFIEN", 0);
#endif

    ser.addByte(0); // the end!

    sendSerial(&ser);
}

void Manage::readSettings(MidiSerial &ser) {
    char key[16];
    int value;
    // Serial.println("READ SETTINGS");
    int num = ser.getWord();
    // Serial.print("NUM "); Serial.println(num);
    for (int i = 0; i < num; i++) {
        int datatype = ser.getByte();
        // Serial.print("DTYPE "); Serial.println(datatype, HEX);
        if (datatype == 0x10) {
            sendMidiStatus(0x90);
            break;
        }
        else if (datatype == 0x00) {
            sendMidiStatus(0x91);
            break;
        }
        else if (datatype == 0x11) {
            ser.getString(key);
            // Serial.println(key);
            // Serial.flush();
            // Serial.println(ser.getLength());
            // Serial.flush();
            value = ser.getWord();
            // Serial.println(value);
            // Serial.flush();
            if (strcmp(key, "RESETBOOT") == 0)
                settings.resetOnBoot = value;
            else if (strcmp(key, "BASENOTE") == 0)
                settings.noteAdj = value - 32;
            else if (strcmp(key, "BASEOCT") == 0)
                settings.octAdj = value - 16;
            else if (strcmp(key, "OCTMODE") == 0)
                settings.octMode = value;

            else if (strcmp(key, "PRESFAC") == 0) {
                settings.pressurefactor = ((float)value) / 10.0;
                pressure.buildPressureMap();
            }
            else if (strcmp(key, "KEYDELAY") == 0)
                settings.fingerChangeDelay = value;

            else if (strcmp(key, "SYNTHACT") == 0)
                settings.synthActive = value;
            else if (strcmp(key, "CODECVOL") == 0)
                settings.hpVolume = value / 100.0;
            else if (strcmp(key, "BASEA4") == 0)
                settings.baseFreqA4 = value / 10.0;
            else if (strcmp(key, "USBMODE") == 0)
                settings.usbMode = value - 1;
            else if (strcmp(key, "USEBEND") == 0)
                settings.useBend = value - 1;

            else if (strcmp(key, "INST") == 0)
                setinstrument(value);
            else if (strcmp(key, "TRANSP") == 0)
                settings.transposeSetting = value;

            else if (strcmp(key, "MIDIVOL") == 0)
                settings.setVolumeMessage(value);
            else if (strcmp(key, "MIDICC") == 0)
                settings.midiCC = value;
            else if (strcmp(key, "MIDIXCCWU") == 0)
                settings.midiCC_accel_ccw_up = value - 5;
            else if (strcmp(key, "MIDIXCCWD") == 0)
                settings.midiCC_accel_ccw_down = value - 5;
            else if (strcmp(key, "MIDIXCW") == 0)
                settings.midiCC_accel_cw = value - 5;
            else if (strcmp(key, "MIDIXCCW") == 0)
                settings.midiCC_accel_ccw = value - 5;
            else if (strcmp(key, "MIDIXCWU") == 0)
                settings.midiCC_accel_tilt_up = value - 5;
            else if (strcmp(key, "MIDIXCWD") == 0)
                settings.midiCC_accel_tilt_down = value - 5;
            else if (strcmp(key, "CHANNEL") == 0)
                settings.channel = value;
            else if (strcmp(key, "MIDIRATE") == 0)
                Midi::midiSetRate(settings.midiRate);
            else if (strcmp(key, "MIDIHIRES") == 0)
                settings.midiHires = value;
            else if (strcmp(key, "MIDIREPEAT") == 0)
                settings.midiNoteOffRepeat = value - 1;
            else if (strcmp(key, "MIDIBEND") == 0)
                settings.midiBendFactor = value;

            else if (strcmp(key, "GESTURES") == 0)
                settings.useGestures = value;

            else if (strcmp(key, "NOTATION") == 0)
                settings.notation = value;
            else if (strcmp(key, "IDLESEC") == 0)
                settings.idleSleep_ms = value * 1000;
            else if (strcmp(key, "KEYLAYOUT") == 0)
                fingering.init(value);

#ifdef USE_WIFI
            else if (strcmp(key, "WIFIEN") == 0)
                settings.enableWifi = 1;
#endif
        }
        else if (datatype == 0x13) {
            char str[255];
            ser.getString(key);
            ser.getString(str);
#ifdef USE_WIFI
            if (strcmp(key, "WIFISSID") == 0)
                strcpy(settings.ssid, str);
            else if (strcmp(key, "WIFIPASS") == 0)
                strcpy(settings.pass, str);
#endif
        }
    }
}

void Manage::sendWifi() {
    MidiSerial ser;
    ser.setType(0x1D);
#ifdef USE_WIFI
    ser.addByte(settings.enableWifi);
    ser.addString(settings.ssid);
    ser.addString(settings.pass);
#else
    ser.addByte(0);
    ser.addString("-");
    ser.addString("-");
#endif
    ser.addByte(settings.usbMode); // terminate
    sendSerial(&ser);
    // Serial.print("SEND SSID:");
    // Serial.println(settings.ssid);
}

void Manage::readWifi(MidiSerial &ser) {
#ifdef USE_WIFI
    settings.enableWifi = ser.getByte();
    ser.getString(settings.ssid);
    ser.getString(settings.pass);
#else
    ser.getByte();
    char x[32];
    ser.getString(x);
    ser.getString(x);
#endif
    settings.usbMode = ser.getByte();
    // Serial.print("READ SSID:");
    // Serial.println(settings.ssid);
}

void Manage::pingMidi(int ditem) {
    static unsigned char data[] = {0xF0, 0x22, 0x00, 0xF7};
    data[2] = ditem;
    usbMIDI.sendSysEx(sizeof(data), data, true);
}


#ifdef USBMIDI
MidiSerial store;
#endif

/**
 * @brief Get incoming MIDI messages, check it is for us, and direct to the appropriate code
 *
 */
void Manage::processMIDIControl() {
#ifdef USBMIDI
    byte* p = usbMIDI.getSysExArray();

    // if it's the right manufacturer code
    if(p[1] == MIDI_MFG1 && p[2] == MIDI_MFG2 && p[3] == MIDI_MFG3) {
        if (p[4] == MIDI_MSG_FILE) {
            file.processMessage(p, 0);
        }

        else if (p[4]==0x17 && p[5]==0x53) { // special reboot to bootloader
            rebootBootloader();
        }

        else if (p[4]==0x17 && p[5]==0x54) { // reset factory  settings
            settings.factorySettings();
        }

        else if (p[4]==0x17 && p[5]==0x34) { // enter diagnostic mode
            Diagnostic::enter();
        }

        else if (p[4]==0x17 && p[5]==0x36) { // ping result
            pingMidi(p[6]);
        }

        else if (p[4]==0x17 && p[5]==0x41) { // internal parameters
            int temp;
            switch(p[6]) {
                case 1:
                    temp = p[7];
                    temp += p[8] << 7;
                    temp += p[9] << 14;
                    dispatch.trigger(dispatchMidiId, temp);
                    break;
            }
        }

        else if (p[4]==0x17 && p[5]==0x40) { // state of synthesizer
#ifdef USE_FILTER_SYNTH
            int temp;
            float tempf;
            SynthSettings *sound = &settings.soundList[settings.currentInstrument];
            switch(p[6]) {
                case 1:
                    sound->u.analog.filterFreq = p[7] * 100.0;
                    break;
                case 17:
                    temp = p[7];
                    temp += p[8] << 7;
                    temp += p[9] << 14;
                    sound->u.analog.filterFreq = temp;
                    break;
                case 2:
                    sound->u.analog.filterResonance = (float)p[7] / 127.0 * 10.0;
                    break;
                case 3:
                    sound->u.analog.fil_vol[p[7]] = (float)p[8] / 127.0;
                    break;
                case 4:
                    sound->u.analog.fil_freq[p[7]] = p[8];
                    if (p[9] != 0) {
                        sound->u.analog.fil_freq[p[7]] += 1.0 / (float)p[9];
                    }
                    break;
                case 5:
                    tempf = MidiSerial::formatMidi2Float(&p[8]);
                    sound->u.analog.fil_freq_bias[p[7]] = tempf;
                    break;
                case 6:
                    sound->u.analog.fil_wave[p[7]] = p[8];
                    break;
                case 7:
                    temp = p[7];
                    temp += p[8] << 7;
                    temp += p[9] << 14;
                    sound->u.analog.fil_lfo_freq = temp / 100.0;
                    break;
                case 8:
                    temp = p[7];
                    temp += p[8] << 7;
                    temp += p[9] << 14;
                    sound->u.analog.fil_lfo_delay = temp;
                    break;
                case 9:
                    sound->u.analog.fil_lfo_mix[p[7]] = (float)p[8] / 127.0;
                    break;
                case 11:
                    if (p[7] <= 0) p[7] = 1;
                    sound->u.analog.fil_chorus1_num = p[7];
                    break;
                case 12:
                    sound->u.analog.fil_vol_noise = (float)p[7] / 127.0;
                    break;
                case 13:
                    sound->u.analog.fil_lfo_wave = p[7];
                    break;
                case 14:
                    sound->u.analog.playAttack = p[7];
                    break;
                case 15:
                    setinstrument(p[7]);
                    break;
                case 16:
                    sound->midiNumber = p[7] + (p[8] << 7);
                    break;
                case 18:
                    sound->u.physical.tone = p[7];
                    break;
                case 19:
                    sound->u.physical.smoothness = p[7];
                    break;
                case 20:
                    sound->u.physical.echo = p[7];
                    break;
                case 21:
                    sound->synth_use = p[7];
                    sound->loadDefaults_synth();
                    break;
                case 22:  // turn speaker on/off
                    currentOutput.setSpeaker(p[7]);
                    break;
                case 23:
                    sound->u.physical.table = p[7] - 1;
                    break;
                // case 24:
                //     sound->u.analog.fil_echo_num = p[7];
                //     break;
            }
            synth.resetFilter();
#endif
        }
        else {
            MidiChunk chunk(&store);
            int ret = chunk.appendNextChunk(p);
            if (ret==1) {
                return; // need more data
            }

            int msg = store.getType();
            lastMidiControl = msg; // DEBUG

            if (msg == 0x10) { // define fingering
                readFingeringFromMIDI(store);
            } else if (msg == 0x12) { // request fingering
                sendFingeringToMIDI();
            } else if (msg == 0x17) {
                sendMidiStatus(0);
            } else if (msg == 0x19) {
                sendInformation();
            } else if (msg == 0x1B) {
                sendSettings();
            } else if (msg == 0x1C) {
                readSettings(store);
                settings.saveSettings();
            } else if (msg == 0x1D) {
                sendWifi();
            } else if (msg == 0x1E) {
                readWifi(store);
            } else if (msg == 0x20) {
                rebootBootloader();
            } else if (msg == 0x21) {
                pingMidi();
            } else if (msg == 0x22) {
                sendSynthSettings(store);
            } else if (msg == 0x23) {
                readSynthSettings(store);
            } else if (msg == 0x24) {
                settings.saveSettings();
            } else if (msg == 0x25) {
                settings.factorySettings();
            }
            // store.init();
        }
    }
#endif
}

void Manage::readMIDI() {
#ifdef USBMIDI
    while (usbMIDI.read()) {
        int mtype = usbMIDI.getType();

        switch(mtype) {
            case usbMIDI.NoteOff: // noteOff
                Event::eventNoteOff();
                Event::overrideControl = 0;
                break;
            case usbMIDI.NoteOn: // noteOn
            {
                Event::overrideControl = 1;
                int n = usbMIDI.getData1();
                int v = usbMIDI.getData2() << volume2midi;
                Event::eventNoteOn(n, v);
            }
                break;
            case usbMIDI.ProgramChange:
            {
                int midi_inst = usbMIDI.getData1();
                for(int i=0; i<numberOfSounds; i++) {
                    if (settings.soundList[i].midiNumber == midi_inst) {
                        setinstrument(i);
                    }
                }
            }
                break;
            case usbMIDI.PitchBend: // pitchbend
            {
                int bend = (usbMIDI.getData2() << 7) +  usbMIDI.getData1() - 0x2000;
                Event::setPitchBend(bend / settings.midiBendFactor);
            }
                break;
            case usbMIDI.ControlChange:
            {
                int c = usbMIDI.getData1();
                int v = usbMIDI.getData2();
                switch(c) {
                    case 1: // mod
                    case 2: // bc
                    case 7: // vol
                    case 11:// expr
                        Event::eventBreath(v << volume2midi);
                        break;
                }
            }
                break;
            case usbMIDI.SystemExclusive: // sysEx
                processMIDIControl();
                break;
        }
    }
#endif
}

/***********
 * 
 * Code for reading/writing files from  host
 */

void HostFile::open(const char *fn, const char *fmode) {
    char x[256];
    x[0] = 0;
    strcat(x, fmode);
    strcat(x, ":");
    strcat(x, fn);
    Serial.write(0x01);
    Serial.write(0x01);
    Serial.write(strlen(x));
    Serial.write(x, strlen(x));
    Serial.read(); // status
}

void HostFile::write(const uint8_t *data, int length) {
    Serial.write(0x01);
    Serial.write(0x04);
    Serial.write((uint8_t)length);
    Serial.write(data, length);
    Serial.read(); // status
}

int HostFile::read(uint8_t *buffer, int maxlen) {
    Serial.write(0x01);
    Serial.write(0x03);
    Serial.write((uint8_t)maxlen);
    Serial.write((uint8_t)0);
    int l = Serial.read();
    if (l>maxlen) l=maxlen;
    for(int i=0; i<l; i++) {
        buffer[i] = Serial.read();
    }
    return l;
}

void HostFile::close() {
    Serial.write(0x01);
    Serial.write(0x02);
    Serial.write(0x00);
    Serial.write(0x00);
    Serial.read(); // status
}

SimpleProfile::SimpleProfile(const char *fxn) {
    name = fxn;
    stime = 0;
    tcount = 0;
    append(this);
}

void SimpleProfile::start() {
    stime = micros();
}

void SimpleProfile::end() {
    uint32_t etime = micros();
    if (etime >= stime) tcount += etime - stime;
    else tcount += etime + (0xFFFFFF - stime);
}

void SimpleProfile::count() {
    callcount++;
}

void SimpleProfile::append(SimpleProfile *p) {
    list[idx++] = p;
}

void SimpleProfile::dump(){
    Serial.println("Function,Count,Time");
    for(int i=0; i<idx; i++) {
        Serial.print(list[i]->name);
        Serial.print(",");
        Serial.print(list[i]->callcount);
        Serial.print(",");
        Serial.print(list[i]->tcount);
        Serial.println();
    }
}
