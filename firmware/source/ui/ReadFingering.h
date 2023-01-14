#ifndef ReadFingering_H
#define ReadFingering_H

#include "ReadFingeringTree.h"

//
// Process fingering maps
//

extern const int16_t fingerlist_sax[];

class ReadFingering : public ReadFingeringTree {
    
public:
    uint16_t buttonBits;
    
private:
    uint16_t buttonBitsPrev;
    int buttonTime;
    uint16_t fingerMask;
    bool autoAccidentals = 0;
    bool useoctave=1;
    bool usebar=1;
    
public:
    uint16_t octMask = 0b11;
    long inactive = 0;
    bool touchDemoMode = 0; // ignores side key
    bool octaveChangeFlag = false;
    
    /**
     * @brief Initialize the fingering using given slot
     * 
     * @param slot Slot number (4, 5 are custom)
     */
    void init(int slot);

    /**
     * @brief Read the fingering bits and return the note number
     * taking into consideration octave keys and adjustments.
     * 
     * @return int Note number
     */
    int getNote();
    
    /**
     * @brief Change the fingering layout
     * 
     * @param slot Fingering layout number
     * @return int Success
     */
    int setKeys(int slot);

    int getOctaveBits(uint16_t mask);

public:

    /**
     * @brief Return last button bits read
     * 
     * @return int Bits of last buttons
     */
    int buttonsOn();

    int setKeyLayout(int c);

    /**
     * @brief Read the button states and determine the note to play.
     */
    int getFingeringNote(bool remember, bool refresh = false) ;
    /**
     * @brief Read the octave buttons and return the octave number, adjusted
     * by octAdj.
     * 
     * @return int Octave number
     */
    int getOctave();

    int getNewNote(bool remember = true);

    /**
     * @brief Return the bend ratio based on the level of pressing
     * of the octave buttons -- DEPRECATED --
     * 
     * @return int 
     */
    int getBend();
    
    void refreshFingering();
};

extern ReadFingering fingering;

#endif
