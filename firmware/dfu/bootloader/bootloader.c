/*
 * Fadecandy DFU Bootloader
 * 
 * Copyright (c) 2013 Micah Elizabeth Scott
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <stdbool.h>
#include "usb_dev.h"
//#include "serial.h"
#include "mk20dx128.h"

extern uint32_t app_resethandler;

extern uint32_t boot_token;
static __attribute__ ((section(".appvectors"))) uint32_t appVectors[64];

// #define BUG_1

// const uint32_t x_bit = 1 << 1;
// const uint32_t led_bit = 1 << 5;
#ifdef BUG_1
const uint32_t led_bit = 1 << 4;
#else
const uint32_t led_bit = 1 << 6;
#endif

static void led_init()
{
    #ifdef BUG_1
    PORTD_PCR4 = PORT_PCR_MUX(1) | PORT_PCR_DSE | PORT_PCR_SRE;
    GPIOD_PDDR = led_bit;
    #else
    //PTD6 is bliue led for Vindor boards
    PORTD_PCR6 = PORT_PCR_MUX(1) | PORT_PCR_DSE | PORT_PCR_SRE;
    GPIOD_PDDR = led_bit;
    #endif
}

static void led_toggle()
{
    GPIOD_PTOR = led_bit;
}

static inline bool test_boot_token()
{
    /*
     * If we find a valid boot token in RAM, the application is asking us explicitly
     * to enter DFU mode. This is used to implement the DFU_DETACH command when the app
     * is running.
     */
    return boot_token == 0x74624346;
}

static inline bool test_app_missing()
{
    /*
     * If there doesn't seem to be a valid application installed, we always go to
     * bootloader mode.
     */

    uint32_t entry = appVectors[1];
#if 0
    serial_debug("ADDR");
    serial_phex32((uint32_t)appVectors);
    serial_write("\r\n",2);
    serial_phex32(entry);
    serial_write("\r\n",2);
#endif

    return entry < 0x00001000 || entry >= 128 * 1024;
}

#if 0
static inline bool test_banner_echo()
{
    /*
     * At startup we print this banner out to the serial port.
     * If we see it echo back to us, we enter bootloader mode no matter what.
     * This is intended to be a foolproof way to enter recovery mode, even if other
     * circuitry has been connected to the serial port.
     */
    static char banner[] = "FC-Boot";
    const unsigned bannerLength = sizeof banner - 1;
    unsigned matched = 0;

    // Write banner
    serial_begin(BAUD2DIV(9600));
    serial_write(banner, sizeof banner - 1);

    // Newline is not technically part of the banner, so we can do the RX check
    // at a time when we're sure the other characters have arrived in the RX fifo.
    serial_write("\r\n", 2);
    serial_flush();

    while (matched < bannerLength) {
        if (serial_available() && serial_getchar() == banner[matched]) {
            matched++;
        } else {
            break;
        }
    }

    //serial_end();
    return matched == bannerLength;
}
#endif

static inline bool test_reset()
{
	return (RCM_SRS0 & RCM_SRS0_PIN);
}

/*
 * system clock for timeouts
 */
volatile uint32_t systick_millis_count;
inline long millis() 
{ 
	return systick_millis_count; 
}
void systick_init() 
{
	// initialize the SysTick counter
        SYST_RVR = (F_CPU / 1000) - 1;
        SYST_CVR = 0;
        SYST_CSR = SYST_CSR_CLKSOURCE | SYST_CSR_TICKINT | SYST_CSR_ENABLE;
        SCB_SHPR3 = 0x20200000;  // Systick = priority 32
}
void systick_isr(void)
{
        systick_millis_count++;
}

/*
 * reboot software
 */
inline void reboot() {
    	SCB_AIRCR = 0x05FA0000 | 0b100;
    	while (1);
}

int flash(int milliseconds)
{
	static int elapse = 0;
	if (millis() - elapse > milliseconds) {
		led_toggle();
		//serial_putchar('.');
		elapse = millis();
		return 1;
	}
	return 0;
}

void run_app() {
    if (test_app_missing() || test_boot_token() || test_reset()) {
        return;
    }
    uint32_t addr = (uintptr_t)appVectors;
    SCB_VTOR = addr; // relocate vector table
    // addr is in r0
    __asm__("ldr sp, [%[addr], #0]\n"
            "ldr pc, [%[addr], #4]"
            :: [addr] "r" (addr));
    // NOTREACHED
    __builtin_unreachable();
}

int main()
{
        // We're doing DFU mode!
        systick_init();        
        led_init();
        dfu_init();
        usb_init();

        int timeoutwait = millis() + 5000; // wait only 5 seconds
        if (! test_boot_token()) { // unless it's a manual job
            timeoutwait += 15000; // then wait another 15 sec
        }

        // Wait for firmware download
        while (dfu_getstate() == dfuIDLE) {
            watchdog_refresh();
            flash(1000);
            if (millis() > timeoutwait) {
                goto rebootnow;
            }
        }
        
        // serial_debug("WT");
        
        // Wait for firmware download
        while (dfu_getstate() != dfuMANIFEST) {
            watchdog_refresh();
            flash(50);
        }

        dfu_end();
        
rebootnow:
        // Clear boot token, to enter the new application
        boot_token = 0;

#if 0
        // Wait a little bit longer, while flashing the LED.
        for (i = 11; i; --i) {
            //led_toggle();
            for (j = 1000000; j; --j) {
                watchdog_refresh();
                if (flash(200)) break;
            }
        }
#endif

        // USB disconnect and reboot, using watchdog to time 10ms.
        watchdog_refresh();
        __disable_irq();
        USB0_CONTROL = 0;
        reboot();
    return 0;
}
