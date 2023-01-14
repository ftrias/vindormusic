#include "firmware.h"
#include "Dispatch.h"

extern Dispatch dispatch;

Dispatch::Dispatch() {
    last = millis();
}
int Dispatch::attach(callback f, void *data, int interval_ms, void *data2) {
    for (int i = 0; i < max; i++) {
        if (active[i]) continue;
        type[i] = FUNC;
        fxn[i] = f;
        count[i] = interval_ms;
        reset[i] = interval_ms;
        pdata[i] = data;
        pdata2[i] = data2;
        active[i] = 1;
        flag[i] = 0;
#ifdef DEBUGDISPATCH
        Serial.print("attach ");
        Serial.print((int)f);
        Serial.print(" ever ");
        Serial.print(interval_ms);
        Serial.println();
#endif
        return i;
    }
    return -1;
}

int Dispatch::attach(callback f, void *data, volatile bool *fg) {
    for (int i = 0; i < max; i++) {
        if (active[i]) continue;
        type[i] = VAR;
        fxn[i] = f;
        pdata[i] = data;
        var[i] = fg;
        active[i] = 1;
        flag[i] = 0;
        return i;
    }
    return -1;
}

int Dispatch::attach(callback f, void *data) {
    for (int i = 0; i < max; i++) {
        if (active[i]) continue;
        type[i] = FLAG;
        fxn[i] = f;
        pdata[i] = data;
        flag[i] = 0;
        active[i] = 1;
        return i;
    }
    return -1;
}

void *Dispatch::get(int i) {
    return pdata[i];
}

int Dispatch::find(callback f, int start) {
    for (int i = start; i < max; i++) {
        if (! active[i]) continue;
        if (f == fxn[i]) return i;
    }
    return -1;       
}

void Dispatch::trigger(int i) {
    flag[i] = true;
}

void Dispatch::trigger(int i, int interval_ms) {
    type[i] = TIMERFLAG;
    count[i] = reset[i] = interval_ms;
}

void Dispatch::setRate(int i, int interval_ms) {
    count[i] = reset[i] = interval_ms;
}

void Dispatch::detach(int i) {
    active[i] = 0;
}
void Dispatch::detach(callback f) {
    for (int i = 0; i < max; i++) {
        if (active[i] && fxn[i] == f) {
            active[i] = 0;
            return;
        }
    }
}
void Dispatch::process() {
    int now = millis();
    int elapse = now - last;
    last = now;
    for (int i = 0; i < max; i++) {
        if (! active[i]) continue;
        if (type[i]==FLAG) {
            if (flag[i]) {
                fxn[i](pdata[i]);
                flag[i] = 0;
            }
        }
        else if (type[i] == VAR) {
            if (*var[i]) {
                fxn[i](pdata[i]);
                *var[i] = 0;
            }
        } 
        else if (type[i] == FUNC || type[i] == TIMERFLAG) {
            count[i] -= elapse;
            if (count[i] > 0) continue;
            fxn[i](pdata[i]);
            if (type[i] == TIMERFLAG) {
                type[i] = FLAG;
                flag[i] = false;
            }
            else {
                count[i] += reset[i];
            }
        }
    }
}
