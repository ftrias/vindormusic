#include "firmware.h"

#include <stdint.h>
#include <Audio.h>

#include "Settings.h"
#include "PlugState.h"
#include "Power.h"
#include "OutputMode.h"
#include "SoundSynthesis.h"
#include "Lights.h"
#include "Display.h"
#include "Pressure.h"
#include "Event.h"
#include "MyWire.h"
#include "vdebug.h"

#ifdef USE_VARIABLE_CHARGER
#include "MCP4551.h"
extern MCP4551 chargeresistor;
const int slowCharge = 100;
const int fastCharge = 300;
static int lastChargeRate = 0;
#endif

// from core libraty, to determime if usb is connected
extern volatile uint8_t usb_configuration;

/**
 * @brief Based on current state, set and power output connections
 * 
 */
void PlugState::setOutput() {
    bool defaultSound = true;

    if (plugUsb || hpJack == HP_MIDI) {
        // showDebug("USB/Midi");
        if (settings.enable_usb_speaker) {
            currentOutput.setLineout(true);
            currentOutput.setSpeaker(1);
            synth.setMainVolume(1.0);
            synth.setInvert(1);
        }
        else {
            currentOutput.setSpeaker(0);
        }
        pressure.initPressure();
        defaultSound = false;
    } 

    if (hpJack == HP_HEADPHONES) { // headphone?
        // showDebug("Headphones");
        setSwitch(SWITCH_HEADPHONES);
        currentOutput.setHeadphone(true);
        currentOutput.setLineout(false);
        currentOutput.setSpeaker(false);
#ifdef USE_SIMPLE_CV
        synth.setCV(false);
#endif
        synth.setMainVolume(1.0);
        synth.setInvert(1);
        pressure.initPressure();
        defaultSound = false;
    }
    else {
        currentOutput.setHeadphone(false);
    }
    
    if (plugLine) { // line-out?
        // showDebug("Lineout");
        currentOutput.setLineout(true);
        currentOutput.setSpeaker(false);
#ifdef USE_SIMPLE_CV
        synth.setCV(false);
#endif
        synth.setMainVolume(1.0);
        synth.setInvert(-1);
        pressure.initPressure();
        defaultSound = false;
    }
    else {
        if (! currentOutput.speakerState) {
            currentOutput.setLineout(false);
        }
    }

#ifdef USE_SIMPLE_CV
    if (plug35CV) { // CV through headphone jack
        // showDebug("CV simple");
        currentOutput.setHeadphone(true);
        currentOutput.setSpeaker(false);
        synth.setCV(true);
        synth.setMainVolume(1.0);
        synth.setInvert(1);
        pressure.initPressure();
        defaultSound = false;
    } 
#endif

#ifdef USE_EXTERNAL_CV
    if (plug35CVext) { // CV external through headphone jack
        // showDebug("CV complex");
        currentOutput.setHeadphone(false);
        currentOutput.setSpeaker(false);
        setSwitch(SWITCH_CV_EXT);
        synth.setCV(true);
        // synth.setMainVolume(1.0);
        // synth.setInvert(0);
        // pressure.initPressure();
        defaultSound = false;
    } 
#endif

#ifdef USE_WIFI
    if (isWifiMidi) {
        // showDebug("Wifi");
        if (plug35Headphones || plugLine) { // headphone or line-out?
            currentOutput.setHeadphone(true);
        }
        else {
            currentOutput.setHeadphone(false);
        }
        currentOutput.setSpeaker(false);
        synth.setMainVolume(1.0);
        synth.setInvert(1);
        pressure.initPressure();
        defaultSound = false;
    }
#endif

    if (defaultSound) {
        // showDebug("Speaker");
        currentOutput.setLineout(true);
        currentOutput.setHeadphone(false);
        currentOutput.setSpeaker(true);
#ifdef USE_SIMPLE_CV
        synth.setCV(false);
#endif
        synth.setMainVolume(0.4);
        synth.setInvert(1);
        pressure.initPressure();
    }
}

/**
 * @brief Called when USB connected/disconnected
 * 
 */
void PlugState::onUsbChange() {
#ifdef USE_VARIABLE_CHARGER
    if (plugUsb) {
        setChargeRate(slowCharge);
    }
    else {
        setChargeRate(fastCharge);
    }
#endif
    led.usb(plugUsb);
    setOutput();

#ifdef USE_ADD_NOISE
    // sometimes, USB adds intereference, so we add noise to muffle it
    synth.setInterference(plugHeadphones);
#endif
}

