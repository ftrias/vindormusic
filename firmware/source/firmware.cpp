/**
 * @file firmware.cpp
 * @author Fernando Trias
 * @brief 
 * @version 0.1
 * @date 2013
 * 
 * @copyright 2013-2021 by Fernando Trias
 * 
 */

// #include <i2c_t3.h>
#include "MyWire.h"
#include <EEPROM.h>
#ifdef USE_SDCARD
#include "SdLib.h"
#endif
#include <SPI.h>
#include <Audio.h>

#include "firmware.h"

#ifdef USBMIDI
#include <usb_desc.h>
#include <usb_dev.h>
#endif

#ifdef DEBUGTIMING
int time_slot;
long time_slot_millis[32];
const char *time_slot_name[32];
#endif

void power_off_device();

unsigned char *UID_REGISTER_ADDR = ((unsigned char *)0x40048054);
const int UID_REGISTER_LEN=16;
char cpu_guid[UID_REGISTER_LEN*2 + 1];

#ifdef SECURE_FLASH
__attribute__ ((section(".flashconfig"), used))
const uint8_t flashconfigbytes[16] = {
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, // No backdoor
    0xFF, 0xFF, 0xFF, 0xFF, // FPROT0-3, no protected pages
    // Backdoor disabled, Mass Erase enabled, NXP denied, Flash Secure
    0b01010111,
    0xF9,
    0xFF, 0xFF
};
#else
__attribute__ ((section(".flashconfig"), used))
const uint8_t flashconfigbytes[16] = {
    0x1F, 0xC2, 0x13, 0x4B, 0x83, 0x92, 0xAF, 0x29, // Backdoor
    0xFF, 0xFF, 0xFF, 0xFF, // FPROT0-3, no protected pages
    // Backdoor enabled, Mass Erase enabled, NXP access, Flash Unsecure
    0b10011110,
    0xF9,
    0xFF, 0xFF
};
#endif

#ifdef DEBUGDEV
unsigned int loopcount = 0;
#endif

int dispatchMidiId = -1;

enum InstrumentState {
    STATE_SILENCE,
    STATE_PRESS_DETECT,
    STATE_PLAY_START,
    STATE_PLAYING,
    STATE_PLAY_CHANGE,
    STATE_RELEASE_DETECT,
};

// used to stabalize air pressure for initial Velocity
const int PRESS_DEBOUNCE_MS = 10;
static InstrumentState state = STATE_SILENCE;


#include "fdebug.h"

#include "Constants.h"

#include "Settings.h"

SettingsData settings;

#include "LipPressure.h"
#include "Touch.h"
#include "SoundSynthesis.h"
// #include "sound.h"

#include "Boot.h"
#include "Lights.h"

#include "WatchDog.h"
#include "Dispatch.h"
#include "Power.h"
#include "ReadFingering.h"
#include "Pressure.h"
#include "OutputMode.h"
#include "Event.h"
#include "PlugState.h"
#include "Display.h"
#include "Diagnostic.h"
#include "Manage.h"

#ifdef USE_ACCELEROMETER
#include "Accelerometer.h"
#endif

#ifdef SAY_NUMBERS
#include "Speech.h"
#endif

#ifdef GPROF
#include "TeensyGProf.h"
#endif

#include "vdebug.h"

#ifdef SMALL_FOOTPRINT
#define GLOBAL_CONST const
#else
#define GLOBAL_CONST
#endif

const char *compile_time = __DATE__ " " __TIME__;

const int STARTNOTEPRESSURE = adc_read_scale * 0.003;  // minimum breath before note starts
const int STOPNOTEPRESSURE = 0;  // breath before note stops
const int WAKENOTEPRESSURE = adc_read_scale * 0.001;  // breath to turn on speaker

#ifdef SIMULATE
int simulate_on = 1;
#endif

/**
 * @brief Note processing parameters
 * 
 */

// state parameters

int playingNote = 0;
int playingOctave = 0;
int breathOnly = 0;
// int currentInstrument = -1;

int effect_flange = 0;
int effect_chorus = 0;

Lights led;
SoundSynthesis synth;
Pressure pressure;
OutputMode currentOutput;
WatchDog watchdog;
Dispatch dispatch;
ReadFingering fingering;
PlugState currentPlugState;

//
// Section for settings
//

void fatal_system_error(int code) {
    led.alloff();
#ifdef Uart
    Uart.begin(9600);
#endif
    while(1) {
#ifdef Uart
        Uart.println(code);
#endif
        for(int i=0; i<code; i++) {
            led.error(1);
            delay(200);
            led.error(0);
            delay(300);
        }
        delay(3000);
    }
}

void showMessage(const char *msg) {
#ifdef USE_DISPLAY
    Display::drawMessage(msg);
#endif
}

