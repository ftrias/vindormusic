#ifndef Boot_H
#define Boot_H
/*
 * reboot
 */

#ifdef ARDUINO

void reboot();
void rebootBootloader();
void usb_reboot_hook(void) ;
void inline wait_cpu() { __asm volatile("wfi"); }

#endif

#endif
