#ifdef ARDUINO

// #include <i2c_t3.h>
// #define TwoWire i2c_t3

#include <Wire.h>

#ifdef __MK20DX256__
#define Wire Wire1
#endif

// #define BUFFER_LENGTH 8

#endif
