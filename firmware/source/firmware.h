//
// uncomment one and only one of the following lines to
// compile the desired ROM
//

#ifndef FIRMWARE_H
#define FIRMWARE_H

#ifndef TEST

#define VINDOR

#ifndef TESTDEBUG
#define PRODUCTION
#endif

#endif

// do this to fool the editor's highlighter
#ifndef PCBREV
#warn PCBREV must be defined in Makefile
#define PCB_R1_60
#endif

#include <Arduino.h>

#include "motherboard.h"

#ifdef ES2_LOGO
#define MODEL "ES2"
#else
#define MODEL "ES1"
#endif

#if BUILDPLATFORM != CPLATFORM
#error "Invalid PLATFORM for motherboard revision; revise Makefile"
#endif

#include "testdef.h"
#include "fdebug.h"

const int adc_read_bits = 16;
const int adc_read_scale = (1<<adc_read_bits) - 1;
const int adc_write_bits = 12;
const int adc_write_scale = (1<<adc_write_bits) - 1;

#define USE_CONST_PRESSURE_MAP
#define BREATH_SMOOTHING_BITS 6

// small memory footprint
#ifdef SMALL_FOOTPRINT
#undef SDCARD_CS
#define CONST_NOTETABLE
#define USE_CONST_PRESSURE_MAP
#endif

// #define SDCARD_DEBUG

// #define SIMULATE

#ifdef SIMULATE
extern int simulate_on;
#endif

// #define NOWORDS

// #define SKIP_POWER_CHECK

#ifdef VINDOR
#define MAIN_LOOP loop
#define MAIN_SETUP setup
#define USBMIDI
#define USE_UART
// #define USE_WIFI
#ifndef GPROF
#define USEWATCHDOG
#endif
#define SILENCESHUTDOWN
#define USE_SLEEP_MODE
#define USE_FILTER_SYNTH
#ifndef VINDOR_DEBUG
#define BASE_SOUNDS_MULTIPLE
#endif
// #define USE_EXTERNAL_CV
// VINDOR_AUDIO_RATE causes clicking, so removed for now
// #define VINDOR_AUDIO_RATE
#endif

#ifdef PRODUCTION
#ifndef VINDOR_USB_AUDIO_MIDI
#error Production build requires VINDOR_USB_AUDIO_MIDI
#endif
#endif

// #define FINGER_TEST

#ifdef TEST
#ifndef VINDOR_USB_AUDIO_MIDI_SERIAL
#error Test build requires MIDI & serial; please change Makefile
#endif
#endif

// #define USE_NOTE_TABLE

// #define SAY_NUMBERS

#ifdef FINGER_TABLE
#define INSTRUMENT_FINGER_FULL
#endif
#ifdef TEST_FINGERING
#define INSTRUMENT_FINGER_FULL
#endif

#ifdef USE_WIFI
#define USE_UART
#endif

#ifdef SDCARD_CS
#ifndef USE_SDCARD
#define USE_SDCARD
#endif
#define USE_SD_DIR
#endif

#endif