void power_off_device() {
    Power::setPeripherals(0);
    currentOutput.powerOff();
    led.alloff();
    synth.disable();
#ifdef USE_DISPLAY
    Display::displayOff();
#endif
    delay(100); // give time for powerdowns
#ifdef USE_SLEEP_MODE
    Power::powerDownCPU();
    delay(1000); // should never get here
    reboot();
#else
    #warn "Power down not enabled"
#endif
}

/************
 * 
 * Silence and idle time handling
 * 
 */

#ifdef SILENCESHUTDOWN

const int silenceStartTime = 10 * 1000; // ms before for simple power reduction
const int silenceQuietTime = 2 * 1000;

enum SilenceState {
    ACTIVE=0, QUIET=1, SILENT=2, IDLE=3
};

int silenceTime = 0;
SilenceState silenceState = ACTIVE;

void silenceInit() {
    silenceState = ACTIVE;
    silenceTime = millis();
}

void silenceEnd() {
    silenceTime = millis();
    if (silenceState == SILENT) {
        currentOutput.enableSilence(0);
    }
    silenceState = ACTIVE;
}

void silenceCheck() {
    if (silenceState==ACTIVE) {
        if ((millis() - silenceTime > silenceQuietTime)) {
            // TODO shut down simple devices
            silenceState = QUIET;
        }
        return;
    }
    if (silenceState==QUIET) {
        if ((millis() - silenceTime > silenceStartTime)) {
            silenceState = SILENT;
            currentOutput.enableSilence(1);
        }
        return;
    }
    if (silenceState==SILENT) {
        if  ((settings.idleSleep_ms) && (millis() - silenceTime > settings.idleSleep_ms)) { // long time inactive
            silenceState = IDLE;
            showMessage("IDLE OFF");
            led.error(1);
            delay(1000);
            power_off_device(); // doesn't return unless grave error
            // SHOULD NEVER GET HERE
            reboot();
        }
        return;
    }
}
#else
void silenceInit() {}
void silenceEnd() {}
void silenceCheck() {}
#endif


/************
 * 
 * Set a new instrument and reset the sound generation module
 * 
 */

void setinstrument(int inst) {
    // if (inst >= staticInstrumentNum) return;
    Event::eventNoteOff();
    bool rx = synth.stopWait();
    if (! rx) return; // couldn't stop play! ERROR!
    int midinum = synth.setInstrument(inst);
    Midi::midiProgramChange(midinum, settings.channel);
}

#ifdef SIMULATE

void compare_ram() {
    Serial.println("!!CHANGE!!");
    const int sz = 256;
    static uint32_t buffer[sz];
    static int first = 1;
    if (first) {first=0; return;}
    uint32_t *p = (uint32_t*)0x0000000020000dd0;
    for(int i=0;i<sz;i++) {
        if (buffer[i] != p[i]) {
            Serial.print((long)(p+i), HEX);
            Serial.print(" ");
            Serial.print(buffer[i]);
            Serial.print(" ");
            Serial.print(p[i]);
            Serial.println();
        }
    }
    for(int i=0;i<sz;i++) buffer[i] = p[i];
}

void dump_ram() {
    Serial.println("!!RAM!!");
    uint8_t *p = (uint8_t*)0x0000000020000dd0;
    for(int i=0;i<20;i++) {
        Serial.print((long)p, HEX);
        for(int j=0;j<16;j++) {
            Serial.print(" ");
            if (*p<16) Serial.print("0");
            Serial.print(*p, HEX);
            p++;
        }
        Serial.println();
    }
}

void dump_core() {
    Serial.println("!!RAM DUMP BEGIN!!");
    uint8_t *p         = (uint8_t*)0x1FFF8000;
    const uint8_t *end = (uint8_t*)0x1fffffff;
    int count = 0;
    while(p <= end) {
        if (count % 16 == 0) {
            Serial.println();
            Serial.print((long)p, HEX);
            Serial.flush();
            delay(5);
        }
        Serial.print(" ");
        if (*p<16) Serial.print("0");
        Serial.print(*p, HEX);
        p++;
        count++;
    }
    Serial.println();
    Serial.println("!!RAM DUMP END!!");
    Serial.flush();
}

void simulate_toggle() {
    simulate_on = simulate_on ? 0 : 1;
    delay(500);
    showMessage(simulate_on?"SIMULATE ON":"SIMULATE OFF");
    delay(500);
}

#endif

/************
 * 
 * i2c failure recovery attempt
 * 
 */

