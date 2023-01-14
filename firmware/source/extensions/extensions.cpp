#include <stdint.h>

// TODO: allocate RAM for extension
volatile int x = 0x1f1f;

enum Item_type { FREQ=1, VOL=2 };

void *init(void *p) {
    if (x) x=15;
    return 0;
}

int update(int16_t data[], int len) {
    for(int i = 0; i<len; i++) {
        data[i] = 0;
    }
    return 0;
}

int modify(Item_type item, int value) {
    x = value;
    return 0;
}

__attribute__ ((section(".vectors"), used))
void (* const _VectorsExt[])(void) = {
    (void (*)(void))init,
    (void (*)(void))update,
    (void (*)(void))modify,
    0
};
