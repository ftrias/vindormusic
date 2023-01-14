#include "Arduino.h"

#include "fdebug.h"
#include "fmemdebug.h"

void blink(int number);

#ifdef TEST
extern "C" void debug_out(const char *s) {
    if (s==NULL) {
        Serial.println();
    }
    else {
        Serial.print(s);
    }
}
extern "C" void debug_out_int(int s) {
    Serial.print(s);
}
#endif

/*
 * Extra for working with stack checker and other debugging
 */
extern "C" int _kill(pid_t pid, int sig) {
    __asm volatile("BKPT");
    return  0;
}

extern "C" int _getpid()  {
    return 0;
}

/*
 * ITM debugging info
 */

#ifdef DEBUG_ITM

#define ITM_Port8(n)    (*((volatile unsigned char *)(0xe0000000+4*n)))
#define ITM_Port16(n)   (*((volatile unsigned short*)(0xe0000000+4*n)))
#define ITM_Port32(n)   (*((volatile unsigned long *)(0xe0000000+4*n)))

#define ITM_TER   (*((unsigned long *) 0xe0000e00))
#define ITM_TCR   (*((unsigned long *) 0xe0000e80))

#define DEMCR           (*((volatile unsigned long *)(0xe000EDFC)))
#define TRCENA          0x01000000

int32_t ITM_SendChar (int32_t ch) {
    if ((ITM_TCR & (1UL << 0)) &&         /* ITM enabled */
        (DEMCR & TRCENA) &&
        (ITM_TER & (1UL << 0)))
    {
        while (ITM_Port32(0) == 0);
        ITM_Port8(0) = (uint8_t)ch;
    }
    return (ch);
}

// Debug Exception Monitor and Control register
// #define DEMCR   (*((unsigned long *) 0xe000edfc))
#define TRCENA  0x01000000  // enable trace

// Stimulus Port registers
#define ITM_STIM0 (*((unsigned long *) 0xe0000000))
#define ITM_STIM1 (*((unsigned long *) 0xe0000004))
#define ITM_STIM2 (*((unsigned long *) 0xe0000008))
#define ITM_STIM3 (*((unsigned long *) 0xe000000c))

// Trace enable registers
#define ITM_TER   (*((unsigned long *) 0xe0000e00))

// Privilege register: registers that can be accessed by unprivileged code
#define ITM_TPR   (*((unsigned long *) 0xe0000e40))

// Trace Control register
#define ITM_TCR   (*((unsigned long *) 0xe0000e80))

// Lock Access register
#define ITM_LAR   (*((unsigned long *) 0xe0000fb0))

// unlock value
#define ITM_LAR_ACCESS  0xc5acce55 

void ITM_init(void)
{
    ITM_LAR = ITM_LAR_ACCESS; // unlock
    ITM_TCR = 0x1;            // global enable for ITM
    ITM_TPR = 0x1;            // first 8 stim registers have unpriv access
    ITM_TER = 0xf;            // enable 4 stim ports
    DEMCR = TRCENA;           // global enable DWT and ITM
}

#endif

/*
 * 
 */

#ifdef DEBUG_MEM

#define Serial3 Serial

struct trace_list_t {
    const char *file;
    const char *function;
    int linenumber;
} trace_list[32];
uint8_t trace_count = 0;

trace_stack::trace_stack(const char *file, const char *fxn, int line) {
    trace_list[trace_count].file = file;
    
#ifdef TRACE_PRINT
    Serial3.print("TRACE IN ");
    Serial3.print(file);
    Serial3.print(":");
    Serial3.print(line);
    Serial3.print(" ");
    Serial3.println(fxn);
#endif
    
    if(file) {
        trace_list[trace_count].function = fxn;
        trace_list[trace_count].linenumber = line;
        trace_count++;
        trace_num = 0;
    }
    else trace_num = line;
    trace_mark = trace_mark_value;
}

trace_stack::~trace_stack() {
    trace_count--;
#ifdef TRACE_PRINT
    Serial3.print("TRACE OUT depth=");
    Serial3.print(trace_count);
    Serial3.print(" ");
    Serial3.println(trace_list[trace_count].function);
#endif
    if (trace_mark != trace_mark_value) {
        Serial3.print("TRACE MARK FAULT ");
        if (trace_num>0) {
            Serial3.println(trace_num);
        }
        else {
            Serial3.println(trace_list[trace_count].function);
            print_stack();
        }
    }
}

