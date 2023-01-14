//
// if you are running a test build, uncomment only one of the following
//
#ifdef TEST
#define FAKE_SYNTH
#define TEST_POWER_ON    // Turn on with power; ignore power button
// TESTING AND DEBUGGING below
// #define FINGER_TABLE
// #define TEST_MULTI
// #define TEST_PLUG
// #define TEST_PID
// #define TEST_PING
// #define TEST_PING_MAIN
// #define TEST_PING_MIDI
// #define TEST_PING_UART
// #define TEST_FINGERING
// #define TEST_FINGERING_SOUND    // req. TEST_FINGERING
// #define TEST_TOUCH
// #define TEST_TOUCH_SIMPLE
// #define TEST_TOUCH_TRIGGER
// #define TEST_TOUCH_GRAPH
// #define TEST_TOUCH_CALIBRATE
// #define TEST_TOUCH_SPEED2
// #define TEST_TOUCH_SPEED
// #define TEST_TOUCH_BEND
// #define TEST_SOUND
// #define TEST_SOUND_LIB
// #define TEST_SOUND_USB
// #define TEST_SOUND_RANGE
// #define TEST_SILENCE
// #define TEST_PRESSURE
// #define TEST_PRESSURE_SIMPLE
// #define TEST_PCM
// #define TEST_USB
// #define TEST_USBSTATUS
// #define TEST_BUTTONS
// #define TEST_WDOG
// #define TEST_LIGHTS
// #define TEST_POWER
// #define TEST_BATTERY
// #define TEST_SD_CARD
// #define TEST_SD_SPEED
// #define TEST_USB_AUDIO
// #define TEST_SNOOZE
// #define TEST_WIFI_MIDI
// #define TEST_DISPLAY
// #define TEST_ACCELEROMETER
// #define TEST_VARIABLE_RESISTOR
#define TEST_ONEWIRE

// #define USBMIDI

#endif

#ifdef TEST_SOUND_LIB
#define FAKE_SYNTH
#endif