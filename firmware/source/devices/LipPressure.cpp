#include "firmware.h"

#ifdef LIP_PRESSURE_PIN

#include "LipPressure.h"

#include "Event.h"
#include "Settings.h"
#include "Accelerometer.h"
#include "vdebug.h"

LipPressure lip;

#if 0

#include <stdint.h>

const uint16_t squares[] = {
    0, 1, 4, 9,
    16, 25, 36, 49,
    64, 81, 100, 121,
    144, 169, 196, 225,
    256, 289, 324, 361,
    400, 441, 484, 529,
    576, 625, 676, 729,
    784, 841, 900, 961,
    1024, 1089, 1156, 1225,
    1296, 1369, 1444, 1521,
    1600, 1681, 1764, 1849,
    1936, 2025, 2116, 2209,
    2304, 2401, 2500, 2601,
    2704, 2809, 2916, 3025,
    3136, 3249, 3364, 3481,
    3600, 3721, 3844, 3969,
    4096, 4225, 4356, 4489,
    4624, 4761, 4900, 5041,
    5184, 5329, 5476, 5625,
    5776, 5929, 6084, 6241,
    6400, 6561, 6724, 6889,
    7056, 7225, 7396, 7569,
    7744, 7921, 8100, 8281,
    8464, 8649, 8836, 9025,
    9216, 9409, 9604, 9801,
    10000, 10201, 10404, 10609,
    10816, 11025, 11236, 11449,
    11664, 11881, 12100, 12321,
    12544, 12769, 12996, 13225,
    13456, 13689, 13924, 14161,
    14400, 14641, 14884, 15129,
    15376, 15625, 15876, 16129,
    16384, 16641, 16900, 17161,
    17424, 17689, 17956, 18225,
    18496, 18769, 19044, 19321,
    19600, 19881, 20164, 20449,
    20736, 21025, 21316, 21609,
    21904, 22201, 22500, 22801,
    23104, 23409, 23716, 24025,
    24336, 24649, 24964, 25281,
    25600, 25921, 26244, 26569,
    26896, 27225, 27556, 27889,
    28224, 28561, 28900, 29241,
    29584, 29929, 30276, 30625,
    30976, 31329, 31684, 32041,
    32400, 32761, 33124, 33489,
    33856, 34225, 34596, 34969,
    35344, 35721, 36100, 36481,
    36864, 37249, 37636, 38025,
    38416, 38809, 39204, 39601,
    40000, 40401, 40804, 41209,
    41616, 42025, 42436, 42849,
    43264, 43681, 44100, 44521,
    44944, 45369, 45796, 46225,
    46656, 47089, 47524, 47961,
    48400, 48841, 49284, 49729,
    50176, 50625, 51076, 51529,
    51984, 52441, 52900, 53361,
    53824, 54289, 54756, 55225,
    55696, 56169, 56644, 57121,
    57600, 58081, 58564, 59049,
    59536, 60025, 60516, 61009,
    61504, 62001, 62500, 63001,
    63504, 64009, 64516, 65025
};

inline int isqrt(uint16_t x) {
    const uint16_t *p = squares;

    if (p[128] <= x) p += 128;
    if (p[ 64] <= x) p +=  64;
    if (p[ 32] <= x) p +=  32;
    if (p[ 16] <= x) p +=  16;
    if (p[  8] <= x) p +=   8;
    if (p[  4] <= x) p +=   4;
    if (p[  2] <= x) p +=   2;
    if (p[  1] <= x) p +=   1;

    return p - squares;
}

#endif

int lip_moving_avg = 0;
const int lip_moving_count = 15;
const int lip_moving_bits = 4;

int lip_base;
const int lip_max = float(adc_read_scale) * 1.1375F; // aprox 3600 for 12 bits
float lip_range;

bool LipPressure::enabled;

float memory_midiCC_lip;

const int analog_trigger = adc_read_scale / 20;

void LipPressure::begin() {
    // without pressure, the Lip has a resistance over 1 Mohm. The 50 Kohm resistor acts as a divider.
    // So we use a pullup. If we get a high reading, then nothing is connected.
    pinMode(LIP_PRESSURE_PIN, INPUT_PULLUP);
    delay(10);
    int lip_initial = analogRead(LIP_PRESSURE_PIN);
    if (lip_initial < analog_trigger) { // no load, so not connected
        enabled = false;
    }
    else {
        enabled = true;
    }
    lip_base = lip_initial * 1.10F;
    lip_range = lip_max - lip_base;
}

void LipPressure::collectData() {
    int v = analogRead(LIP_PRESSURE_PIN);
    // debugOutputDecimal2(0, lip_base, v);
    lip_moving_avg = (lip_moving_avg * lip_moving_count + v) >> lip_moving_bits;
}

int LipPressure::getPressure() {
    if (lip_moving_avg > lip_base) return lip_moving_avg - lip_base;
    return 0;
}

void LipPressure::process() {
    if (! enabled) return;

    if (Accelerometer::getState()) { // effects button is active
        if (settings.midiCC_lip < 0) { // we're doing pitch and so is accelerometer
            return;
        }
    }

    // debugOutputDecimal2(0, lip_base, lip_moving_avg);

    if (lip_moving_avg < lip_base) {
        Event::eventEffect(settings.midiCC_lip, 0.0F, &memory_midiCC_lip);
        return;
    }

    if (lip_moving_avg > lip_max) {
        Event::eventEffect(settings.midiCC_lip, 1.0F, &memory_midiCC_lip);
        return;
    }

    float val = (lip_moving_avg - lip_base) / lip_range;
    Event::eventEffect(settings.midiCC_lip, val, &memory_midiCC_lip);
}

void LipPressure::debug() {
    if (lip_moving_avg < lip_base) return;
    if (lip_moving_avg > lip_max) return;

    float val = (lip_moving_avg - lip_base) / lip_range;
    Midi::midiSysExDecimal(val * 1000);
}

#endif
