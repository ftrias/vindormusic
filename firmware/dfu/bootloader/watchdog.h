#ifndef _watchdog_h
#define _watchdog_h
static inline void watchdog_refresh(void)
{
return;
    __disable_irq();
    WDOG_REFRESH = 0xA602;
    WDOG_REFRESH = 0xB480;
    __enable_irq();
}

static inline void watchdog_reboot(void)
{
    // Any invalid write to the WDOG registers will trigger an immediate reboot
    WDOG_REFRESH = 0;
}
#endif
