#include <string.h>

#ifdef ARDUINO
#include <EEPROM.h>
#endif

#include "Boot.h"
#include "Settings.h"
#include "SynthSettings.h"
#include "ReadFingering.h"

#include "ProgramFlash.h"

/**
 * Set default settings
 */

// Location in flash where custom fingering settings are stored
FLASH_SECTORS(flash_fingerlist_custom, int16_t, 1);

/**
 * @brief Reset factory settings
 * 
 */
void SettingsData::defaultsettings() {    
    // memset(this, 0, sizeof(SettingsData));

    noteAdj = 0;   // no transposing (C major)
    resetOnBoot = 0;
    octAdj = 0;    // standard middle C ocatave
    octMode = 1;
    
    for (unsigned int i = 0; i < countof(transpose); i++) {
        transpose[i] = 0; // no finger transposing
    }
    transposeSetting = 0;
    usetranspose = 0;

    pressurefactor = 3.0F;
    fingerChangeDelay = 30;

    synthActive = 1;
    hpVolume = 0.8F;
    baseFreqA4 = 440.0F;
    usbMode = 0;
    useBend = 0;
    
    currentInstrument = 0;
    for(int i=0; i<numberOfSounds; i++) {
        soundList[i].loadInstrument(i);
    }

    setVolumeMessage(2); // use volume control
    // prev sets midiCC
    midiCC_accel_ccw_up = 1;
    midiCC_accel_ccw_down = 5;
    midiCC_accel_ccw = 4;
    midiCC_accel_cw = 12;
    midiCC_lip = -1;
    midiCC_accel_tilt_up = -1;
    midiCC_accel_tilt_down = -2;
    midiNoteOnVol = initialNoteVolume;
    channel = 1;   // channel one
    midiRate = 20;
    midiHires = 0;
    midiNoteOffRepeat = 0;
    midiActiveSense = 0;
    midiBendFactor = 40;

    useGestures = 1;
    
    notation = NOTATION_LETTERS;
    idleSleep_ms = 10*60*1000; // 10 minutes until sleep mode
    keyLayout = 0;

    fingerlist_custom_bits = 0;
    fingerlist_custom_mask = 0;
    fingerlist_custom_useoctave = 0;
    fingerlist_custom_count = 0;

    switch_hp_midi = SWITCH_HEADPHONES; // SWITCH_AUTO;

    enable_shortcuts = 0;
    enable_usb_speaker = 0;

#ifdef USE_WIFI
    enableWifi = 0;
    memset(ssid, 0, sizeof(ssid));
    memset(pass, 0, sizeof(pass));
#endif

#ifdef USE_SAVE_TOUCH
    for (unsigned int i = 0; i < countof(saveTouch); i++) {
        saveTouch[i] = 0;
    }
#endif
}

/**
 * @brief Set the MIDI CC number to use for volume change events
 * 
 * @param v Stored code (0=none, 1=BC, 2=Vol, etc.)
 * @return int MIDI CC number
 */
int SettingsData::setVolumeMessage(int v) {
    useVolume = v;    
    switch (useVolume) {
        case 0:
            midiCC = -1; // don't use any
            break; 
        case 1:  // BC
            midiCC = 2; 
            midiNoteOnVol = 0;
            break;
        case 2: // Vol
            midiCC = 7; 
            if (midiNoteOnVol == 0) midiNoteOnVol = initialNoteVolume;
            break;
        case 3:  // Expr
            midiCC = 11; 
            if (midiNoteOnVol == 0) midiNoteOnVol = initialNoteVolume;
            break;
        case 4: // Aftertouch
            midiCC = 0; 
            if (midiNoteOnVol == 0) midiNoteOnVol = initialNoteVolume;
            break;
        case 5: // Modulation Wheel
            midiCC = 1; 
            if (midiNoteOnVol == 0) midiNoteOnVol = initialNoteVolume;
            break; 
        default: 
            midiCC = -1; // invalid
            break; 
    }
    return midiCC;
}

/**
 * @brief Initialize custom fingering array/tree and write to flash
 * 
 * @param bits 
 * @param mask 
 * @param octave 
 * @param data 
 * @return int 
 */
int SettingsData::setCustomFingering(int bits, int mask, int octave, const int16_t data[])
{
    fingerlist_custom_bits = bits;
    fingerlist_custom_mask = mask;
    fingerlist_custom_useoctave = octave;
    int16_t *dest = (int16_t *)ProgramFlash::getWrite(flash_fingerlist_custom);
    for(int i=0; i<fingerlist_custom_count; i++) {
        dest[i] = data[i];
        if (data[i] == END_FINGERING) return 1;
    }
    ProgramFlash::commitWrite(flash_fingerlist_custom);
    return 0;
}

/**
 * @brief Pack the fingering information for sending via MidiSerial
 * 
 * @param ser MidiSerial object pointer
 */
void SettingsData::getFingerSerialize(MidiSerial *ser) const
{
    ser->addByte(1);
    ser->addWord2(fingerlist_custom_bits);
    ser->addWord2(fingerlist_custom_mask);
    ser->addWord2(fingerlist_custom_useoctave);
    ser->addWord(fingerlist_custom_count);
    for (int i=0; i<fingerlist_custom_count; i++) {
        ser->addWord2(flash_fingerlist_custom[i]);
    }
}

