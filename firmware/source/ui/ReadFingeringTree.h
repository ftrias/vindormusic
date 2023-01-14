#ifndef ReadFingeringTree_H
#define ReadFingeringTree_H

#include "stdint.h"

const int END_FINGERING = 1024;

class ReadFingeringTree {
public:
    void setKeysCustom(const int16_t keys[], int bits, int mask, int useoctave);
    
    /**
     * @brief Given the button bits, follow the tree and return
     * the note number
     * 
     * @param buttons Bits for buttons
     * @return int Note number
     */
    int getNoteFromTree(unsigned int buttons);

    int useBisKey();
};

#endif