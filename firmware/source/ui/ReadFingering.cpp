#include "firmware.h"
#include "ReadFingering.h"
#include "Touch.h"
#include "Settings.h"

//
// Below are the hard-coded fingerings
// Custom fingerings are stored in SettingsData()
//

#include "FingerTables.auto.hpp"

#ifdef FINGER_TEST
const int16_t fingerlist_test[] = {
0, 41, 0, 5, -17, 14, 8, -26, 0, 0, 0, 13, -25, -25, 18, 17, -24, -25, 0, 37, 36, 29, 26, 25, 
-22, -21, 28, -21, -21, 33, 32, -20, -19, 35, -19, -19, -15, 0, 40, -23, -17, 0, 49, 45, -16, 
0, 48, -15, -16, 55, 54, 53, -14, -15, -13, 59, 58, -12, -13, 0, 84, 77, 70, 67, 66, -10, -9, 69, 
-9, -9, 74, 73, -8, -7, 76, -7, -7, 79, -3, 81, -3, 83, -3, -3, 0, 87, -11, 89, -5, 91, -5, -5, END_FINGERING
};
#endif

int fingerlist_num = 0;

extern const int16_t *fingerlist;
extern int fingerlist_bits;
extern unsigned int fingerlist_mask;
extern int fingerlist_useoctave;

int ReadFingering::setKeyLayout(int c) {
#ifdef DEBUGKEYS
    Serial.print("setKeyLayout=");
    Serial.println(c);
#endif
    fingerlist_num = c;
    switch(c) {
        case 0: 
#ifdef FINGER_TEST
            fingerlist = fingerlist_test;
            fingerlist_bits = 14;
            fingerlist_mask = 0x3FFF;
            fingerlist_useoctave = 0;   
#else
            fingerlist = fingerlist_sax_table; 
            fingerlist_bits = fingerlist_sax_bits;
            fingerlist_mask = fingerlist_sax_mask;
            fingerlist_useoctave = fingerlist_sax_octave;
#endif
            break;
        case 1:
            fingerlist = fingerlist_clarinet_table;
            fingerlist_bits = fingerlist_clarinet_bits;
            fingerlist_mask = fingerlist_clarinet_mask;
            fingerlist_useoctave = fingerlist_clarinet_octave;
            break;
        case 2:
            fingerlist = fingerlist_recorder_table;
            fingerlist_bits = fingerlist_recorder_bits;
            fingerlist_mask = fingerlist_recorder_mask;
            fingerlist_useoctave = fingerlist_recorder_octave;      
            break;
        case 3: 
            fingerlist = fingerlist_trumpet_table; 
            fingerlist_bits = fingerlist_trumpet_bits;
            fingerlist_mask = fingerlist_trumpet_mask;
            fingerlist_useoctave = fingerlist_trumpet_octave;
            break;
        case 4:
            fingerlist = fingerlist_flute_table; 
            fingerlist_bits = fingerlist_flute_bits;
            fingerlist_mask = fingerlist_flute_mask;
            fingerlist_useoctave = fingerlist_flute_octave;
            break;
        case 5:
            fingerlist = flash_fingerlist_custom;
            fingerlist_bits = settings.fingerlist_custom_bits;
            fingerlist_mask = settings.fingerlist_custom_mask;
            fingerlist_useoctave = settings.fingerlist_custom_useoctave;      
            break;
        default: return 0;
    }
    return fingerlist_mask;
}

//
//
//

extern int playingNote;

void ReadFingering::init(int slot) {
    if (slot < 0) return;
    settings.keyLayout = slot;
    setKeys(slot);
    buttonBits = buttonBitsPrev = 0;
}

int ReadFingering::buttonsOn() {
    return buttonBits;
}

int ReadFingering::setKeys(int c) {
    fingerMask = setKeyLayout(c);
    useoctave = fingerlist_useoctave;
    if (fingerMask==0x3FFF) usebar = 1;
    return 1;
}

/**
 * Read the button states and determine the note to play.
 */
