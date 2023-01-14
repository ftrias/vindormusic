#include "firmware.h"

#include "Lights.h"
#include "Display.h"

void Lights::begin(int minimal) {
    pinMode(LED_RED_PIN, OUTPUT);
    pinMode(LED_GREEN_PIN, OUTPUT);
    pinMode(LED_BLUE_PIN, OUTPUT);
    alloff();
    
    pinMode(AMP_PIN, OUTPUT);
    digitalWrite(AMP_PIN, LOW);
}
void Lights::write_pwm(int pin, int value) {
#ifdef LED_COMMON_ANODE
    analogWrite(pin, adc_write_scale-value);
#else
    analogWrite(pin, value);
#endif
}
void Lights::write_digital(int pin, int value) {
#ifdef LED_COMMON_ANODE
    digitalWrite(pin, value?LOW:HIGH);
#else
    digitalWrite(pin, value?HIGH:LOW);
#endif
}
// void Lights::speaker(int state) {
//     // showDebug(state?"Amplifier on":"Amplifier off");
//     digitalWrite(AMP_PIN, state?HIGH:LOW);
// }
void Lights::volume(int level) { // level 16 bit
    write_pwm(LED_BLUE_PIN, level >> (16 - adc_write_bits));
}

void Lights::flash(int count, int delay, int delayoff) {
    blink_count = count;
    setBlink(delay, delayoff);
}
void Lights::setBlink(int delayon, int delayoff) {
    blink_on = delayon;
    blink_off = delayoff;
}
void Lights::setBlinkOff() {
    blink_on = 0;
    blink_off = 0;
    on(blink_save, 0);
}
void Lights::processLoop() {
    static int blink_state;
    static long blink_time;
    if (blink_on) {
        long tm = millis() - blink_time;
        if (blink_state && tm > blink_on) { // turn off
            blink_state=0;
            blink_time = millis();
            on(blink_state, 0);
            if (blink_count > 0) { // are we counting blinks?
                blink_count--;
                if (blink_count==0) {
                    setBlinkOff();
                }
            }
        }
        else if (blink_state==0 && tm > blink_off) { // turn on
            blink_state = 1;
            blink_time = millis();
            on(blink_state, 0);
        }
    }
}
void Lights::color(int col, int state) {
    switch(col) {
        case 1: write_pwm(LED_RED_PIN, state); break;
        case 2: write_pwm(LED_BLUE_PIN, state); break;
        case 3: write_pwm(LED_GREEN_PIN, state); break;
    }
}
void Lights::on(int state, int save) {
    // digitalWrite(LED_GREEN_PIN, state?LOW:HIGH);
    write_pwm(LED_GREEN_PIN, state?127:0);
    if (save) blink_save = state;
}
void Lights::usb(int state) {
    //digitalWrite(LED_BLUE_PIN, state);
}
void Lights::midi(int state) {
    //digitalWrite(LED_BLUE_PIN, state);
}
void Lights::headphones(int state) {
    //digitalWrite(LED_BLUE_PIN, state);
}
void Lights::sleep(int state) {
    int level = state * 128;
    write_pwm(LED_BLUE_PIN, level);
}
void Lights::wait(int state) {
    write_pwm(LED_BLUE_PIN, state?127:0);
}
void Lights::error(int state) {
    write_pwm(LED_RED_PIN, state?127:0);
}
void Lights::alloff() {
    write_pwm(LED_RED_PIN, 0);
    write_pwm(LED_GREEN_PIN, 0);
    write_pwm(LED_BLUE_PIN, 0);
}
void Lights::bars(int num) {}
void Lights::blink(int number) {
    on(0);
    delay(1000);
    for (int i = 0; i < number; i++) {
        on(1);
        delay(200);
        on(0);
        delay(200);
    }
    delay(2000);
    on(1);
}
