#include "firmware.h"

#include "Pressure.h"

#include "Lights.h"
#include "Settings.h"

#include "Display.h"

#include "vutils.h"
#include "vdebug.h"

unsigned int base_pressure = 0;
const int base_bits = 6;
int base_scale = 2 << base_bits;

#ifdef USE_CONST_PRESSURE_MAP
// const pressure map saves space but is not adjusted for atmospheric conditions
#include "pressuremap.h"
#endif

#ifdef USE_AIR_MPRLS
#include "MPRLS.h"
MPRLS mprls;
int mprls_valid = 0;
#endif

unsigned int pressure_debounce = 0;

const int pressure_adjust_scale = 4;
int pressure_adjust = 1 << pressure_adjust_scale;
    
/**
 * @brief Get current pressure
 * 
 * @param myval Optional value to return (used by simulation)
 * @return int Reading (0-65535)
 */
int Pressure::getPressure(int myval) {
    uint32_t v;
    if (myval) {
        lastraw = myval;
    }
    else {
#ifdef USE_AIR_MPRLS
        if (mprls_valid) {
            // long start = micros();
            AudioNoInterrupts();
            v = mprls.readDataStage();
            AudioInterrupts();
            // long elapse = micros() - start;
            if (v == MPRLS_INVALID) { return lastPressure; }
            lastraw = v - base_pressure;
            if (lastraw < 0) lastraw = 0;
            else lastraw >>= 8;
        }
        else return 0;
#else
        v = analogRead(BREATH_PIN);
        
        static EMA<BREATH_SMOOTHING_BITS> v_ema;
        v_ema.update(v);
        v = v_ema;
        
        // scale reading
        if (v >  base_pressure) {
            lastraw = ((v - base_pressure) * base_scale) >> base_bits;
            if (lastraw > adc_read_scale) lastraw = adc_read_scale;
        }
        else {
            lastraw = 0;
        }
#endif

        /* 
        When you tongue a note off (that is a very quick stop), the sensor readings
        go negative and then bounce up and down for approx 30-40ms. Thus, we need
        to debounce the signal. However, if it goes very high, then it's probabably
        a quick tonguing effect, so no debouncing. 
        
        Tested on analog sensor. Need to test on MPRLS.
        */
        if (pressure_debounce) {
            if (pressure_debounce > millis()) {
                if (v < (base_pressure << 1)) {
                    lastPressure = 0;
                    return lastPressure;
                }
            }
            pressure_debounce = 0;
        }

        if (v < (base_pressure >> 1)) {
            pressure_debounce = millis() + 40;
            lastPressure = 0;
            return lastPressure;
        }
    }
    
#ifdef SIMULATE
    static int simvalue = 0;
    static int simdir = 1;
    static long simlast = 0;
    if (simulate_on) {
        if (millis() - simlast > 50) {
            simlast = millis();
            simvalue += simdir * random(-500, 1000);
            if (simvalue<0) {
                simvalue=0;
                simdir = random(1, 1000);
            }
            if (simvalue>65535) {
                simvalue=65535;
                simdir = -random(1, 1000);
            }
            lastraw = simvalue;
        }
        else lastraw = simvalue;
    }
#endif
    
    if (lastraw <= 0) {
        lastPressure = 0;
    }
    else {
        // extrapolate actual value from table
        const int bits = adc_read_bits - expPressureBits;
        const int res = 1 << bits;
        const int mask = res - 1;
        int index = lastraw >> bits;
        int scale = lastraw & mask;
#ifdef USE_CONST_PRESSURE_MAP
        int val1 = expPressureMap[index] * (res - scale);
        int val2 = expPressureMap[index+1] * (scale);
#else
        int val1 = pressuremap[index] * (res - scale);
        int val2 = pressuremap[index+1] * (scale);
#endif
        lastPressure = ((val1 + val2) * pressure_adjust) >> (pressure_adjust_scale + bits);
    }

    led.volume(lastPressure);
    
    return lastPressure;
}

#ifdef USE_CONST_PRESSURE_MAP
void Pressure::buildPressureMap() {
    pressure_adjust = settings.pressurefactor * (1<<pressure_adjust_scale);
}
#else
void Pressure::buildPressureMap() {
    int16_t *map = pressuremap;
    float factor = table_exponent * (1.0F - settings.pressurefactor);
        
    for (int i = 0; i < pressurecount; i++) {
        if (i > table_top) {
            map[i] = table_max;
        }
        else {
            float adj = i / table_top;
            float val = pow(adj, factor) * table_max; // + 1.0;
            if (val > table_max)
                map[i] = table_max;
            else
                map[i] = val;
        }
    }
}
#endif

/**
 * @brief Initialize pressure sensor and settings
 * 
 */
void Pressure::initPressure() {

#ifdef USE_AIR_MPRLS

    const int minpressure = 5000;

    if (mprls_valid) return;

    AudioNoInterrupts();
    mprls_valid = mprls.begin(0x18, &Wire);
    delay(2);
    base_pressure = mprls.readData() + minpressure;
    AudioInterrupts();
    
#else

    const int minpressure = 0; //adc_read_scale * 0.02;
    
    pinMode(BREATH_PIN, INPUT);
    
    //delay(10);
    base_pressure = 0;
    for (int i = 0; i < 10; i++) {
        delay(1);
        unsigned int v = analogRead(BREATH_PIN) + minpressure;
        if (v > base_pressure)
            base_pressure = v;
    }

    base_scale = (float(adc_read_scale) / float(adc_read_scale - base_pressure)) * (1<<base_bits);

#endif
    
    buildPressureMap();
}
