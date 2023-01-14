#ifndef AudioPlayUtils_h
#define AudioPlayUtils_h

#include <stdint.h>

extern const int16_t MuLawDecompressTable[256];
extern const int8_t MuLawCompressTable[256];

class AudioPlayUtils
{
public:
    static int cushion(int actual, int desired, int noise, int rate);
    static int cushion2(int actual, int desired, int factor);
    static int cushion3(int actual, int desired);
    static int interpolate(int x, int x1, int y1, int x2, int y2, int x3, int y3);
    
    static float LinearInterpolate(float y1, float y2, float mu);
    static int LinearInterpolateDivF(int y1, int y2, float mu, float div);
    static int LinearInterpolateDiv(int y1, int y2, int x, int x2);
    
    static float CubicInterpolate(float y0, float y1, float y2, float y3, float mu);
    static int CubicInterpolateDivF(int y0, int y1, int y2, int y3, float mu, float div);
    static int CubicInterpolateDiv(int y0, int y1, int y2, int y3, int x, int x1);	

    static uint8_t MuLawCompress(int16_t sample);
};

#endif
