#include "firmware.h"
#include "AudioPlayUtils.h"

const int16_t MuLawDecompressTable[256] =
{
    -32124,-31100,-30076,-29052,-28028,-27004,-25980,-24956,
    -23932,-22908,-21884,-20860,-19836,-18812,-17788,-16764,
    -15996,-15484,-14972,-14460,-13948,-13436,-12924,-12412,
    -11900,-11388,-10876,-10364, -9852, -9340, -8828, -8316,
    -7932, -7676, -7420, -7164, -6908, -6652, -6396, -6140,
    -5884, -5628, -5372, -5116, -4860, -4604, -4348, -4092,
    -3900, -3772, -3644, -3516, -3388, -3260, -3132, -3004,
    -2876, -2748, -2620, -2492, -2364, -2236, -2108, -1980,
    -1884, -1820, -1756, -1692, -1628, -1564, -1500, -1436,
    -1372, -1308, -1244, -1180, -1116, -1052,  -988,  -924,
    -876,  -844,  -812,  -780,  -748,  -716,  -684,  -652,
    -620,  -588,  -556,  -524,  -492,  -460,  -428,  -396,
    -372,  -356,  -340,  -324,  -308,  -292,  -276,  -260,
    -244,  -228,  -212,  -196,  -180,  -164,  -148,  -132,
    -120,  -112,  -104,   -96,   -88,   -80,   -72,   -64,
    -56,   -48,   -40,   -32,   -24,   -16,    -8,     -1,
    32124, 31100, 30076, 29052, 28028, 27004, 25980, 24956,
    23932, 22908, 21884, 20860, 19836, 18812, 17788, 16764,
    15996, 15484, 14972, 14460, 13948, 13436, 12924, 12412,
    11900, 11388, 10876, 10364,  9852,  9340,  8828,  8316,
    7932,  7676,  7420,  7164,  6908,  6652,  6396,  6140,
    5884,  5628,  5372,  5116,  4860,  4604,  4348,  4092,
    3900,  3772,  3644,  3516,  3388,  3260,  3132,  3004,
    2876,  2748,  2620,  2492,  2364,  2236,  2108,  1980,
    1884,  1820,  1756,  1692,  1628,  1564,  1500,  1436,
    1372,  1308,  1244,  1180,  1116,  1052,   988,   924,
    876,   844,   812,   780,   748,   716,   684,   652,
    620,   588,   556,   524,   492,   460,   428,   396,
    372,   356,   340,   324,   308,   292,   276,   260,
    244,   228,   212,   196,   180,   164,   148,   132,
    120,   112,   104,    96,    88,    80,    72,    64,
    56,    48,    40,    32,    24,    16,     8,     0
};

const int8_t MuLawCompressTable[256] =
{
     0,0,1,1,2,2,2,2,3,3,3,3,3,3,3,3,
     4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
     5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
     5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
     6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
     6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
     6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
     6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
     7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
     7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
     7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
     7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
     7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
     7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
     7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
     7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7
};

float AudioPlayUtils::LinearInterpolate(float y1, float y2, float mu) {
    return(y1*(1-mu)+y2*mu);
}

int AudioPlayUtils::LinearInterpolateDivF(int y1, int y2, float mu, float div) {
    return LinearInterpolate(y1, y2, mu/div);
}

int AudioPlayUtils::LinearInterpolateDiv(int y1, int y2, int x, int x2) {
    return y1 + (((y2 - y1)*x) / x2);
}

float AudioPlayUtils::CubicInterpolate(float y0, float y1, float y2, float y3, float mu)
{
    float a0,a1,a2,a3,mu2;
    mu2 = mu*mu;
    a0 = y3 - y2 - y0 + y1;
    a1 = y0 - y1 - a0;
    a2 = y2 - y0;
    a3 = y1;
    return(a0*mu*mu2+a1*mu2+a2*mu+a3);
}

int AudioPlayUtils::CubicInterpolateDivF(int y0, int y1, int y2, int y3, float mu, float div) {
    return CubicInterpolate(y0, y1, y2, y3, mu/div);
}

int AudioPlayUtils::CubicInterpolateDiv(int y0, int y1, int y2, int y3, int x, int x1) {
    int a0,a1,a2,a3;
    a0 = y3 - y2 - y0 + y1;
    a1 = y0 - y1 - a0;
    a2 = y2 - y0;
    a3 = y1;
    return(((a0*x/x1 + a1)*x/x1 + a2)*x/x1 + a3);
}

int AudioPlayUtils::interpolate(int x, int x1, int y1, int x2, int y2, int x3, int y3)
{
    return  y1 * (x-x2)*(x-x3) / ((x1-x2)*(x1-x3)) +
    y2 * (x-x1)*(x-x3) / ((x2-x1)*(x2-x3)) +
    y3 * (x-x1)*(x-x2) / ((x3-x1)*(x3-x2));
}

int AudioPlayUtils::cushion(int actual, int desired, int noise, int rate) {
    if (actual != desired) {
        int change = desired - actual;
        if (rate > 0) {
            if (change>rate || change<-rate) {
                actual += change/rate;
            }
            else {
                actual = desired;
            }
        }
        else {
            rate = -rate; // make rate positive
            if (change>rate) {
                actual += rate;
            }
            else if (change<-rate) {
                actual -= rate;
            }
            else {
                actual = desired;
            }
        }
    }
    return actual;
}

int AudioPlayUtils::cushion2(int actual, int desired, int factor) {
    //  #ifndef ARDUINO
    // const int factor = 40;
    int change = desired - actual;
    if (change < factor && change > -factor) return desired;
    return actual + change / factor;
    //  #endif
}

int AudioPlayUtils::cushion3(int actual, int desired) {
    if (desired < actual) return actual-1;
    if (desired > actual) return actual+1;
    return desired;
}

const int cBias = 0x84;
const int cClip = 32635;

uint8_t AudioPlayUtils::MuLawCompress(int16_t sample) {
     int sign = (sample >> 8) & 0x80;
     if (sign)
          sample = -sample;
     if (sample > cClip)
          sample = cClip;

     sample = (short)(sample + cBias);
     int exponent = (int)MuLawCompressTable[(sample>>7) & 0xFF];
     int mantissa = (sample >> (exponent+3)) & 0x0F;

     int compressedByte = ~ (sign | (exponent << 4) | mantissa);

     return (int8_t)compressedByte;
}