void trace_stack::print_stack() {
    Serial3.println("!!!!STACK!!!!");
    Serial3.flush();
    Serial3.println(trace_count);
    Serial3.flush();
    for (int i=0;i<trace_count;i++) {
        Serial3.print(i);
        Serial3.print("=");
        Serial3.print(trace_list[i].file);
        Serial3.print(":");
        Serial3.print(trace_list[i].function);
        Serial3.print("#");
        Serial3.println(trace_list[i].linenumber);
        Serial3.flush();
    }
    Serial3.println("!!!!END!!!!");
    Serial3.flush();
}

trace_memory *trace_memory_list[16];
uint8_t trace_memory_count = 0;

trace_memory::trace_memory(const char * tag) {
    trace_mark = trace_mark_value;
    trace_tag = tag;
    trace_memory_list[trace_memory_count] = this;
    trace_memory_count++;
}
trace_memory::~trace_memory() {
    verify();
}
void trace_memory::verify() {
    if (trace_mark != trace_mark_value) {
        Serial3.print("TRACE MEMORY CORRUPTION ");
        Serial3.println(trace_tag);
        trace_stack::print_stack();
    }
    //else {
    //  Serial3.print("TRACE MEMORY OK ");
    //  Serial3.println(trace_tag);
    //}
}
void trace_memory::verify_all() {
    for(int i=0; i<trace_memory_count; i++) {
        trace_memory_list[i]->verify();
    }
}

void dump_obj(void *obj, unsigned int sz) {
    uint8_t *b = (uint8_t *)obj;
    for(unsigned int i=0;i<sz;i++) {
        if (i) {
            if (i%4==0) Serial3.print("!");
            else Serial3.print(":");
        }
        Serial3.print(b[i], HEX);
    }
    Serial3.println();
}

#endif

#ifdef DEBUG_TIMING

trace_timing::trace_timing(const char * tag) {
    trace_tag = tag;
    startmicrosec = micros();
}

trace_timing::~trace_timing() {
    long endmicrosec = micros();
    Serial.print(trace_tag);
    Serial.print(" ");
    Serial.print(endmicrosec - startmicrosec);
    Serial.println(" us");
}

#endif


// stack guard
#ifdef USE_STACK_GUARD

extern "C"{
    
    uintptr_t __stack_chk_guard;
    void __stack_chk_guard_setup()
    {
        __stack_chk_guard = 0x000a0dff;
    }
    //__stack_chk_fail
    void __attribute__((noreturn)) __stack_chk_fail()
    //void __stack_chk_fail(void)
    { 
        while(1) {
#ifdef DEBUG_MEM
            trace_stack::print_stack();
#endif
            blink(20);
        }
    }
    
}

#else

extern "C"{ void __stack_chk_guard_setup() { } }

#endif


#ifdef SDCARD_DEBUG

class DebugSD : public Print
{
public:
    File fp;
    bool needopen = false;
    void begin(int x) { begin(); }
    void begin(void) { 
        if (!SD.begin(SDCARD_CS)) {
            return;
        }
        fp = SD.open("debug.txt", FILE_WRITE);
        if (!fp) {
            return;
        }
        fp.println("begin");
    }
    void end(void) { 
        fp.println("end");
        fp.close();
        needopen = true;
    }
    void flush() {
        fp.flush();
    }
    virtual size_t write(uint8_t c) { 
        if (needopen) {
            fp = SD.open("debug.txt", FILE_WRITE);
            needopen = false;
        }
        fp.write(c);
        return 1;
    }
    using Print::write;
};
DebugSD debugsd;
#endif

#ifdef USEDBSTORE

class DebugStore : public Print
{
public:
    static const int maxdata = 10240;
    char data[maxdata];
    char *current;
    char *dataend;
    bool autodump = true;
    void begin(int x) { begin(); }
    void begin(void) { 
        current = data; 
        dataend = data + maxdata; 
    }
    void end(void) { 
        *current = 0; 
#undef Serial
        Serial.print(data); 
#define Serial dbstore
        current = data;
    }
    virtual size_t write(uint8_t c) { 
        if (current >= dataend) {
            if (autodump) end();
            else return 1;
        }
        *current = (char)c;
        current++;
        return 1;
    }
    using Print::write;
};
DebugStore dbstore;
#endif

//
// debugging / crash detection
//