void resetI2CBus()
{
    uint8_t scl = 29;
    uint8_t sda = 30;

    // change pin mux to digital I/O
    pinMode(sda, INPUT);
    digitalWrite(scl,HIGH);
    pinMode(scl,OUTPUT);

    for(uint8_t count=0; digitalRead(sda) == 0 && count < 9; count++)
    {
        digitalWrite(scl,LOW);
        delayMicroseconds(5);       // 10us period == 100kHz
        digitalWrite(scl,HIGH);
        delayMicroseconds(5);
    }

    Wire.begin();
}
 
extern "C" char* sbrk(int incr);
extern char *__brkval;

/**
 * @brief Return amount of free/unallocated RAM
 * 
 * @return int Number of bytes
 */
int freeMemory() {
    // difference between the stack and the heap
    char *stack;
    __asm__ volatile("mov %0, sp" : "=r" (stack) ::);
    return stack - __brkval;
}

// extern "C" void usb_force_reset(int configuration);

/**
 * @brief If SET button is pressed, read the settings change.
 * 
 * @param s Bit representation of buttons
 */
void processSetButton(int s) {
    // unsigned int bar = (s >> 12) & 0b1;
    unsigned int oct = (s >> 11) & 0b11;
    unsigned int left3 = (s >> 8) & 0b111;
    unsigned int left2 = (s >> 5) & 0b11;
    unsigned int right3 = (s >> 2) & 0b111;
    unsigned int right2 = (s) & 0b11;
    unsigned int side1 = (s>>7) & 0b1;

#ifdef DEBUGSET
    Serial.print("SET CMD  buttons=");
    Serial.println(s, BIN);
    Serial.print("left3=b");
    Serial.println(left3, BIN);
    Serial.print("left2=b");
    Serial.println(left2,BIN);
    Serial.print("right3=b");
    Serial.println(right3,BIN);
    Serial.print("right2=b");
    Serial.println(right2,BIN);
#endif

    if (oct && left3 == 0 && left2 == 0 && right3 == 0 && right2 == 0 && side1 == 0) {
        if (oct == 0b10) {
            settings.octAdj++;
            if (settings.octAdj > 3) settings.octAdj = 3;
        } else if (oct == 0b01) {
            settings.octAdj--;
            if (settings.octAdj < -3) settings.octAdj = -3;
        } else {
            settings.octAdj = 0;
        }
#ifdef USE_DISPLAY
        Display::state.setOctave(settings.octAdj);
        Display::update();
#endif
    }

    else if (left3 == 0 && left2 == 0 && right3 == 0 && right2 == 0b10 && side1 == 1) {
        fingering.touchDemoMode = !fingering.touchDemoMode;
    }

    else if (left3 == 0b111 && left2 == 0 && right3 == 0 && right2 == 0b11) {
#ifndef SIMULATE
        showMessage("FACTORY RESET");
        led.blink(3);
        settings.factorySettings();
#endif
    }

    else if (left3 != 0 && left2 == 0 && right3 == 0 && right2 == 0b01) {
        if (left3 == 0b111) {
            breathOnly = 1;
        } else {
            breathOnly = 0;
            if (left3 == 0b011) {
                settings.setVolumeMessage(1); // breath
            }
            else if (left3 == 0b110) {
                settings.setVolumeMessage(5); // mod
            }
            else if (left3 == 0b010) {
                settings.setVolumeMessage(2); // volume
            }
            else if (left3 == 0b100) {
                settings.setVolumeMessage(3); // expr
            }
            else if (left3 == 0b001) {
                settings.setVolumeMessage(4); // aftertouch
            }
        }
    }

#if 0
    else if (left3 == 0 && left2 == 0 && right3 != 0 && right2 == 0b01) {
        switch(right3) {
            case 0b100: fingering.setKeys(0); break;
            case 0b010: fingering.setKeys(1); break;
            case 0b001: fingering.setKeys(2); break;
            case 0b110: fingering.setKeys(3); break;
            case 0b011: fingering.setKeys(4); break;
            case 0b111: fingering.setKeys(5); break;
        }
    }
#endif

    else if (left3 == 0 && left2 != 0 && right3 == 0 && right2 == 0b01) {
        switch(left2) {
            case 0b10: settings.pressurefactor += 0.2; break;
            case 0b01: settings.pressurefactor -= 0.2; break;
            case 0b11: settings.pressurefactor = 1.0; break;
        }
        if (settings.pressurefactor > 10.0) settings.pressurefactor = 10.0;
        if (settings.pressurefactor < 1.0) settings.pressurefactor = 1.0;
        // showDebug("Pr=", settings.pressurefactor * 10.0);
        pressure.buildPressureMap();
    }

    else if (right3 == 0 && right2 == 0) {
        int instnum = -1;
        if (left2 == 0) {
            switch (left3) {
                case 0b100:
                    instnum = 0;
                    break;
                case 0b010:
                    instnum = 1;
                    break;
                case 0b001:
                    instnum = 2;
                    break;
                case 0b110:
                    instnum = 3;
                    break;
                case 0b011:
                    instnum = 4;
                    break;
                case 0b111:
                    instnum = 5;
                    break;
            }
        } else {
            switch (left2) {
                case 0b10:
                    instnum = 6;
                    break;
                case 0b01:
                    instnum = 7;
                    break;
                case 0b11:
                    instnum = 8;
                    break;
            }
        }

        if (instnum >= 0) {
            setinstrument(instnum);
#ifdef USE_DISPLAY
            Display::state.setInstrument(settings.currentInstrument);
            Display::update();
#endif
        }
    }

    // change transposition to C, Bb, Eb or A
    else if (left3 == 0 && left2 == 0 && right3 != 0 && right2 == 0) {
        if (right3 == 0b100)
            settings.noteAdj = 0; // C
        else if (right3 == 0b010)
            settings.noteAdj = -2; // Bb
        else if (right3 == 0b001)
            settings.noteAdj = +3; // Alto Eb
        else if (right3 == 0b110)
            settings.noteAdj = -3; // A
        else if (right3 == 0b011)
            settings.noteAdj = +5; // F
        // settings.noteAdj = -21; // Bari Eb - 2 octaves below Alto Eb
        else if (right3 == 0b111)
            settings.noteAdj = 2; // D

#ifdef USE_DISPLAY
        Display::state.setKey(settings.noteAdj);
        Display::update();
#endif
    }

    // MIDI channel to use
    else if (left3 == 0 && left2 != 0 && right3 == 0 && right2 == 0b01) {
        switch (left2) {
            case 0b10:
                settings.channel = 1;
                break;
            case 0b01:
                settings.channel = 2;
                break;
            case 0b11:
                settings.channel = 3;
                break;
        }
    }

    // Freq for A4
    else if (left3 == 0 && left2 != 0 && right3 == 0 && right2 == 0b11) {
        const int centFactor = 1.0005777895;
        switch (left2) {
                //case 0b10: baseFreqA4 += 1.0F; break;
                //case 0b01: baseFreqA4 -= 1.0F; break;
            case 0b10:
                settings.baseFreqA4 *= centFactor;
                break;
            case 0b01:
                settings.baseFreqA4 /= centFactor;
                break;
            case 0b11:
                settings.baseFreqA4 = 440.0F;
                break;
        }
    }

    // octave Simple/complex toggle
    else if (left3 == 0 && left2 == 0 && right3 == 0 && right2 == 0b11) {
        if (settings.octMode)
            settings.octMode = 0;
        else
            settings.octMode = 1;
#ifdef USE_DISPLAY
        Display::state.setOctaveMode(settings.octMode);
#endif
    }

    // Effects toggle
    else if (left3 != 0 && left2 == 0 && right3 == 0 && right2 == 0b11) {
        if (left3==0b001) {
            settings.useBend = settings.useBend?0:1;
        }
        else if (left3==0b010) {
            if (effect_flange) {
                synth.setEffect(1, 0, 0);
                effect_flange = 0;
            } else {
                synth.setEffect(1, 10, 2);
                effect_flange = 1;
            }

            if (effect_chorus) {
                synth.setEffect(2, 0, 0);
                effect_chorus = 0;
            } else {
                synth.setEffect(2, 3, 2);
                effect_chorus = 1;
            }
        }
    }


#if 0
    // auto-key
    else if (left3 == 0 && left2 != 0 && right3 != 0 && right2 == 0) {
        settings.transposeSetting = (left2 << 3) | right3;
        settings.processTransposeSetting();
    }
#endif

    else if (left3 == 0 && left2 == 0b00 && right3 == 0 && right2 == 0b10) {
        synth.deltaCodecVolume(+0.1);
    }
    else if (left3 == 0 && left2 == 0b00 && right3 == 0 && right2 == 0b01) {
        synth.deltaCodecVolume(-0.1);
    }
    // reset
    else if (left3 == 0 && left2 == 0b10 && right3 == 0 && right2 == 0b10) {
#ifndef SIMULATE
        reboot();
#endif
    }

    settings.saveSettings();
}