/**
 * @brief Called when HP jack is changed
 * 
 */
void PlugState::onHpChange() {
    // verify transition
    if (plug35) {
        int v = is35(10);
        plug35 =  v;
    }

    // midi is off by default
    enable_uart_midi = 0;

    if (plug35) {
        hpJack = getHpJack();
        // debugOutputDecimal(990 + hpJack);
        if (hpJack == HP_HEADPHONES) {
            synth.powerHeadphone(1);
            setSwitch(SWITCH_HEADPHONES);
        }
#ifdef USE_SIMPLE_CV
        else if (hpJack == HP_CV) {
            synth.powerHeadphone(1);
            setSwitch(SWITCH_CV);
        }
#endif
#ifdef USE_EXTERNAL_CV
        else if (hpJack == HP_CV_EXT) {
            synth.powerHeadphone(0);
            setSwitch(SWITCH_CV_EXT);
        }
#endif
        else if (hpJack == HP_MIDI) {
            synth.powerHeadphone(0);
            enable_uart_midi = 1;
            Uart.begin(31250);
            setSwitch(SWITCH_MIDI);
        }
        Display::update();
    }
    else {
        hpJack = HP_NONE;
        synth.powerHeadphone(0);
    }
    setOutput();
}

/**
 * @brief Called when lineout is plugged/unplugged
 * 
 */
void PlugState::onLineChange() {
    led.headphones(plugLine);
    setOutput();
}

#if 0
void PlugState::onMidiSerialChange() {
    return; // DEBUG
#ifdef USE_UART
    if (hpJack == HP_MIDI) {
        enable_uart_midi = 1;
#ifdef USE_WIFI
        if (settings.enableWifi) {
            // midi_wifi.begin();
        }
        else {
            Uart.begin(31250);
        }
#else
        Uart.begin(31250);
#endif
    }
    else {
        enable_uart_midi = 0;
        Uart.end();
    }
#else
    enable_uart_midi = 0;
#endif
    setOutput();
}
#endif

#ifdef RFMIDI   

void PlugState::onRFChange() {
    // showDebug("RF change");
    if (rfmidi.enabled) {
        currentOutput.setHeadphone(true);
        currentOutput.setSpeaker(false);
        pressure.initPressure();
        rftimer.begin(rftimerCallback, 100); // time in microsec
    }
    else {
        currentOutput.setHeadphone(false);
        currentOutput.setSpeaker(true);
        pressure.initPressure();
        rftimer.end();
    }
}

void PlugState::checkRF() {
    if (rfmidi.setupConnect()) {
        enableRF = true;
        onRFChange();
    } else {
        enableRF = false;
        disableRF();
    }
}
#endif

/**
 * @brief Is USB connectd?
 * 
 * @return true   USB available
 * @return false  USB not connected
 */
bool PlugState::isUSB() {
    return usb_configuration != 0;
}

#ifdef AUDIOPLUG_REVERSE
const int analog_trigger = adc_read_scale * 3 / 4;
#else
const int analog_trigger = adc_read_scale / 30;
#endif

/**
 * @brief Is something plugged into HP 3.5mm jack?
 * 
 * @return true  Something plugged in
 * @return false Nothing plugged in
 */
bool PlugState::is35(int delayms) {
    if (settings.switch_hp_midi ==  HP_NONE) return false;
    if (delayms) delay(delayms);

    int v = analogRead(AUDIOPLUG_PIN);

#ifdef AUDIOPLUG_REVERSE
    return v > analog_trigger;
#else
    return v < analog_trigger;
#endif
}

/**
 * @brief Determine the type of device plugged into 3.5mm jack
 * 
 * @return PlugState::HpJack 
 */
