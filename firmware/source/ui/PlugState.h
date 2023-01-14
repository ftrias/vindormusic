#ifndef PlugState_H
#define PlugState_H

#include "Power.h"

class PlugState {
public:
    typedef enum HpJack_enum { 
        HP_NONE = 0, HP_HEADPHONES = 1, HP_MIDI = 2, 
        HP_CV = 3, HP_CV_EXT = 4 
    } HpJack;
    bool plugUsb = false;
    bool plugLine = false;
    bool plug35 = false;
    HpJack hpJack;

    bool enableRF = false;
#ifdef USE_WIFI
    bool isWifiMidi = false;
#endif
    SwitchState switchState;
    
    void setOutput();

    void onUsbChange();
    void onHpChange();
    void onLineChange() ;

    // void onHeadphonePlugIn();
    // void onHPMIDIChange();
    // void onMidiSerialChange();

    #if RFMIDI
    void onRFChange();
    void checkRF();
    #endif

    bool isUSB();
    bool is35(int delay = 0);
    bool isLineout();
    HpJack getHpJack();
    // bool isMidiSerial();
    void checkState();
    void checkStateDetail();
    static void interruptHeadphonePlug();

    bool hasHeadphone() { return hpJack == HP_HEADPHONES; }
    bool hasMidi() { return hpJack == HP_MIDI; }

    void setSwitch(int8_t state) { setSwitch((SwitchState)state); }
    void setSwitch(SwitchState state);
    
#ifdef USE_VARIABLE_CHARGER
    void push() ;
    void pop();
    static void setChargeRate(float mAmp, int nv=0);
#endif
    
    void begin();
};

extern PlugState currentPlugState;

#endif
