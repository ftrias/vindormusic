#include "firmware.h"
#include "perftimer.h"

#ifdef DEBUGPERFTIMER

unsigned long PerfTimeUse::elapse[256];
unsigned long PerfTimeUse::count[256];

void PerfTimer::mark(const char *txt, int id)
{
    if (id == -1) {
        id = next;
        next++;
    }
    if (first == 0) {
        first = micros();
        for (int i=0; i<count; i++) {
            slot[i] = 0;
        }
        slot[id] = first;
    }
    else {
        slot[id] = micros();            
    }
    marker[id] = txt;
}
int PerfTimer::getMax(int *idx)
{
    unsigned long max = slot[0];
    int maxi = 0;
    for (int id=1; id<count; id++) {
        if (slot[id] > max) {
            max = slot[id];
            maxi = id;
        }
    }
    if (idx) *idx = maxi;
    return (int) (max - first);
}
void PerfTimer::show()
{
    for (int i=0; i<count; i++) {
        if (slot[i] == 0) continue;
        Serial.print("PerfTimer slot ");
        Serial.print(i);
        Serial.print(" ");
        Serial.print(marker[i]);
        Serial.print(" ");
        Serial.print(slot[i] - first);
        Serial.print(" ");
        if (i>0) Serial.print(slot[i] - slot[i-1]);
        Serial.println();
    }
}

PerfTimer perf;

#endif
