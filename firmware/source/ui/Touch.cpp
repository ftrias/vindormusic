/**
 * @file Touch.cpp
 * @author Fernando Trias
 * @brief 
 * @version 0.1
 * @date 
 * 
 * @copyright Copyright (c) 2013-2021 by Vindor Music, Inc.
 * 
 */

#include "firmware.h"
#include "fdebug.h"
#include "Touch.h"
#include "Settings.h"
#include "Display.h"
#include "vutils.h"
#include "vdebug.h"

// Use the interrupt-based reading (as opposed to polling)
#define USEINTERRUPT

#ifdef DEBUGTOUCHSAVE
int touch_dump = 0;
int touch_save = 0;
const int touch_save_len = 1000;
const int touch_save_pins = 13; //pinCount;
int touch_save_idx = 0;
uint16_t touch_save_data[touch_save_pins][touch_save_len];
#endif

#ifdef SIMULATE
extern int simulate_on;
#endif

#ifdef PIN_MPR
/********
 * 
 * This section includes old code when using MPR121 touch sensor. Left in for use
 * in future.
 * 
 */

boolean *mpr_readTouchInputs();
void mpr_setup();

int *touchReadAll(bool cached = false) {
    static int state[pinCount];  
    boolean *x = mpr_readTouchInputs();
    for(int i=0; i<pinCount; i++) {
        state[i] = x[pinTouchMPR[i]];
    }
    return state;
}

#else

/*********
 * 
 * Below is the code for using the K20 built-in touch sensors
 * 
 */

/*
 Pin assignments
 Num  CPU_pin  CPU_id
 0 - PTB16 - TSI0_CH9
 1 - PTB17 - TSI0_CH10
 22 - PTC1  - TSI0_CH14
 23 - PTC2  - TSI0_CH15
 32 - PTB18 - TSI0_CH11
 16 - PTB0  - TSI0_CH0
 17 - PTB1  - TSI0_CH6
 33 - PTA4  - TSI0_CH5
 18 - PTB3  - TSI0_CH8
 19 - PTB2  - TSI0_CH7
 34 - PTA1  - TSI0_CH2
 15 - PTC0  - TSI0_CH13
 25 - PTB19 - TSI0_CH12
 
 CH1 - PTA0/SWD_CLK
 CH2 - PTA1  #34
 CH3 - PTA2  #35
 CH4 - PTA3/SWD_DIO
 */

#include <core_pins.h>

// Pin number to K20 pin mapping
static const uint8_t pin2tsi[] = {
    //0    1    2    3    4    5    6    7    8    9
    9,  10, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255,  13,   0,   6,   8,   7,
    255, 255,  14,  15, 255,  12, 255, 255, 255, 255,
    255, 255,  11,   5,   2,   3
};

/**
 * @brief Read single pin using given parameters. Used for debugging and tuning.
 * 
 * @param pin 
 * @param current 
 * @param nscan 
 * @param prescale 
 * @param refchg 
 * @return int 
 */
int touchReadTune(uint8_t pin, int current, int nscan, int prescale, int refchg)
{
    uint32_t ch;
    
    if (pin >= NUM_DIGITAL_PINS) return 0;
    ch = pin2tsi[pin];
    if (ch == 255) return 0;
    
    *portConfigRegister(pin) = PORT_PCR_MUX(0);
    SIM_SCGC5 |= SIM_SCGC5_TSI;
    TSI0_GENCS = 0;
    TSI0_PEN = (1 << ch);
    TSI0_SCANC = TSI_SCANC_REFCHRG(refchg) | TSI_SCANC_EXTCHRG(current);
    TSI0_GENCS = TSI_GENCS_NSCN(nscan) | TSI_GENCS_PS(prescale) | TSI_GENCS_TSIEN | TSI_GENCS_SWTS;
    delayMicroseconds(10);
    while (TSI0_GENCS & TSI_GENCS_SCNIP) ; // wait
    delayMicroseconds(1);
    return *((volatile uint16_t *)(&TSI0_CNTR1) + ch);
}

#ifdef USEINTERRUPT

// The interrupt loads into this space
volatile uint32_t lastTouchRead32[8];

// Data copied here when processing in case interrupt overwrites data
volatile uint16_t lastTouchRead[16];

// Flag indicating new data ready to be processed
volatile bool dataAvailable = 0;

/**
 * @brief Interrupt for TSI (touch sensing). Loads in lastTouchRead32 and
 * sets flag.
 * 
 */
