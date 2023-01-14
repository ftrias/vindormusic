#ifndef Power_H
#define Power_H

// #include "firmware.h"

#ifdef USE_VARIABLE_CHARGER
#include "MCP4551.h"
extern MCP4551 chargeresistor;
#endif

extern void sayNumbers(int num);
extern void showMessage(const char *msg);

// #ifdef USE_SLEEP_MODE
// #include <Snooze.h>
// #endif

class Power {
public:
    
    static void begin();
    static void chargerOff();
    static void chargerOn();
    static int getDirectVoltage();
    static int getInputVoltage();
    static float volt();
    static int getBatteryPercent();
    static void setPeripherals(int state);
    
#ifdef USE_SLEEP_MODE
    static void powerDownCPU(int times=10);
    static void testVolt(int minimal = 0);
    static void testVoltPrep(void *p);
    static void testVoltDispatch(void *p);
#endif
    
    static void idleSleep(int level);
    
};

#endif
