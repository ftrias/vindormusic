#ifndef Pressure_H
#define Pressure_H

class Pressure {
    
private:
    // const static int pressurebits = 12;
    // const static int pressurescale = 1200;
    // const static int pressuremax = 1023;


    
public:
#ifdef USE_CONST_PRESSURE_MAP
    const float table_max = 65535; // maximum resulting volume
    const float table_top = adc_read_scale; // maximum reading
#else
    NOT USED - BUGGY
    const static int pressurecount = 1024; // must be same as analogRead range, including adjustments
    int16_t pressuremap[pressurecount];
    const float table_exponent = 1.6; // exponential scale
    const float table_max = 1023; // maximum resulting volume
    const float table_top = 800; // maximum reading
#endif
    
public:
    int lastPressure;
    
    int lastraw;
    
    /**
     * @brief Get the current pressure object, including post-processing
     * 
     * @param myval Value to use instead of actual reading
     * @return int 
     */
    int getPressure(int myval = 0);
    
    /**
     * @brief If pressure ROM pressure map is not used, calculate new curve based on
     * current readings. Otherwise, set an adjustment factor.
     * 
     */
    void buildPressureMap();
    
    /**
     * @brief Initialize pressure sensor
     * 
     */
    void initPressure();
    
};

extern Pressure pressure;

#endif