extern "C" void tsi0_isr (void) {
    TSI0_GENCS |= TSI_GENCS_EOSF;
    if (dataAvailable) return; // don't override existing data
    for (int i=0; i<8; i++) {
        lastTouchRead32[i] = *((volatile uint32_t *)(&TSI0_CNTR1) + i);
    }
    dataAvailable = 1;
}

#endif

/**
 * @brief Configure the Touch sensors sensitity, prescale, etc
 * 
 * @param current Current level
 * @param nscan Number of scans to average over
 * @param prescale Prescale factor
 * @param refchg Reference charge
 */
void configTouch(int current, int nscan, int prescale, int refchg) {
    SIM_SCGC5 |= SIM_SCGC5_TSI;
    TSI0_GENCS = 0;
    
#ifdef USEINTERRUPT
    TSI0_THRESHOLD = (600 << 16) | 0;
    TSI0_SCANC = TSI_SCANC_AMCLKS(0) | TSI_SCANC_SMOD(20) | TSI_SCANC_REFCHRG(refchg) | TSI_SCANC_EXTCHRG(current);
    TSI0_GENCS = TSI_GENCS_NSCN(nscan) | TSI_GENCS_PS(prescale);
    TSI0_GENCS |= TSI_GENCS_LPSCNITV(15) | TSI_GENCS_TSIEN | TSI_GENCS_TSIIE | TSI_GENCS_ESOR | TSI_GENCS_STM | TSI_GENCS_STPE;
    NVIC_SET_PRIORITY(IRQ_TSI, 100);
    NVIC_ENABLE_IRQ(IRQ_TSI);
#else
    TSI0_SCANC = TSI_SCANC_REFCHRG(refchg) | TSI_SCANC_EXTCHRG(current);
    TSI0_GENCS = TSI_GENCS_NSCN(nscan) | TSI_GENCS_PS(prescale);
    TSI0_GENCS |= TSI_GENCS_TSIEN;
#endif
    
    // just in case, but not really needed because enableTouchPin() does this too
    PORTA_PCR1 = PORT_PCR_MUX(0); // | PORT_PCR_DSE | PORT_PCR_SRE;
}

/**
 * @brief Enable touch on pin
 * 
 * @param pin Pin number
 */
void enableTouchPin(uint8_t pin) {
    if (pin==255) return;
    uint32_t ch;
    ch = pin2tsi[pin];
    if (ch == 255) return;
    *portConfigRegister(pin) = PORT_PCR_MUX(0);
    TSI0_PEN |= (1 << ch);
}

/**
 * @brief Wait until data is available and discard it.
 * 
 * @return int 0 if timeout;  1 if success
 */
int waitbusy() {
#ifdef USEINTERRUPT
    int timestart = millis();
    while (! dataAvailable) {
        if (millis()-timestart>100) return 0;
    }	
    dataAvailable=0;
    return 1;
#endif
    
    if (TSI0_GENCS & TSI_GENCS_SCNIP) {
        int timeout = millis();
        while (TSI0_GENCS & TSI_GENCS_SCNIP) {
            if (millis()-timeout>10) return 0;
            delayMicroseconds(1); // wait if busy
        }
        delayMicroseconds(10);
    }
    return 1;
}

/**
 * @brief Read pins; if using interrupts, doesn't do anything.
 * 
 */
void startReadPins()
{
#ifndef USEINTERRUPT
    waitbusy();
    TSI0_GENCS |= TSI_GENCS_SWTS;
#endif
}

/**
 * @brief Wait until data is available indefinitely.
 * 
 */
void waitForData() {
    while(dataAvailable == 0) delayMicroseconds(10);
}

/**
 * @brief Copy last read values into out work space.
 * 
 * @return int 0 if no data; 1 if success.
 */
int touchGetAll() {
#ifdef USEINTERRUPT
    if (! dataAvailable) return 0;
    memcpy((void*)lastTouchRead, (void*)lastTouchRead32, sizeof(lastTouchRead));
    dataAvailable = 0;
#endif
    return 1;
}

int readTouchPin(uint8_t pin)
{
    if (pin==255) return 0;
    uint32_t ch;
    ch = pin2tsi[pin];
    if (ch == 255) return 0;
#ifdef USEINTERRUPT
    return lastTouchRead[ch];
#else
    waitbusy();
    return *((volatile uint16_t *)(&TSI0_CNTR1) + ch);
#endif
}

//
// Touch sensors
//

int stateTouch[pinCount];
int baseTouch[pinCount];
int lastTouch[pinCount];
int resetPin[pinCount];
int minTouch[pinCount];
int maxTouch[pinCount];
EMA<6> baseLine[pinCount];
EMA<4> averageTouch[pinCount];

