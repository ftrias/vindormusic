#ifndef LIP_PRESSURE_H
#define LIP_PRESSURE_H

#ifdef LIP_PRESSURE_PIN

class LipPressure {
    public:
    static void begin();
    static void collectData();
    static int getPressure();
    static void process();
    static void debug();
    static bool enabled;
};

extern LipPressure lip;

#endif

#endif