int ReadFingering::getFingeringNote(bool remember, bool refresh) {
    buttonBitsPrev = buttonBits;
    buttonBits = getButtonsState();
  
    if (buttonBits == 0) {
        if (inactive == 0) inactive = millis();
    } 
    else {
        inactive = 0;
    }
    
    if (refresh) return 0;
    
    if (remember && buttonBits == buttonBitsPrev) {
        return -1;
    }

    // if ((buttonBits & 0b1100000000000)  != (buttonBitsPrev & 0b1100000000000)) {
    //     octaveChangeFlag = true;
    // }
    // else {
    //     octaveChangeFlag = false;
    // }
    
    uint16_t s = buttonBits & fingerMask;
    
    if (touchDemoMode) {
        if (s & 0b10) s |= 0b11; // link both bottom pinkies
        s &= 0b11100011101;      // ignore Bb and top pinkies
    }
    
    // Check if pressing side button and lower keys. If so,
    // you are probably touching side button by mistake.
    if (s & 0b10000000) { // side Bb button is 
        if ((s & 0b11100000000) == 0b11100000000) { // top three buttons
            if (s & 0b11100) { // pressing first 3 right hand buttons?
                s &= ~0b10000000; // disable side button
            }
        }
    }
    
    if (useoctave) {
        if (s & 0b100000000000) { // bar button?
            if (s & 0b11111111) { // pressing any right hand, side or left/right pinkies?
                s -= 0b100000000000; // disable bar button
            }
        }
    }
    
    int tp = 0;
    if (autoAccidentals) {
        tp = (s >> 6) & 0b11;
        s = s & 0b111100011111;
    }
    
    int note = fingering.getNoteFromTree(s);     
   
    if (fingering.useBisKey()) {
#ifdef PIN_BIS
        int bBis = (buttonBits >> 13) & 1; // Bis button?
        // int bBis2 = (buttonBits >> 10) & 1; // first button?
        int bBisC = ((buttonBits & 0x7F0) == 0b10000000000)?1:0; // first button?
        if (bBis && bBisC) note = 11;
#else
        // int vBis = buttonBits & 1;
        // if (vBis && ((buttonBits & 0x7F0) == 0b10000000000)) {
        if (s == 0b10000100000) {
            note = 11; // high Bb (70 MIDI)
        }
        if ((s & 0b11100011100)== 0b11100000100) { // F#
            note = 19;
        }
#endif
    }
    
    if (note < 0) {
        return -1; // should not happen!
    }
    
    note--; // adj to MIDI 0=C because 0=B
    int note12 = note % 12;
    note += 48; // middle c adj
    
    if (autoAccidentals) {
        if (tp == 0b10) {  // force sharp
            return note + 1;
        } else if (tp == 0b01) { // force flat
            return note - 1;
        } else if (tp == 0b11) { // force natural
            return note;
        }
        return note + settings.transpose[note12];
    } else {
        // no transposition or auto-key, so just use note
        return note;
    }
    
    return 0;
}

int ReadFingering::getOctaveBits(uint16_t mask) {
    return (buttonBits >> 11) & mask;
}

/**
 * Read the octave buttons and return the octave number, adjusted
 * by octAdj.
 *
 * @return Octave number
 */
int ReadFingering::getOctave() {
    byte octBit = (buttonBits >> 11) & octMask;
    
    // using simple sax mode?
    if (settings.octMode == 1) {
        if (octBit & 0b10) {
            return 1 + settings.octAdj;
        }
        return settings.octAdj;
    }
    
    // complex mode
    if (octBit == 0b11) {
        // highoct = 0;
        return 1 + settings.octAdj;
    }
    if (octBit == 0b01) {
        // highoct = 0;
        return settings.octAdj;
    }
    if (octBit == 0b10) {
        // highoct = 1;
        return 2 + settings.octAdj;
    }
    if (octBit == 0b00) {
        // if (highoct)
        //    return 3 + octAdj;
        return -1 + settings.octAdj;
    }
    return settings.octAdj;
}

int ReadFingering::getNote() {
    int note = getFingeringNote(false);
    if (! useoctave) {
        return settings.octAdj * 12 + note + settings.noteAdj;
    }
    if (note <= 0) {
        return 0; // error!
    }
    note += (12 * getOctave()) + settings.noteAdj;
    return note;
}

int ReadFingering::getNewNote(bool remember) {
    int note = getFingeringNote(remember);
    if (note < 0) {
        return -1; // same fingering
    }
    if (note == 0) {
        return 0; // error!
    }
    note += (12 * getOctave()) + settings.noteAdj;
    if (note == playingNote) {
        return -1;
    }
    return note;
}

void ReadFingering::refreshFingering() {
    getFingeringNote(true, true);
}

int ReadFingering::getBend() {
    byte octBit = (buttonBits >> 11) & 0b11;
    if (octBit != 0b11) return 0;        
    const int o1 = pinCount - 13;
    const int o2 = pinCount - 12;
    int v1 = readTouchPin(pinTouch[o1]);
    int v2 = readTouchPin(pinTouch[o2]);
    int rr;
    if (v1>v2) {
        rr = v1 * 1000 / v2;
        if (rr>2000) rr=rr-2000;
        else rr=0;       
    }
    else {
        rr = v2 * 1000 / v1;
        if (rr>2000) rr=-(rr-2000);
        else rr=0;
    }
    
    return rr>>7;
}
