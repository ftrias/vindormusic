#ifndef FMEMDEBUG_H
#define FMEMDEBUG_H

#ifdef DEBUGITM

void ITM_init(void);
int32_t ITM_SendChar (int32_t ch);

#endif

#ifndef DEBUGMEM

#define TRACESTACK
#define TRACEX(x)
#define TRACEMEM(x)

#else

#include <stdint.h>

#define TRACESTACK volatile trace_stack my_trace_stack##__LINE__(__FILE__, __FUNCTION__, __LINE__);
#define TRACEX(x) volatile trace_stack my_trace_stack##__LINE__(NULL, NULL, x);
#define TRACEMEM(x) trace_memory my_trace_memory_##x(#x);

class trace_stack {
public:
    trace_stack(const char *file, const char *fxn, int line);
    ~trace_stack();
    static void print_stack();
private:
    const uint32_t trace_mark_value = 0xFAFBFCFD;
    uint32_t trace_mark;
    int trace_num;
};

class trace_memory {
private:
    const uint32_t trace_mark_value = 0xFAFBFCFD;
    uint32_t trace_mark;
    const char *trace_tag;
public:
    trace_memory(const char * tag);
    ~trace_memory();
    void verify();
    static void verify_all();
};

void dump_obj(void *obj, unsigned int sz);

#endif // DEBUGMEM

#ifndef DEBUGTIMING
#define TIMING(x);
#else
#define TIMING(x) volatile trace_timing my_trace_timing##__LINE__(x);

class trace_timing {
private:
    long startmicrosec;
    const char *trace_tag;
public:
    trace_timing(const char * tag);
    ~trace_timing();
};
#endif

extern void debug_crash_init();

#endif // FMEMDEBUG_H