/**
 * @brief Parse a serialized fingering information and write to flash
 * 
 * @param ser MidiSerial pointer containing serialized info
 */
void SettingsData::setFingerSerialize(MidiSerial *ser)
{
    int ver = ser->getByte();
    if (ver==1) {
        fingerlist_custom_bits = ser->getWord2();
        fingerlist_custom_mask = ser->getWord4();
        fingerlist_custom_useoctave = ser->getWord2();
        int c = ser->getWord();
        fingerlist_custom_count = c;        
        int16_t *dest = (int16_t *)ProgramFlash::getWrite(flash_fingerlist_custom);        
        for (int i=0; i<c; i++) {
            dest[i] = ser->getWord2();
        }
        ProgramFlash::commitWrite(flash_fingerlist_custom);        
    }
}

//
// This code is reused by the control app, so we remove hardware
// specific code.
//
#ifdef ARDUINO

/**
 * @brief Reset to factory settings
 */
void SettingsData::factorySettings() {
    // wipe out custom fingering
    int *dest = (int *)ProgramFlash::getWrite(flash_fingerlist_custom);
    memset(dest, 0, FLASH_SECTOR_SIZE);
    ProgramFlash::commitWrite(flash_fingerlist_custom); 

    // save the default settings
    defaultsettings();
    saveSettings();
    // this is important for booting
    EEPROMwritenew(SAVE_USBMODE_ADDRESS, usbMode);
    delay(200);
    reboot();
}

/**
 * @brief Write a new value to EEPROM only if different from the old one.
 * This is done to avoid wearing out the EEPROM prematurely
 */
void SettingsData::EEPROMwritenew(int offset, int value) {
    if (EEPROM.read(offset) != value) {
        EEPROM.write(offset, value);
    }
}

int SettingsData::getUsbMode() {
    return EEPROM.read(SAVE_USBMODE_ADDRESS);
}

void SettingsData::loadSettings(int quick) {
    static int loaded = 0;
    if (quick && loaded) return;
    loaded = 1;
    int ver = EEPROM.read(SAVE_VER_ADDRESS);
    if (ver == SAVE_VER_REVISION) {
        eeprom_read_block(this, (void*)SAVE_ADDRESS, sizeof(*this));
        // EEPROM.get(SAVE_ADDRESS, *this);
        if (! quick) {
            if (resetOnBoot) {
                resetOnBoot = 0;
                defaultsettings();
                saveSettings();
            }
            setVolumeMessage(useVolume);
        }
    }
}

void SettingsData::saveSettings() {
#ifdef USE_SAVE_TOUCH
    for(int i=0;i<pinCount;i++) {
        saveTouch[i] = baseTouch[i];
    }
#endif
    // load the setting from EEPROM and compare them to the active
    // settiings. If they are different, then save the current settinigs.
    // This is to avoid weariing out the EEPROM.
    SettingsData s2;
    s2.loadSettings();
    int c = memcmp(this, &s2, sizeof(SettingsData));
    if (c != 0) {
        // Serial.println("PUT");
        EEPROM.update(SAVE_VER_ADDRESS, SAVE_VER_REVISION);
        // EEPROM.put(SAVE_ADDRESS, *this);
        eeprom_write_block(this, (void*)SAVE_ADDRESS, sizeof(*this));
    }

    // USB mode is handled seperately because it is used by boot
    EEPROMwritenew(SAVE_USBMODE_ADDRESS, usbMode);
}

SettingsData::SettingsData() {
    defaultsettings();
}

void SettingsData::begin() {
    loadSettings(1);
}

#endif


void SettingsData::processTransposeSetting() {
    //transposeSetting = (val << 3) | num;
    unsigned int val = transposeSetting >> 3;
    unsigned int num = transposeSetting & 0b111;
    
    for (int i = 0; i < 12; i++)
        transpose[i] = 0;
    // val== 0b11 to reset all transp
    if (val == 0b10) { // # sharps
        if (num == 0)
            usetranspose = 0;
        else {
            usetranspose = 1;
            if (num >= 0b001)
                transpose[5] = 1; // F#
            if (num >= 0b010)
                transpose[0] = 1; // C#
            if (num >= 0b011)
                transpose[7] = 1; // G#
            if (num >= 0b100)
                transpose[2] = 1; // D#
            if (num >= 0b101)
                transpose[9] = 1; // A#
        }
    }
    if (val == 0b01) { // # flats
        if (num == 0)
            usetranspose = 0;
        else if (num == 0b111) { // auto-transpose for key of C
            usetranspose = 1;
        } else {
            usetranspose = 1;
            if (num >= 0b001)
                transpose[11] = -1; // Bb
            if (num >= 0b010)
                transpose[4] = -1; // Eb
            if (num >= 0b011)
                transpose[9] = -1; // Ab
            if (num >= 0b100)
                transpose[2] = -1; // Db
            if (num >= 0b101)
                transpose[7] = -1; // Gb
        }
    }
    
    if (val == 0b11) { // use bass cleff
        if (num == 0) {
            noteAdj = 0;
        } else if (num == 0b100) {
            noteAdj = -20;
        } else if (num == 0b010) {
            noteAdj = -11;
        }
    }
    
#ifdef DEBUGDEV
    Serial.print("TRANSP ");
    Serial.print(usetranspose);
    Serial.print(" ");
    Serial.print(num);
    Serial.println("");
#endif
}