/**
 * @brief Called when SET/MENU button is pressed
 * 
 */
void loadSetButton() {
    delay(1); // debounce time?

    if (digitalRead(SET_PIN) == HIGH) { // false alarm, exit
        return;
    }

#ifdef SIMULATE
    // turn off simulation; to turn on use right2=0b01, all else 0
    if (simulate_on) {
        simulate_toggle();
        return;
    }
#endif

    unsigned int s = getButtonsState();

    // wait for release of button, check for debug mode
    elapsedMillis s1;
    while(digitalRead(SET_PIN)==LOW) {
        s = getButtonsState();
        delay(1);
    }

    // Not using shortcuts, so go to menu
    if (settings.enable_shortcuts == 0) {
        unsigned int oct = (s >> 11) & 0b11;
        if (s == 0b10000) {
            settings.enable_shortcuts = 1;
        }
        else if (oct == 0) {
#ifdef USE_DISPLAY
            menu.action(99);
#endif
            return;
        }
    }

    // no button pressed, so activate menu
    if (s == 0) {
#ifdef GPROF
        gprof.end();
#endif

#ifdef USE_DISPLAY
        menu.action(99);
#endif
        return;
    }

#ifdef USE_DISPLAY
    // if menu is active, pass action event to menu
    if (menu.isActive()) {
        menu.action(99);
        return;
    }
#endif

    // If we get here, this is a shortcut and not a menu
    processSetButton(s);
}

