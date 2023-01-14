#ifndef WatchDog_H
#define WatchDog_H
class WatchDog {
private:
    int timeoutmsec;
    int initflag;
    unsigned int lastkick;
public:
    WatchDog() : initflag(0) {};
    void begin(int ms=5000);
    void kick();
private:
    void init();
};
extern WatchDog watchdog;
#endif
