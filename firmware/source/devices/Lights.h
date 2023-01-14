/*******
 * Lights
 *******/

#ifndef Lights_H
#define Lights_H

class Lights {
protected:
    int blink_on = 0;
    int blink_off = 0;
    int blink_save = 0;
    int blink_count = 0;
public:
    void begin(int minimal = 0);
    static void write_pwm(int pin, int value);
    static void write_digital(int pin, int value);
    void speaker(int state);
    void volume(int level);
    void flash(int count, int delay=300, int delayoff=400);
    void setBlink(int delayon=200, int delayoff=200) ;
    void setBlinkOff();
    void processLoop();
    void color(int col, int state);
    void on(int state, int save = 1);
    void usb(int state);
    void midi(int state);
    void headphones(int state);
    void sleep(int state);
    void wait(int state);
    void error(int state);
    void alloff() ;
    void bars(int num);
    void blink(int number);
};

extern Lights led;

#endif
