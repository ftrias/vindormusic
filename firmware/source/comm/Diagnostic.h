#ifndef INCLUDE_DIAGNOSTIC_H
#define INCLUDE_DIAGNOSTIC_H

class Diagnostic {
public:
    static void diagreboot();
    static void setLed();
    static void touchToggleDigital();
    static void touchGetDigital();
    static void touchReadValue();
    static void touchState() ;
    static void touchButtons();
    static void pressureGet() ;
    static void voltGet() ;
    static void testWifi() ;
    static void noteOn();
    static void noteOff();
    static void setOutput() ;
    static void enter() ;
};
#endif
