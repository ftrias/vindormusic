#ifndef PERFTIMER_H
#define PERFTIMER_H

#ifdef DEBUGPERFTIMER

class PerfTimer {
private:
    const static int count = 64;
    unsigned long slot[count];
    const char *marker[count];
    unsigned long first;
    int next;
public:
    PerfTimer() {
        next = 0;
    }
    void mark(const char *txt, int id=-1);
    int getMax(int *idx);
    void show();
};

class PerfTimeUse {
protected:
    unsigned long timestart;
    int slot;
public:
    static unsigned long elapse[256];
    static unsigned long count[256];

    PerfTimeUse(int id) { 
        timestart = micros();
        slot = id;
    }
    ~PerfTimeUse() {
        count[slot]++;
        elapse[slot] += micros() - timestart;
    }
    static void start(int id) { 
        count[id]++;
        elapse[id] -= micros();
    }
    static void stop(int id) {
        elapse[id] += micros();
    }
    static void begin() {
        for(int i=0; i<256; i++) elapse[i] = count[i] = 0;
    }
    static void dump() {
        for(int i=0; i<256; i++) {
            if (elapse[i] != 0) {
                Serial.print(i);
                Serial.print(" ");
                Serial.println(elapse[i]);
            }
        }
    }
};

extern PerfTimer perf;

// #define DEBUGPERFTIMER

#define PERFTIMER(x) {perf.mark(x);};
#define PERFTIMERDUMP {perf.show();};

#else

#define PERFTIMER(x)
#define PERFTIMERDUMP

#endif

#endif
