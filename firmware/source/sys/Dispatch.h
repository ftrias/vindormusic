/*
 * Dispatch class
 */

#ifndef Dispatch_H
#define Dispatch_H

class Dispatch {
public:
    typedef void (*callback)(void *);
protected:
    const static int max = 15;
    typedef enum { NONE = 0, FUNC = 1, FLAG = 2, VAR = 3, TIMERFLAG = 4 } calltype;
    int active[max];
    calltype type[max];
    callback fxn[max];
    void *pdata[max];
    void *pdata2[max];
    union {
        struct {
            int count[max];
            int reset[max];
            int last;
        };
        volatile bool *var[max];
        volatile bool flag[max];
    };
public:
    Dispatch();
    
    int attach(callback f, void *data, volatile bool *fg);
    int attach(callback f, void *data);
    int attach(callback f, void *data, int interval_ms, void *data2 = NULL);
    template <typename F, typename D, typename P> int attach(F f, D d, P p) {
        return attach((callback)f, (void*)d, p);
    }
    void *get(int i);
    int find(callback f, int start = 0);
    void trigger(int i);
    void trigger(int i, int interval_ms);
    void setRate(int i, int interval_ms);
    void detach(int i);
    void detach(callback f);
    void process();
};

extern Dispatch dispatch;

#if 0 

template <int PIN> class ButtonDispatch {
protected:
    int id;
    int pin = PIN;
    volatile bool pending;
    
    void test_button() {
#ifdef DEBUGDISPATCH
        Serial.println("test button");
#endif
        int state = digitalRead(pin);        
        if (state == LOW) { 
            run();
        }
        pending = false;
    }
    
    static ButtonDispatch * me() {
        int i = 0;
        do {
            i = dispatch.find((Dispatch::callback)callback, i);
            if (i == -1) return NULL;
            ButtonDispatch *p = (ButtonDispatch *)dispatch.get(i);
            if (p->pin == PIN) return p;
            i++;
        } while (i >= 0);
        return NULL;
    }
    
    static void callback(ButtonDispatch *me) {
        me->test_button();
    }
    static void interrupt() {
        ButtonDispatch * p = me();
        p->process();
    }
    void process() {
#ifdef DEBUGDISPATCH
        Serial.println("interrupt");
#endif
        if (pending) return;
        pending = true;
        dispatch.trigger(id, 50);       
    }
public:
    ButtonDispatch() {
        id = dispatch.attach((Dispatch::callback)callback, this);
        pinMode(pin, INPUT_PULLUP);
        attachInterrupt(pin, interrupt, CHANGE);
    }
    void test() { test_button(); }
    virtual void run() = 0;
};

#endif

template <int PIN> class ButtonDispatch {
protected:
    int id;
    int pin = PIN;
    volatile int state;
    volatile int pending;
    volatile int currentstate;
    
    static ButtonDispatch * obj() {
        int i = 0;
        do {
            i = dispatch.find((Dispatch::callback)callback, i);
            if (i == -1) return NULL;
            ButtonDispatch *p = (ButtonDispatch *)dispatch.get(i);
            if (p->pin == PIN) return p;
            i++;
        } while (i >= 0);
        return NULL;
    }
    
    static void callback(ButtonDispatch *me) {
        if (! me->pending) return;
        me->pending = 0;
        if (me->state == LOW && me->currentstate == HIGH) {
            me->onPress();
        }
        else if (me->state == HIGH && me->currentstate == LOW) {
            me->onRelease();
        }
        me->currentstate = me->state;
    }

    static void interrupt() {
        auto me = obj();
        me->state = digitalRead(me->pin);    
        if (! me->pending) {  
            me->pending = 1;  
            dispatch.trigger(me->id, 25);
        }
    }

public:
    ButtonDispatch() {
        pending = 0;
        currentstate = HIGH;
        id = dispatch.attach((Dispatch::callback)callback, this);
        pinMode(pin, INPUT_PULLUP);
        attachInterrupt(pin, interrupt, CHANGE);
    }
    virtual void onPress() = 0;
    virtual void onRelease() { };
};

#endif
