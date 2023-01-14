#ifndef TOUCH_H
#define TOUCH_H

#include <stdint.h>
void initTouch();
int *touchReadAll(bool cached = false);
uint16_t getButtonsState();

#ifdef TOUCH_PAD_SPACE
const int touchCurrent = 10;
const int touchNscan = 4;
const int touchPrescale = 5;
const int touchRefchg = 2;
#else
const int touchCurrent = 7;
const int touchNscan = 2;
const int touchPrescale = 3;
const int touchRefchg = 2;
#endif


#if PIN_LAYOUT==10
const int pinCount = 14;
const int pinTouch[] = {35,22,23,0,1,32,33,25,25,18,19,34,15,25};
#elif PIN_LAYOUT==9
const int pinCount = 14;
const int pinTouch[] = {35,22,23,0,1,32,33,16,17,18,19,34,15,25};
#elif PIN_LAYOUT==8
const int pinCount = 13;
const int pinTouch[] = {22,23,0,1,32,33,16,17,18,19,34,15,25};
#elif PIN_LAYOUT==7
const int pinCount = 13;
const int pinTouch[] = {0,1,22,23,32,33,16,17,18,19,33,15,25};
#elif PIN_LAYOUT==6
const int pinCount = 13;
const int pinTouch[] = {1,0,22,23,32,33,16,17,18,19,33,15,25};
#elif PIN_LAYOUT==5
const int pinCount = 13;
const int pinTouch[] = {0,1,22,23,32,33,16,17,18,19,33,15,25};
#elif PIN_LAYOUT==4
const int pinCount = 13;
const int pinTouch[] = {0,1,22,23,25,32,16,17,18,19,32,15,33};
#elif PIN_LAYOUT==3
const int pinCount = 13;
const int pinTouch[] = {33,32,0,1,25,23,16,17,18,19,23,15,22};
#elif PIN_LAYOUT==2
const int pinCount = 12;
const int pinTouch[] = {32,33,0,1,25,16,17,18,19,23,15,22};
#elif PIN_LAYOUT==MPR
const int pinCount = 13;
const int pinTouchMPR[] = {0,1,2,3,4,9,5,6,7,8,9,10,11};
#else
// pins used for each button, in order: 2 octaves, key closest to mouth, to farthest
const int pinCount = 12;
const int pinTouch[] = {32,33,0,1,15,16,17,18,19,22,23,25};
#endif

extern int baseTouch[];
extern int lastTouch[];
extern int resetPin[];

int touchReadTune(uint8_t pin, int current, int nscan, int prescale, int refchg);
int touchRead2(uint8_t pin);

void configTouch(int current=touchCurrent, int nscan=touchNscan, 
                 int prescale=touchPrescale, int refchg=touchRefchg);
void enableTouchPin(uint8_t pin);
void startReadPins();
int readTouchPin(uint8_t pin);
void enableTouch();

void dispatchDebugTouch();

#endif