#ifdef SIMULATE
void simulateButton() {
    int s = getButtonsState();
    if (s==0b11100000011) { return; } // reset
    if (s==0b1) { return; } // turn off sim
    processSetButton(s);
}
#endif

/**
 * @brief Flag set when menu button is clicked
 * 
 */
int flag_set_button = 0;

/**
 * @brief Called on hardware interrupt (button clicked)
 * 
 */
void set_button_interrupt()
{
    flag_set_button = 1;
}

/**
 * @brief Initialize interrupt on menu button
 * 
 */
void set_button_begin()
{
    pinMode(SET_PIN, INPUT_PULLUP);
    attachInterrupt(SET_PIN, set_button_interrupt, CHANGE);
}

/**
 * Dispatch callbacks
 * 
 */

#ifdef LIP_PRESSURE_PIN
void dispatchReadLip() {
    // PerfTimeUseMidi x(8);
    LipPressure::process();
}
#endif

void dispatchCheckPlug() {
    // PerfTimeUseMidi x(7);
    if (playingNote <= 0) {
        currentPlugState.checkState();
    }
}

void dispatchLed() {
    // PerfTimeUseMidi x(6);
    led.processLoop();
}

void dispatchSynth() {
    // PerfTimeUseMidi x(5);
    synth.runSlowTasks();
}

void dispatchSynthFast() {
    synth.runFastTasks();
}

#ifdef USE_DISPLAY

void dispatchDisplayUpdate() {
#ifdef SILENCESHUTDOWN
    if (playingNote <= 0) {
        Display::update();
    }
    else {
        Display::updateOff();
    }
#else
    Display::update();
#endif
}

#if 0
void dispatchDisplayBlock() {
    int b = Display::displayRenderBlock();
    if (! b) {
        resetI2CBus();
        // Display::drawMessage("Please unplug USB and restart.", 5000, 1);
        Display::displayError("Please unplug USB\nand restart.");
        b = Display::displayRenderBlock();
        led.flash(2);
    }
}
#endif

#endif // USE_DISPLAY

#ifdef SIMULATE

void dispatchDebugPower() {
    int m  =  millis() / 60000; // number of minutes
    int v = Power::getInputVoltage();
    debugOutputDecimal2(0, m, v);
}

#endif

#if 0

void dispatchDebugPing() {
    Midi::midiSendDebug(millis()/1000);
}

void dispatchDebug() {
    Midi::midiSendDebug((millis() - silenceTime)/1000);

    // volatile void *x = malloc(100);
    // Serial.print("Free memory:");Serial.println(freeMemory());
    // Midi::midiSendDebug(pressure.lastPressure);

    // int v = digitalRead(SDCARD_DETECT);
    // Midi::midiSendDebug(v);

    // Midi::midiSendDebug(sum_z2 - sum_z);
    // Midi::midiSendDebug(sum_pitch2 - sum_pitch);

    // int pc = sum_pitch2 - sum_pitch;
    // if (pc < 20 && pc > -20) {
    //     int px = sum_x2 - sum_x;
    //     if (px > 100) {
    //         Midi::midiSendDebug(px);
    //     }
    //     else if (px < -100) {
    //         Midi::midiSendDebug(px);
    //     }
    // }


    // Midi::midiSendDebug(9999);
    // Midi::midiSendDebug(Accelerometer::id);
    // Midi::midiSendDebug(Accelerometer::raw_x);
    // Midi::midiSendDebug(rAccelerometer::aw_y);
    // Midi::midiSendDebug(Accelerometer::raw_z);

    // Midi::midiSendDebug(Accelerometer::isFaceUp());

    // Midi::midiSendDebug(state);

    // Midi::midiSendDebug(silenceTime);

    // Midi::midiSendDebug(80000+ABS(Accelerometer::roll));

    // Midi::midiSendDebug(millis() - idleTime);

    // LipPressure::debug();

    // digitalWrite(SWITCH_HP_MIDI_PIN, HIGH);
    // pinMode(UART_SENSE_PIN_TX, INPUT_PULLUP);
    // delay(1);
    // int v = digitalRead(UART_SENSE_PIN_TX);
    // Midi::midiSendDebug(v);

    // currentPlugState.setSwitch(SWITCH_HEADPHONES);
    // delay(1);

    // if (currentPlugState.switchState == SWITCH_MIDI) {
    //     // pinMode(UART_SENSE_PIN_TX, OUTPUT);
    //     // digitalWrite(UART_SENSE_PIN_TX, LOW);
    //     pinMode(AUDIOPLUG_PIN, INPUT);
    //     delay(1);
    //     int v = analogRead(AUDIOPLUG_PIN);
    //     Midi::midiSendDebug(v);
    // }
    // else {
    //     pinMode(AUDIOPLUG_PIN, INPUT);
    //     delay(1);
    //     int v = analogRead(AUDIOPLUG_PIN);
    //     Midi::midiSendDebug(v);
    // }
}

