#include "firmware.h"

#include "Power.h"
#include "Lights.h"
#include "Dispatch.h"
#include "Event.h"

void power_off_device();
extern Lights led;
extern Dispatch dispatch;

#ifdef USE_VARIABLE_CHARGER
#include "MCP4551.h"
MCP4551 chargeresistor(0x2c);
#endif

// extern void sayNumbers(int num);
extern void showMessage(const char *msg);

#include "vdebug.h"

volatile int testVoltId;

#ifdef PIN_BATTERY
const int voltMax = 4000;
const int cutoffPower = 2900;
const int cutoffWarn = 3100;
const int cutoffError = 3000;
#else
const int voltMax = 3250;
const int cutoffPower = 3000;
const int cutoffWarn = 3000;
const int cutoffError = 2900;
#endif

void Power::begin() {
    pinMode(AUDIO_LDO_PIN, OUTPUT);
    digitalWrite(AUDIO_LDO_PIN, HIGH);
#ifdef PIN_BATTERY
    pinMode(PIN_BATTERY, INPUT);
#endif
    pinMode(PIN_INTERNAL_REF, INPUT);
#ifdef __MK20DX256VLL10__
    // remove protection
    SMC_PMPROT = 0b00101010;
#endif
    // usbChargerDetect();
}

void Power::setPeripherals(int state) {
    if (state) {
        digitalWrite(AUDIO_LDO_PIN, HIGH);
    }
    else {
        digitalWrite(AUDIO_LDO_PIN, LOW);
    }
}

#ifdef PIN_BATTERY

#ifdef USE_VARIABLE_CHARGER
int old_charge_state;
void Power::chargerOff() {
    // PlugState::setChargeRate(1);
    old_charge_state = chargeresistor.getWiper();
    chargeresistor.setWiper(128);
}
void Power::chargerOn() {
    // PlugState::setChargeRate(1);
    chargeresistor.setWiper(old_charge_state);
}
#else
void chargerOff() {}
void chargerOn() {}
#endif

int getDebugVoltage() {
    uint32_t x2 = analogRead(PIN_BATTERY);
    return x2;
}

int Power::getDirectVoltage() {
    // this reads voltages below 3.3V
    // const uint32_t basevolt = 1195; // per spec, ref volat is 1.195 V
    // const uint32_t basevolt = 1250; // from measurements
    uint32_t x = analogRead(PIN_INTERNAL_REF);
    // debugOutputDecimal2(0x11, 0, x);
    x >>= (adc_read_bits - 12);
    // complex model from PJRC forum (https://forum.pjrc.com/threads/26117-Teensy-3-1-Voltage-sensing-and-low-battery-alert?p=69205#:~:text=The%20analogRead(39)%20has%20an,voltage%20of%20about%204.1V.)
    int mV = (178*x*x + 2688757565 - 1184375 * x) / 372346;
    // int mV = basevolt * adc_read_scale / x; // basic model
    // debugOutputDecimal(mV);
    if (mV < 3300) return mV;

    // this reads voltages above 3.3V through a 1/2 voltage divider
    const uint32_t basevolt2 = 3450; // ref level voltage
    uint32_t x2 = analogRead(PIN_BATTERY);
    // debugOutputDecimal2(0x22, 0, x2);
    int mV2 = x2 * 2 * basevolt2 / adc_read_scale; // basic model (with 1/2 voltage divider)
    return mV2;
}

#else // PIN_BATTERY

int Power::getDirectVoltage() { // for Teensy 3.x, only valid up to 3.5V. Returns in millivolts.
    uint32_t x = analogRead(PIN_INTERNAL_REF);
    const uint32_t basevolt = 1195; // per spec, ref volt is 1.195 V
    return basevolt * adc_read_scale / x; // basic model
}

#endif // PIN_BATTERY

extern int playingNote;

int Power::getInputVoltage() { // for Teensy 3.x, only valid up to 3.5V. Returns in millivolts.
    // returns a running average for readingsMax readings
    const int runlength = 5;
    const int readinterval = 5000;
    static int average = -1;
    static unsigned int nextread = 0;
    // debugOutputDecimal(measure);
    if (average == -1) {
        average = getDirectVoltage();
        return average;
    }
    else if (playingNote <= 0) { // nothing playing because it messes up reading
        if (millis() > nextread) {
            nextread = millis() + readinterval;
            int measure = getDirectVoltage();
            average = (average * runlength + measure) / (runlength+1);
        }
    }
    return average;
}

float Power::volt() {
    return ((float) getInputVoltage()) / 1000.0;
}

int Power::getBatteryPercent() {
    int measure = getInputVoltage();
    if (measure > voltMax) return 100;
    if (measure < cutoffPower) return 0;
    int perc =  100 - (voltMax - measure) * 100 / (voltMax - cutoffPower);
    perc = (perc * perc * perc) / 10000; // put it on a x^3 curve
    if (perc > 100) perc = 100; // just in case, but not really needed
    return perc;
}

#ifdef USE_SLEEP_MODE

void Power::powerDownCPU(int times) {

    SMC_PMCTRL = SMC_PMCTRL_STOPM( 0x04 ) ;
    ( void ) SMC_PMCTRL;
    SMC_VLLSCTRL =  SMC_VLLSCTRL_VLLSM( 0x01 );
    ( void ) SMC_VLLSCTRL;

    // Now execute the __stop instruction to go into VLLS0
    SYST_CSR &= ~SYST_CSR_TICKINT;
    // Set the SLEEPDEEP bit to enable deep sleep mode (STOP)
    SCB_SCR = SCB_SCR_SLEEPDEEP;
    // WFI instruction will start entry into STOP mode
    asm volatile( "wfi" );
    // in case of problem shutting down, just try to sleep
    // as much as possible
    while(1) { asm volatile( "wfi" ); }
    
    // // Clear the SLEEPDEEP bit
    // SCB_SCR = 0;
    // // renable systick timer interrupt
    // SYST_CSR |= SYST_CSR_TICKINT;

}

void Power::testVolt(int minimal) {    
    static int warning;
    int v = getInputVoltage(); // in mV
    
#ifdef SKIP_POWER_CHECK
    return;
#endif
    
    if (v <= cutoffPower) { // H/L
        showMessage("LOW BATTERY - POWER OFF");
        delay(50); // wait for message to appear
        power_off_device();
    }
    else if (v <= cutoffError) {
        if (warning != 1) {
            // sayNumbers(1);
            warning=1;
            led.setBlink(100, 600);
        }
    }
    else if (v <= cutoffWarn) {
        if (warning != 2) {
            // sayNumbers(2);
            warning=2;
            led.setBlink(400, 400);
        }
    }
    else {
        if (warning) {
            // sayNumbers(0);
            led.setBlinkOff();
            warning=0;
        }
    }
}

void Power::testVoltPrep(void *p) {
#ifdef USE_VARIABLE_CHARGER
    chargerOff();
#endif
    dispatch.trigger(testVoltId, 200);
}

void Power::testVoltDispatch(void *p) {
    testVolt();
#ifdef USE_VARIABLE_CHARGER
    chargerOn();
#endif
}

void Power::idleSleep(int level) {        
    // TODO!!!! Power-save mode
}

#else

void Power::idleSleep(int x) { }

#endif
