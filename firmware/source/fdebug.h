#ifdef PRODUCTION

#define FAULT_REBOOT
#include "fmemdebug.h"

#else

// #define DEBUGNONE
// #define DEBUGUART

// #define DEBUGDELAY
// #define DEBUGDEV
// #define DEBUGPLAYER
// #define DEBUGPOWER
// #define DEBUGPRESSURE
// #define DEBUGPRESSUREMAP
// #define DEBUGPC
// #define DEBUG
// #define DEBUG
// #define DEBUG3
// #define DEBUG4
// #define DEBUGDISPATCH
// #define DEBUGSET
// #define DEBUGDEV
// #define DEBUGKEYBOARD
// #define DEBUGNOTE
// #define DEBUGBEND
// #define DEBUGLISTEN
// #define DEBUGSOUND
// #define DEBUGSILENCE
// #define DEBUGDUMPSOUND
// #define DEBUGSOUNDMEM
// #define DEBUGSDPLAY
// #define DEBUGSDPLAYDETAIL
// #define DEBUGEVENT
// #define DEBUGFINGERING
// #define DEBUGFINGERINGMIDI
// #define DEBUGBUTTONS
// #define DEBUGTIMING
// #define DEBUGTIMINGSD
// #define DEBUGRF24
// #define DEBUGDAC
// #define DEBUGTOUCH
// #define DEBUGTOUCH2
// #define DEBUGTOUCH3
// #define DEBUGTOUCHDUMP
// #define DEBUGSPEAKER
// #define DEBUGAMP
// #define DEBUGPLUG
// #define DEBUGMIDI
// #define DEBUGKEY
// #define DEBUGBATTERY
// #define DEBUGACTIVE
// #define DEBUGPERFTIMER
// #define DEBUGTOUCHSAVE
// #define DEBUGRF
// #define DEBUGWIFI
// #define DEBUGVARIABLERESISTOR
// #define DEBUGKEYS
// #define DEBUGNOTETREE

// #define DEBUGMEM
// #define TRACE_PRINT
// #define DEBUGTIMING

#define FAULT_CATCH
// #define FAULT_CATCH_DUMP
// #define FAULT_REBOOT

#include "fmemdebug.h"
#include "perftimer.h"

#if defined(USEDBSTORE)
#define DEBUGINIT dbstore.begin();
#define DEBUGLOG dbstore
#elif defined(SDCARD_DEBUG)
#define DEBUGINIT debugsd.begin();
#define DEBUGLOG debugsd.fp
#define Serial debugsd.fp
#else
#define DEBUGINIT Serial.begin(57600);
#define DEBUGLOG Serial
#endif

#ifdef DEBUGNONE
#define Serial XYZ
#endif

#ifdef DEBUGUART
#define Serial Serial3
#endif

#endif

#ifdef DEBUGTIMING
extern int time_slot;
extern long time_slot_millis[];
extern const char *time_slot_name[];
#define TIME(x) { time_slot_name[time_slot] = x; time_slot_millis[time_slot] = millis() / 100; time_slot++; }
// #define TIME(x) { Serial.print(x); Serial.print(" "); Serial.println(millis()); }
#else
#define TIME(x)
#endif