#define SCB_SHCSR_USGFAULTENA (uint32_t)1<<18
#define SCB_SHCSR_BUSFAULTENA (uint32_t)1<<17
#define SCB_SHCSR_MEMFAULTENA (uint32_t)1<<16

#define SCB_SHPR1_USGFAULTPRI *(volatile uint8_t *)0xE000ED20
#define SCB_SHPR1_BUSFAULTPRI *(volatile uint8_t *)0xE000ED19
#define SCB_SHPR1_MEMFAULTPRI *(volatile uint8_t *)0xE000ED18

#ifdef FAULT_REBOOT
void reboot();

void __attribute__((naked)) fault_isr () {
    reboot();
}
#endif

#ifdef FAULT_CATCH

/**
 * 
 * This code is executed in a debug environment. In a release environment all
 * faults cause a reboot.
 * 
 */

// The stack frame saved by th interrupt
typedef struct {
    uint32_t r0;
    uint32_t r1;
    uint32_t r2;
    uint32_t r3;
    uint32_t r12;
    uint32_t lr;
    uint32_t pc;
    uint32_t xpsr;
} interrupt_stack_t;

volatile int fault_id =0;

volatile unsigned long fault_CFSR ;
volatile unsigned long fault_HFSR ;
volatile unsigned long fault_DFSR ;
volatile unsigned long fault_AFSR ;
volatile unsigned long fault_BFAR ;
volatile unsigned long fault_MMAR ;
interrupt_stack_t fault_reg;

/**
 * @brief Copy interrupt registers to memory
 * 
 * @param sp Stack pointer (SP register value)
 */
void save_fault_reg(void *sp) {
    // this creates a local copy of the stack info
    fault_reg =  *((interrupt_stack_t *)sp);
}

/**
 * @brief Default fault handler
 * 
 */
extern "C" void __attribute((naked, noinline, used)) debug_fault_handler() {
#if 0
    register void *rsp __asm("sp");
    // r0-r3,r2,lr,pc of caller on stack, copy to variable
    save_fault_reg(rsp);
#endif
#if 0
    // Control Fault Status Register
    fault_CFSR = (*((volatile unsigned long *)(0xE000ED28)));
    // Hard Fault Status Register
    fault_HFSR = (*((volatile unsigned long *)(0xE000ED2C)));
    // Debug Fault Status Register
    fault_DFSR = (*((volatile unsigned long *)(0xE000ED30)));
    // Auxiliary Fault Status Register
    fault_AFSR = (*((volatile unsigned long *)(0xE000ED3C))) ;
    // Read the Fault Address Registers. These may not contain valid values.
    // Check BFARVALID/MMARVALID to see if they are valid values
    // MemManage Fault Address Register
    fault_MMAR = (*((volatile unsigned long *)(0xE000ED34)));
    // Bus Fault Address Register
    fault_BFAR = (*((volatile unsigned long *)(0xE000ED38)));
#endif
    
#ifdef FAULT_CATCH_REBOOT
    reboot();
#endif
    
    __asm volatile("BKPT");
    // __asm volatile("LDR r0, [sp, #24]"); // get PC of fault
    __asm volatile("BX LR");
}

void __attribute((naked, noinline, used)) hard_fault_isr() {
    // __asm volatile("BKPT");
    __asm volatile("b debug_fault_handler");
}

void __attribute((naked, noinline, used)) memmanage_fault_isr() {
    // __asm volatile("BKPT");
    __asm volatile("b debug_fault_handler");
}

void __attribute((naked, noinline, used)) bus_fault_isr() {
    // __asm volatile("BKPT");
    __asm volatile("b debug_fault_handler");
}

void __attribute((naked, noinline, used)) usage_fault_isr() {
    // __asm volatile("BKPT");
    __asm volatile("b debug_fault_handler");
}

void __attribute((naked, noinline, used)) fault_isr() {
    // __asm volatile("BKPT");
    __asm volatile("b debug_fault_handler");
}

/**
 * @brief Initialize fault handing and stack guard handling
 * 
 */
void debug_crash_init() {
    SCB_SHCSR |= SCB_SHCSR_BUSFAULTENA | SCB_SHCSR_USGFAULTENA | SCB_SHCSR_MEMFAULTENA;
    __stack_chk_guard_setup();
}

#else

/**
 * 
 * Release environment. No fault handing. Reboot on fault.
 * 
 */

void debug_crash_init() {}

#endif