PlugState::HpJack PlugState::getHpJack() {
#ifndef AUDIOPLUG_PIN
    return HP_HEADPHONES;
#endif

#ifdef SWITCH_HP_MIDI_PIN
    if (settings.switch_hp_midi == SWITCH_AUTO) {
        // we're going to check if there is resistance on the pin; that would
        // indicate a headphone. MIDI doesnt' really create a load.
        // setSwitch(SWITCH_MIDI); // set so we can read the pin
        digitalWrite(SWITCH_HP_MIDI_PIN, LOW);
        pinMode(UART_SENSE_PIN_TX, INPUT_PULLUP);
        delay(1);
        int v = digitalRead(UART_SENSE_PIN_TX);
        setSwitch(switchState);
        // Midi::midiSendDebug(90+v);
        if (v) return HP_MIDI;
        else return HP_HEADPHONES;
    }
    else {
        switch(settings.switch_hp_midi) {
            case SWITCH_NONE: return HP_NONE;
            case SWITCH_MIDI: return HP_MIDI;
            case SWITCH_HEADPHONES: return HP_HEADPHONES;
#ifdef USE_SIMPLE_CV
            case SWITCH_CV: return HP_CV;
#endif
#ifdef USE_EXTERNAL_CV
            case SWITCH_CV_EXT: return HP_CV_EXT;
#endif
            default: return HP_HEADPHONES;
        }
    }
#else
    // Midi::midiSendDebug(880 + settings.switch_hp_midi);
    switch(settings.switch_hp_midi) {
        case SWITCH_NONE: return HP_NONE;
        case SWITCH_HEADPHONES: return HP_HEADPHONES;
#ifdef USE_SIMPLE_CV
        case SWITCH_CV: return HP_CV;
#endif
        default: return HP_HEADPHONES;
    }
#endif
}

/**
 * @brief Is something plugged into line out?
 * 
 * @return true 
 * @return false 
 */
bool PlugState::isLineout() {
#ifdef LINEPLUG_PIN
#ifdef LINEPLUG_PIN_ANALOG
    return analogRead(LINEPLUG_PIN) > analog_trigger;
#else
    return digitalRead(LINEPLUG_PIN) == HIGH;
#endif
#else
    return false;
#endif
}

#if 0

bool PlugState::isMidiSerial() {
#ifdef XUART_SENSE_PIN_TX
    if (enable_uart_midi) {
        if (millis() - last_midi_time > 1000) {
            pinMode(UART_SENSE_PIN_TX, INPUT_PULLDOWN);
            delay(1);
            bool r = digitalRead(UART_SENSE_PIN_TX)?1:0;
            pinMode(UART_SENSE_PIN_TX, OUTPUT);
            if (r) {
                return false; // headphone connected
            }
            else {
                return true;
            }
            return r;
        }
    }
    else {
        pinMode(UART_SENSE_PIN_TX, INPUT_PULLDOWN);
        delay(1);
        bool r = digitalRead(UART_SENSE_PIN_TX)?1:0;
        pinMode(UART_SENSE_PIN_TX, OUTPUT);
        return r;
    }
#endif
    return false;
}

#endif

const int CHECKPLUGINTERVAL = 500;

/**
 * @brief Peridically check the plugs for changes
 * 
 */
void PlugState::checkState() {
    static unsigned int checkinterval = millis() + CHECKPLUGINTERVAL;

    if (millis() > checkinterval) {
        checkinterval = millis() + CHECKPLUGINTERVAL;
        checkStateDetail();
    
#ifdef USE_WIFI
        if (isWifiMidi != midi_wifi.isConnected) {
            isWifiMidi = midi_wifi.isConnected;
            setOutput();
        }
#endif
    }
}

/**
 * @brief See if there are any changes in plugs and perform actions
 * 
 */
void PlugState::checkStateDetail() {
#if RFMIDI
    static int prev_rf_state = 0;
    if (rfmidi.enabled != prev_rf_state) {
        onRFChange();
        prev_rf_state = rfmidi.enabled;
    }
#endif
    
    if (isUSB()) {
        if (!plugUsb) {
            plugUsb = true;
            onUsbChange();
        }
    } else {
        if (plugUsb) {
            plugUsb = false;
            onUsbChange();
        }
    }
    
    if (isLineout()) {
        if (! plugLine) {
            plugLine = true;
            onLineChange();
        }
    }
    else {
        if (plugLine) {
            plugLine = false;
            onLineChange();
        }
    }
    
    if (is35()) {
        if (! plug35) {
            plug35 = true;
            onHpChange();
        }
    }
    else {
        if (plug35) {
            plug35 = false;
            onHpChange();
        }
    }
}

