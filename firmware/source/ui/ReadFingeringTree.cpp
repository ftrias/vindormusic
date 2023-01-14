#include "firmware.h"
#include "ReadFingeringTree.h"

const int16_t *fingerlist = 0; //fingerlist_sax;
int fingerlist_bits = 12;
unsigned int fingerlist_mask = 0x7FF;
int fingerlist_useoctave = 1;

int ReadFingeringTree::useBisKey() {
    return fingerlist_bits == 12;
}

int ReadFingeringTree::getNoteFromTree(unsigned int buttons) {
    int where = 0;
    static unsigned int lastbuttons=~0;
    static unsigned int lastresult=0;
    if (buttons==lastbuttons) return lastresult;
    lastbuttons = buttons;  

    int lastnote = -1;
    buttons &= fingerlist_mask;
    int bit = 1<<(fingerlist_bits-1);

    for(int b=0; b<fingerlist_bits; b++, bit >>= 1) {
        if (fingerlist[where] == 0) { // this button is irrelevant
            where++;
        }
        else if (buttons & bit) {
            where = fingerlist[where];
            if (where <= 0) { // this happens on errors
                lastnote = -where-2;
                break;
            }
        }
        else {
            where++;
        }

        if (where >= 1024) { // internal flaw! overflow
            return -1;
        }

        int next = fingerlist[where];

        if (next < 0) {
            // lastnote = -fingerlist[where];
            lastnote = -next-2;
            break;
        }
    }

    lastresult = lastnote;
    return lastnote;
}

void ReadFingeringTree::setKeysCustom(const int16_t keys[], int bits, int mask, int useoctave) {
    fingerlist = keys; 
    fingerlist_bits = bits;
    fingerlist_mask = mask;
    fingerlist_useoctave = useoctave;
}