#endif

/*
 * The width of the CRC calculation and result.
 * Modify the typedef for a 16 or 32-bit CRC standard.
 */
typedef uint16_t crc;

const int CRCWIDTH = (8 * sizeof(crc));
const int TOPBIT = (1 << (CRCWIDTH - 1));

#define POLYNOMIAL 0xD8  /* 11011 followed by 0's */

crc
crcSlow(uint8_t const message[], int nBytes)
{
    crc  remainder = 0;
    /*
     * Perform modulo-2 division, a byte at a time.
     */
    for (int byte = 0; byte < nBytes; ++byte)
    {
        /*
         * Bring the next byte into the remainder.
         */
        remainder ^= (message[byte] << (CRCWIDTH - 8));

        /*
         * Perform modulo-2 division, a bit at a time.
         */
        for (uint8_t bit = 8; bit > 0; --bit)
        {
            /*
             * Try to divide the current data bit.
             */
            if (remainder & TOPBIT)
            {
                remainder = (remainder << 1) ^ POLYNOMIAL;
            }
            else
            {
                remainder = (remainder << 1);
            }
        }
    }

    /*
     * The final remainder is the CRC result.
     */
    return (remainder);

}   /* crcSlow() */

const char hexchar[] = "0123456789ABCDEF";

/**
 * @brief Get the Unique Id object from CPU GUID
 * 
 * @return int 
 */
int getUniqueId() {
    return crcSlow(UID_REGISTER_ADDR, UID_REGISTER_LEN);
    // uint16_t x = SIM_UIDH + SIM_UIDMH + SIM_UIDML + SIM_UIDL;
    // return x;
}

extern "C" void usb_init_descriptor(int configuration);
extern "C" unsigned int usb_default_serialnumber;

/**
 * @brief Called by framework on USB init. Sets the serial number.
 * 
 */
extern "C" void usb_early_hook(void) {
    usb_init_descriptor(SettingsData::getUsbMode());
#ifdef VINDOR_RELEASE
    usb_default_serialnumber = getUniqueId();
#else
    usb_default_serialnumber = 1;
#endif
    char *p = cpu_guid;
    for(int i=0; i < UID_REGISTER_LEN; i++) {
        *(p++) = hexchar[UID_REGISTER_ADDR[i] >> 4];
        *(p++) = hexchar[UID_REGISTER_ADDR[i] & 0xF];
    }
    *p = 0;
}

/***********
 * 
 * Main code
 * 
 */

/**
 * @brief Initialize hardware. Called by framework on boot.
 * 
 */