int touchEnabledFlag = 0;

/**
 * @brief Configure and enable touch all pins to use interrupts
 * 
 */
void enableTouch() {
    // configure parameters
    configTouch();
    // enable touch pins
    for (int i = 0; i < pinCount; i++) { 
        enableTouchPin(pinTouch[i]);
    }
    attachInterruptVector(IRQ_TSI, tsi0_isr);
    touchEnabledFlag = 1;
}

/**
 * @brief Initialze touch system; called by initTouch().
 * 
 */
void beginTouch() {
    if (!touchEnabledFlag) enableTouch();
    
    startReadPins();
    waitbusy();
    waitForData();
    touchGetAll();

    for (int i = 0; i < pinCount; i++) { 
#ifdef USE_SAVE_TOUCH
        baseTouch[i] = settings.saveTouch[i];
#else
        baseTouch[i] = readTouchPin(pinTouch[i]);
#endif
        stateTouch[i] = 0;
        baseLine[i].set(baseTouch[i]);
        averageTouch[i].set(baseTouch[i]);
        resetPin[i] = 0;
        minTouch[i] = baseTouch[i];
    }  

#ifndef USEINTERRUPT
    startReadPins();
#endif
}

/**
 * @brief Read all pins and calculate on/off states
 * 
 * @param cached True to use previous results; default false.
 * @return int* Array of pin states (in  static memory)
 */
int *touchReadAll(bool cached) {
    if (cached) return stateTouch;

    if (touchGetAll()==0) return stateTouch;

#ifdef TOUCH_CALIB_HIGH
    const int cutstable = 20;
    const int cuton = 200;
    const int cutoff = 50; // offset above base for button OFF
#else
    const int cutstable = 10;
    const int cuton = 120;
    const int cutoff = 30; // offset above base for button OFF
#endif
    int v;

    for (int i = 0; i < pinCount; i++) { 
        v = readTouchPin(pinTouch[i]);

        if (v==0xFFFF) continue; // error on read
        lastTouch[i] = v; // for debugging
        
        // If our initial baseline is too high, then the user probably
        // was touching buttons during boot. Or something else bad
        // happened and we need to reset the baseline.
        averageTouch[i].update(v);
        if (averageTouch[i] < baseTouch[i]) {
            baseTouch[i] = averageTouch[i];
        }

        // Check for state transitions for finger
        if (stateTouch[i]) {
            if (v < baseTouch[i] + cutoff) {
                // finger off
                stateTouch[i] = 0;
                resetPin[i] = 0;
            }
        }
        else {
            resetPin[i]++;
            int change = v - baseTouch[i];
            if (change > cuton) {
                // above the cutoff, so we're on
                stateTouch[i] = 1;
            }
            else if (change < cutstable) {
                baseLine[i].update(v);
                if (resetPin[i] > 100) {
                    // reset baseline for env drift (grease, temp, humid)
                    // and also in case fingers were touching during boot
                    // debugOutputDecimal2(i, baseTouch[i], v);
                    baseTouch[i] = baseLine[i];
                    resetPin[i] = 0;
                }
            }
            else {
                resetPin[i] = 0;
            }
        }
    }

    // initialize next read of touch sensors
    startReadPins();
    
#ifdef DEBUGTOUCHSAVE
    if (touch_save) touch_save_idx++;
#endif

    return stateTouch;
}

#endif

/**
 * @brief Initialize touch sensors; called by framework
 * 
 */
void initTouch() {
#ifdef PIN_MPR
    
#ifdef DEBUG
    Serial.println("setup MPR");
#endif
    mpr_setup();
    
#else
    
    beginTouch(); // first time, will initialize
    
#endif
}  

/**
 * @brief Get ON/OFF state of all buttons (note and octave) as a 12-bit number.
 * Each it is a button. MSB to LSB: 2 bits are the octave buttons, then 
 * buttons 1-10.
 * 
 * @return uint16_t Bit field for buttons
 */
uint16_t getButtonsState() {
    uint16_t binState = 0;
    
    int *cap = touchReadAll();

#ifdef SIMULATE
    static int prevsim = 0;
    static long simlast = 0;
    if (simulate_on) {
        int32_t random(int32_t howsmall, int32_t howbig);
        if (millis() - simlast > 50) {
            binState = random(0,16383);
            prevsim = binState;
            simlast = millis();
        }
        else binState = prevsim;
        return binState;
    }
#endif

    int state;
    for (int i = 0; i < pinCount; i++) { 
        state = cap[i] > 0 ? 1 : 0;
        binState = (binState << 1) | state;
    }
    
    return binState;
}
