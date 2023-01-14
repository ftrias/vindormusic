/**
 * @file WatchDog.cpp
 * @author Fernando Trias
 * @brief 
 * @version 0.1
 * @date 
 * 
 * @copyright Copyright (c) 2013-2021 by Vindor Music, Inc.  
 * 
 */

#include <kinetis.h>
#include <core_pins.h>
#include <wiring.h>
#include <firmware.h>
#include "WatchDog.h"

void __attribute((naked, noinline, used)) watchdog_isr() {
    __asm volatile("BKPT");
}    

/**
 * @brief Initialize the watchdog parameters.
 * 
 * @param ms 
 */
void WatchDog::begin(int ms)
{
    timeoutmsec = ms;
}

/**
 * @brief Start the watchdog timer; called by kick() on first run
 * 
 */
void WatchDog::init() {
#ifdef USEWATCHDOG
    initflag=1;
    lastkick = millis();

    // the following code should be placed at the end of setup() since the watchdog starts right after this
    WDOG_UNLOCK = WDOG_UNLOCK_SEQ1;
    WDOG_UNLOCK = WDOG_UNLOCK_SEQ2;
    delayMicroseconds(1);// Need to wait a bit..
    //WDOG_STCTRLH = 0x0001; // Enable WDG
    WDOG_STCTRLH = (WDOG_STCTRLH_ALLOWUPDATE | WDOG_STCTRLH_WDOGEN | WDOG_STCTRLH_WAITEN | WDOG_STCTRLH_STOPEN);// Enable WDG
    WDOG_TOVALL = timeoutmsec;// The next 2 lines sets the time-out value. This is the value that the watchdog timer compare itself to.
    WDOG_TOVALH = 0;
    WDOG_PRESC = 0;// This sets prescale clock so that the watchdog timer ticks at 1kHZ instead of the default 1kHZ/4 = 200 HZ
#endif
}

/**
 * @brief Kick the watchdog so it doesn't force a reboot.
 * 
 */
void WatchDog::kick()
{
#ifdef USEWATCHDOG
    if (! initflag) {
        init();
        return;
    }
    
    // the smallest delay needed between each refresh is 1ms. anything faster and it will also reboot.
    if (millis() <= lastkick + 10) return;// at most every 10ms
    lastkick = millis();
    
    // use the following 4 lines to kick the dog
    noInterrupts();
    WDOG_REFRESH = 0xA602;
    WDOG_REFRESH = 0xB480;
    interrupts();
#endif
}
