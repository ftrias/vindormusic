#include "firmware.h"
#include "Boot.h"

#include <Audio.h>

/*
 * reboot
 */
void reboot() {
    SCB_AIRCR = 0x05FA0000 | 0b100;
    while (1); // __asm volatile("wfi");
}

extern uint32_t boot_token;
void rebootBootloader() {
    boot_token = 0x74624346;
    reboot();
}

void usb_reboot_hook(void) {
    rebootBootloader();
}