void MAIN_SETUP() {
#ifndef DEBUGNONE
    Serial.begin(57600);
#endif

#ifdef GPROF
    gprof.begin(&Serial);
#endif

    pinMode(SET_PIN, INPUT_PULLUP);


#ifdef USE_SDCARD
    pinMode(SDCARD_CS, OUTPUT);
    digitalWrite(SDCARD_CS, HIGH);
#endif

#ifdef USE_ACCELEROMETER_CS
    pinMode(USE_ACCELEROMETER_CS, OUTPUT);
    digitalWrite(USE_ACCELEROMETER_CS, HIGH);
#endif

    // set the ADC to use the external reference which is more stable
    // this will affect all analogRead() calls
    analogReference(EXTERNAL);
    // or, use internal reference, but it's not as stable
    //analogReference(INTERNAL);
    analogWriteResolution(adc_write_bits);
    analogReadResolution(adc_read_bits);
    analogReadAveraging(4); // this one is optional.

    // Wire.setSDA(30);
    // Wire.setSCL(29);
    Wire.begin();
    // Wire.resetBus();

    if (digitalRead(SET_PIN)==LOW) {
        int start = millis();
        while(digitalRead(SET_PIN)==LOW) {
            if (millis() - start > 5000) break;
        }
        if (millis() - start > 5000) {
            settings.factorySettings();
        }
    }

    settings.begin();

    led.begin();
    led.wait(1);

    currentPlugState.begin();

    Power::begin();
#ifdef USE_SLEEP_MODE
    Power::testVolt(1);
#endif

#ifdef USE_DISPLAY
    Display::begin();
    Display::drawLogo();
    Display::displayRender(1);
    Display::displayOn();
#endif

    debug_crash_init();

#ifdef DEBUGDELAY
    delay(3000);
    Serial.println("start");
#endif

    enableTouch();

#if 0

#ifdef USE_UART
    if (currentPlugState.isMidiSerial()) {
        enable_uart_midi = 1;
#ifdef USE_WIFI
        if (settings.enableWifi) {
            midi_wifi.begin();
            // midi_wifi.setLogin(settings.ssid, settings.pass);
        }
        else {
            Uart.begin(31250);
        }
#else
        // Uart.begin(31250);
#endif
    }
    else {
        enable_uart_midi = 0;
    }
#endif

#endif

    fingering.init(settings.keyLayout);

    pressure.initPressure(); // performed in currentOutput
    currentOutput.begin();

#ifdef USE_ACCELEROMETER
    if (Accelerometer::begin()) {
        // updates at 50Hz, so every 20 ms, roughly
        dispatch.attach(Accelerometer::measure, NULL, 20);
    }
    else {
        led.flash(5);
        led.error(1);
        Display::displayError("Accel failed");
    }
#endif

#ifdef LIP_PRESSURE_PIN
    lip.begin();
    dispatch.attach(dispatchReadLip, NULL, 15);
    // dispatch.attach(LipPressure::debug, NULL, 1000);
#endif

    initTouch();

    dispatch.attach(dispatchCheckPlug, NULL, 500);

#ifdef USE_SLEEP_MODE
    dispatch.attach(Power::testVolt, NULL, 5000);
#endif

    dispatch.attach(dispatchLed, NULL, 100);
    dispatch.attach(dispatchSynth, NULL, 100);
    dispatch.attach(dispatchSynthFast, NULL, 1);
    dispatchMidiId = dispatch.attach(Event::processMidiBreath, NULL, settings.midiRate);
    dispatch.attach(Event::processMidiSlow, NULL, 100);
    
    // Sends Active Sense messages at least once every 300ms. However, not all hosts can process this
    // and it causes some problems so disabled by default.
    dispatch.attach(Event::processMidiAlive, NULL, 250);

    // dispatch.attach(dispatchIdleCheck, NULL, 1000);
    // dispatch.attach(dispatchDebug, NULL, 1000);
#ifdef SIMULATE
    dispatch.attach(dispatchDebugPower, NULL, 60*1000); // every 1 minutes
#endif
    // dispatch.attach(dispatchDebugTouch, NULL, 5000);
    // dispatch.attach(dispatchDebugPing, NULL, 1000);

    set_button_begin();

    silenceInit();

    if (! synth.begin()) {
        led.flash(7);
        led.error(1);
        Display::displayError("DAC failed");
    }
    setinstrument(settings.currentInstrument);

#ifdef USE_DISPLAY
    // just in case first init failed due to mistiming
    if (Display::begin(1)) {
        led.flash(3);
        led.error(1);
    }
    Display::update();
    dispatch.attach(dispatchDisplayUpdate, NULL, 300);
    #ifdef DISPLAY_BLOCK_SEND
    dispatch.attach(dispatchDisplayBlock, NULL, 1);
    #endif
#endif

    watchdog.begin();

    led.wait(0);
    led.on(1);
}

int wait_time;

void loop_debug() {
#ifdef DEBUGDEV
    if (loopcount % 10000 == 0) {
        static long tx;
        long ptx = tx;
        tx = millis();
        Serial.print("Loop ");
        Serial.print(loopcount);
        Serial.print(" time=");
        Serial.print(tx - ptx);
        Serial.print(" ms, wait=");
        Serial.print((float)wait_time / 1000.0);
        Serial.println(" ms");
        wait_time = 0;
    }
#endif

#ifdef DEBUGMEM
    if (loopcount % 1000 == 0) {
        trace_memory::verify_all();
    }
#endif

#ifdef DEBUGSOUNDMEM
    if (loopcount % 10000 == 0) {
        synth.dump_memory();
    }
#endif
}

uint32_t delaytime = 0;
int newFingerNote = 0;