#ifdef USE_VARIABLE_CHARGER

void PlugState::setChargeRate(float mAmp, int nv) {    
    lastChargeRate = mAmp;
    
    // we have 10K resistor in parallel with variable resistor
    // charge rate is mA
    // A = 1000 / R_tot
    // mA = A * 1000
    // R_var = N * (100K / 128)
    // 1/R_tot = 1/10K + 1/R_var
    float R_tot = 1000000.0 / mAmp;
    float R_var = 10000.0 * R_tot / (10000.0 - R_tot);
    if (R_tot >= 10000.0) {
        R_var = 100000.0;
    }
    float N = 128.0 * R_var / 100000.0;
    
#ifdef DEBUGVARIABLERESISTOR
    Serial.print("set wiper ");
    if (nv) Serial.print("NV ");
    Serial.println(N);
    Serial.print("desired res ");
    Serial.println(R_tot);
    Serial.print("variable res ");
    Serial.println(R_var);       
#endif
    
    AudioNoInterrupts();
    if (nv) {
        chargeresistor.setNVWiper(N);
    }
    else {
        chargeresistor.setWiper(N);
    }
    AudioInterrupts();
}
#endif

/**
 * @brief Switch the HP 3.5mm jack function
 * 
 * @param state 
 */
void PlugState::setSwitch(SwitchState state) {
#ifdef SWITCH_HP_MIDI_PIN
    if (state == SWITCH_AUTO) {
        if (getHpJack() == HP_MIDI) {
            switchState = SWITCH_MIDI;
        }
        else {
            switchState = SWITCH_HEADPHONES;
        }
    }
    else {
        switchState = state;
    }

    enable_uart_midi = 0;

    if (switchState == SWITCH_NONE) {
        digitalWrite(SWITCH_HP_MIDI_PIN, HIGH);
    }
    else if (switchState == SWITCH_HEADPHONES || switchState == SWITCH_CV) {
        digitalWrite(SWITCH_HP_MIDI_PIN, HIGH);
        pinMode(UART_SENSE_PIN_TX, INPUT);
        Uart.end();
    }
    else if (switchState == SWITCH_CV_EXT) {
        digitalWrite(SWITCH_HP_MIDI_PIN, LOW);
    }
    else if (switchState == SWITCH_MIDI) {
        digitalWrite(SWITCH_HP_MIDI_PIN, LOW);
        enable_uart_midi = 1;
        Uart.begin(31250);
    }
#ifdef USE_SIMPLE_CV
    else if (switchState == SWITCH_CV_EXT) {
        digitalWrite(SWITCH_HP_MIDI_PIN, LOW);
        pinMode(UART_SENSE_PIN_TX, OUTPUT);
        digitalWrite(UART_SENSE_PIN_TX, HIGH);
    }
#endif

#else
    switchState = state;
#endif
}

/**
 * @brief Initialize jack sensing
 * 
 */
void PlugState::begin() {
#ifdef AUDIOPLUG_PIN
    // detect audio plug/unplug
    pinMode(AUDIOPLUG_PIN, INPUT);
    // attachInterrupt(AUDIOPLUG_PIN, interruptHeadphonePlug, CHANGE);
#endif
    
#ifdef LINEPLUG_PIN
    // detect audio plug/unplug
    pinMode(LINEPLUG_PIN, INPUT_PULLUP);
    // attachInterrupt(LINEPLUG_PIN, interruptLinePlug, CHANGE);
#endif

#ifdef SWITCH_HP_MIDI_PIN
    pinMode(SWITCH_HP_MIDI_PIN, OUTPUT);
    digitalWrite(SWITCH_HP_MIDI_PIN, HIGH); // headphone connect to avoid static
    // setSwitch(settings.switch_hp_midi);
#endif

#ifdef SDCARD_DETECT
    pinMode(SDCARD_DETECT, INPUT_PULLDOWN);
    pinMode(SDCARD_DETECT, INPUT);
#endif
    
#ifdef USE_VARIABLE_CHARGER
    // Wire.begin();
    chargeresistor.begin();
    if (chargeresistor.testConnection()) {
        setChargeRate(fastCharge,1);
        setChargeRate(fastCharge);
    }
#endif

    checkStateDetail();
#ifdef RFMIDI
    checkRF();
#endif
    setOutput();
}
