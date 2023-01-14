#include "firmware.h"
#include "WatchDog.h"

#include "Diagnostic.h"

#include "Boot.h"
#include "Lights.h"

#include "Touch.h"

#include "Power.h"
#include "Settings.h"
#include "Event.h"
#include "Pressure.h"

#include "OutputMode.h"

#include "Display.h"

uint8_t *p;

void Diagnostic::diagreboot() {
    reboot();
}

void Diagnostic::setLed() {
    led.color(p[2], p[3]);
}

void Diagnostic::touchToggleDigital() {
    for (int i = 0; i < pinCount; i++) {
        pinMode(pinTouch[i], INPUT_PULLUP);
    }
}

void Diagnostic::touchGetDigital() {
    uint8_t s[pinCount+2];
    s[0] = 0xF0;
    for (int i = 0; i < pinCount; i++) {
        s[i+1] = digitalRead(pinTouch[i]);
    }
    s[pinCount+1] = 0xF7;
    usbMIDI.sendSysEx(pinCount+2, s, true);
}

void Diagnostic::touchReadValue() {
    uint8_t s[pinCount*2+2];
    s[0] = 0xF0;
    for (int i = 0; i < pinCount; i++) {
        int v = readTouchPin(pinTouch[i]);
        s[i*2+1] = v >> 7;
        s[i*2+2] = v & 0x7F;
    }
    s[pinCount*2+1] = 0xF7;
    usbMIDI.sendSysEx(pinCount*2+2, s, true);
}

void Diagnostic::touchState() {
    uint8_t s[pinCount+2];
    int *x = touchReadAll();
    s[0] = 0xF0;
    for (int i = 0; i < pinCount; i++) {
        s[i+1] = x[i];
    }
    s[pinCount+1] = 0xF7;
    usbMIDI.sendSysEx(pinCount+2, s, true);
}

void Diagnostic::touchButtons() {
    uint8_t s[4];
    int x = getButtonsState();
    s[0] = 0xF0;
    s[1] = x >> 7;
    s[2] = x & 0x7F;
    s[3] = 0xF7;
    usbMIDI.sendSysEx(4, s, true);
}

void Diagnostic::pressureGet() {
    int v = pressure.getPressure();
    uint8_t s[4];
    s[0] = 0xF0;
    s[1] = (v >> 7) & 0x7F;
    s[2] = v & 0x7F;
    s[3] = 0xF7;
    usbMIDI.sendSysEx(4, s, true);
}

void Diagnostic::voltGet() {
    int v = analogRead(39); // ref voltage
    // TODO, we can do this without power management
    uint32_t x = Power::getDirectVoltage();
    uint8_t s[6];
    s[0] = 0xF0;
    s[1] = (v >> 7) & 0x7F;
    s[2] = v & 0x7F;
    s[3] = (x >> 7) & 0x7F;
    s[4] = x & 0x7F;
    s[5] = 0xF7;
    usbMIDI.sendSysEx(6, s, true);
}

void Diagnostic::testWifi() {
    uint8_t s[7];
    switch(p[2]) {
        case 0x01:
#ifdef USE_WIFI
            p[p[3]] = 0;
            strcpy(settings.ssid, (char*)p+4);
#endif
            break;
        case 0x02:
#ifdef USE_WIFI
            p[p[3]] = 0;
            strcpy(settings.pass, (char*)p+4);
#endif
            break;
        case 0x03:
#ifdef USE_WIFI
            // midi_wifi.setLogin(settings.ssid, settings.pass);
            midi_wifi.sendLogin();
#endif
            break;
        case 0x04:
            s[0] = 0xF0;
#ifdef USE_WIFI
            s[1] = 0x01;
            s[2] = midi_wifi.isConnected;
            s[3] = midi_wifi.isEnabled();
            s[4] = midi_wifi.isWifi;
#else
            s[1] = 0x00;
#endif
            s[5] = 0;
            s[6] = 0xF7;
            usbMIDI.sendSysEx(7, s, true);
            break;
    }
}

void Diagnostic::noteOn() {
    Event::eventNoteOn(p[2], p[3]);
}

void Diagnostic::noteOff() {
    Event::eventNoteOff();
}

void Diagnostic::setOutput() {
    switch(p[2]) {
        case 0: currentOutput.setSpeaker(p[3]); break;
    }
}

void Diagnostic::enter() {
    while(1) {
        while (usbMIDI.read()) {
            watchdog.kick();
            p = usbMIDI.getSysExArray();
            switch(p[1]) {
                case 0x01: touchToggleDigital(); break;
                case 0x02: touchGetDigital(); break;
                case 0x03: touchReadValue(); break;
                case 0x04: touchState(); break;
                case 0x05: touchButtons(); break;
                case 0x08: pressureGet(); break;
                case 0x09: voltGet(); break;
                case 0x0A: testWifi(); break;
                case 0x15: noteOn(); break;
                case 0x16: noteOff(); break;
                case 0x20: setLed(); break;
                case 0x21: setOutput(); break;
                case 0x2F: rebootBootloader(); break;
                case 0x30: diagreboot(); break;
                case 0x31: return;
            }
        }
    }
}