/**
 * @brief Called by main loop to process buttons, breath and other playing events
 * 
 */
void processInstrumentState() {

#ifdef LIP_PRESSURE_PIN
    lip.collectData();
#endif

    int pvalue = pressure.getPressure();

    if (breathOnly) {
        Event::eventBreath(pvalue);
        return;
    }

    if (state == STATE_SILENCE) { // no note is playing
        if (pvalue >= STARTNOTEPRESSURE) {  // need to hit min pressure
            if (settings.midiNoteOnVol) {
                state = STATE_PLAY_START;
                delaytime = 0;
            }
            else {
                state = STATE_PRESS_DETECT;
                delaytime = millis() + PRESS_DEBOUNCE_MS;
                return;
            }
        }
        else {
            // we need this to make sure fingering is always the most current; without this
            // we might get phantom notes when we start playing again
            fingering.getNote();
            if (fingering.buttonBits == 0) {
                // nothing going on, so check if we should sleep
                silenceCheck();
            }
            else {
                // still have fingers on pads, so don't sleep
                silenceEnd();
            }
            return;
        }
    }

    if (state == STATE_PRESS_DETECT) {
        if (delaytime > millis()) {
            return;
        }
        state = STATE_PLAY_START;
    }

    if (state == STATE_PLAY_START) {
        silenceEnd();
        int note = fingering.getNote();
        if (note > 0) { // valid note
            Event::eventNoteOn(note, pvalue);
            state = STATE_PLAYING;
        } else if (note == 0) {
            showDebug("Fingering invalid");
            state = STATE_SILENCE;
        } else if (note == -1) {
            showDebug("Fingering error");
            state = STATE_SILENCE;
        }
        return;
    }

    if (state == STATE_PLAYING) { // playing note
        silenceEnd();
        if (pvalue <= STOPNOTEPRESSURE) { // no pressure, so silence.
            Event::eventNoteOff();
            state = STATE_SILENCE;
        }
        else {
            Event::eventBreath(pvalue);
            int note = fingering.getNote();
            if (note != playingNote) {
                state = STATE_PLAY_CHANGE;
                delaytime = millis() + settings.fingerChangeDelay;
                newFingerNote = note;
            }
        }
        return;
    }

    if (state == STATE_PLAY_CHANGE) {
        Event::eventBreath(pvalue);
        if (delaytime > millis()) return;
        int note = fingering.getNote();
        if (note != newFingerNote) {
            // still transitioning fingers
            delaytime = millis() + (settings.fingerChangeDelay >> 1);
            newFingerNote = note;
            return;
        }
        if (note == playingNote) {
            state = STATE_PLAYING;
        }
        else {
            Event::eventNoteChange(note, pvalue);
            state = STATE_PLAYING;
        }
    }
}

int loop_timing = 0;

/**
 * @brief Main loop called by framework repeatedly
 * 
 */
void MAIN_LOOP() {
    watchdog.kick();

#ifdef DEBUGDEV
    int wait_start = micros();
    wait_cpu();
    wait_time += micros() - wait_start;
    ++loopcount;
    loop_debug();
#else
    wait_cpu();
#endif

#ifdef DEBUG2
    Serial.println("loop start");
#endif

    // process displatch functions
    dispatch.process();

    // has SET/MENU button been pressed?
    if (flag_set_button) {
        flag_set_button = 0;
        loadSetButton();
#ifdef USE_DISPLAY
        Display::update();
#endif
    }

#ifdef USE_DISPLAY
    // process menu updates
    if (menu.getState()) {
        int oct = fingering.getOctaveBits(0b11);
        menu.action(oct);
    }
#endif

#ifdef USE_WIFI
    midi_wifi.readData();
#endif

    // Any control messages from host?
    Manage::readMIDI();

    // Override control is active when the host is sending MIDI commands
    // to control synth; used by control app.
    if (Event::overrideControl) {
        if (Event::overrideControl < 4) {
            Event::overrideControl++;
        }
        else if (Event::overrideControl == 4) {
            silenceEnd();
            Event::eventNoteOn(playingNote, 16384);
            Event::overrideControl++;
        }
        // Event::overrideControl = 2;
    }
    else {
        // normal operation of instrument
        processInstrumentState();
    }
}

/**************************
 *
 * MIDI player firmware
 *
 **************************/

#ifdef MIDIPLAYER

#include "MidiPlayer.h"

MidiPlayer player;

void setup()
{
    Serial.begin(57600);
    currentOutput.enableMidi = true;
    pinMode(POWER_LED, OUTPUT);

    player.setupRF(0);
    setinstrument(0);
    player.setSynth(&synth);
}

void loop()
{
    player.processMidi();
    readMIDI();
}
#endif

#include "Test.h"
