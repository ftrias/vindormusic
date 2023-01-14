#ifndef SETTINGS_H
#define SETTINGS_H

#include <string.h>

void setinstrument(int inst);
void reboot();

#define countof(x) (sizeof(x) / sizeof(x[0]))

/**
 * Constants for saving settings to EEPROM
 */

// update with each change in the structure of the settings class
const int SAVE_VER_REVISION = 0x05;
const int SAVE_VER_ADDRESS = 0x00;
const int SAVE_USBMODE_ADDRESS = 0x02;
const int SAVE_ADDRESS = 0x10;

#include "SynthSettings.h"

const int initialNoteVolume = 120;

const int numberOfSounds = 9;

extern const int16_t flash_fingerlist_custom[];

const int NOTATION_LETTERS = 1;
const int NOTATION_SOLFEGE = 2;
const int NOTATION_GERMAN = 3;

typedef enum SwitchState_enum { 
    SWITCH_HEADPHONES = 1, SWITCH_MIDI = 2, SWITCH_AUTO=3, 
    SWITCH_CV = 4, SWITCH_CV_EXT = 5, SWITCH_NONE = 6
} SwitchState;

class SettingsData {
public:
    // settings and parameters (see defaultsettings())
    int8_t noteAdj;
    int8_t resetOnBoot;
    int8_t octAdj;
    int8_t octMode;
    int8_t transpose[12];
    int8_t transposeSetting;
    int8_t usetranspose;

    float pressurefactor;
    int fingerChangeDelay;

    int8_t synthActive;
    float hpVolume;
    float baseFreqA4;
    int8_t usbMode;
    int8_t useBend;
    int8_t currentInstrument;
    SynthSettings soundList[numberOfSounds];

    int8_t useVolume; // by default, use volume
    int8_t midiCC;
    int8_t midiCC_accel_ccw_up;
    int8_t midiCC_accel_ccw_down;
    int8_t midiCC_accel_cw;
    int8_t midiCC_accel_ccw;
    int8_t midiCC_accel_tilt_up;
    int8_t midiCC_accel_tilt_down;
    int8_t midiCC_lip;
    int8_t midiNoteOnVol;
    int8_t channel;
    int midiRate;
    int8_t midiHires;
    int8_t midiNoteOffRepeat;
    int8_t midiActiveSense;
    int midiBendFactor;

    int8_t useGestures;

    int8_t notation;
    unsigned int idleSleep_ms;
    int8_t keyLayout;

    int8_t fingerlist_custom_bits;
    int fingerlist_custom_mask;
    int8_t fingerlist_custom_useoctave;
    static const int fingerlist_custom_max = 1024; // 2048 bytes sector size / sizeof(int16)
    int fingerlist_custom_count;

    int8_t switch_hp_midi;

    int8_t enable_shortcuts;
    int8_t enable_usb_speaker;
    int8_t cv_type;

#ifdef USE_WIFI
    int8_t enableWifi;
    char ssid[32];
    char pass[32];
#endif

#ifdef USE_SAVE_TOUCH
    int16_t saveTouch[pinCount];
#endif

    /**
     * Set default settings
     */

    void defaultsettings();

    int setVolumeMessage(int v);
    int setCustomFingering(int bits, int mask, int octave, const int16_t data[]);    
    void getFingerSerialize(MidiSerial *ser) const;
    void setFingerSerialize(MidiSerial *ser);

    static int getUsbMode();

#ifdef ARDUINO
    
    /**
     * Reset to factory settings
     */
    void factorySettings();
    /**
     * Write a new value to EEPROM only if different from the old one.
     * This is done to avoid wearing out the EEPROM prematurely
     */
    static void EEPROMwritenew(int offset, int value);
    
    void loadSettings(int quick = 0);
    
    void saveSettings();
    
    SettingsData();
    
    void begin();
    
#endif
    
    void processTransposeSetting();
};

// SettingsData must fit in 2048 byte page
static_assert(sizeof(SettingsData) < (2048 - SAVE_ADDRESS - 1), "Error: class SettingsData larger than 2048 bytes");

#if 0
// print size of SettingsData during compile; used for debugging
char (*__kaboom)[sizeof( SettingsData )] = 1;
#endif

extern SettingsData settings;

#endif
