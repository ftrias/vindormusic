// #include "firmware.h"
#include "ReadFingeringTree.h"

#include <stdio.h>

const int16_t fingerlist_test[] = {
27, 2, 0, 5, -17, 6, 8, -24, 0, 26, 0, 19, 16, 15, -22, 0, 18, 0, 0, 23, 22, -20, -19, 25, -19, -19, 0, 28, 0, 36, 32, 0, 0, 35, -15, 0, 
42, 41, 40, -14, -15, -13, 46, 45, -12, -13, 0, 70, 64, 57, 54, 53, -10, -9, 56, -9, -9, 61, 60, -8, -7, 63, -7, -7, 65, 67, -3, 69, -3, 
-3, 0, 73, -11, 75, -5, 77, -5, -5
, END_FINGERING
};
int fingerlist_test_bits = 14;
int fingerlist_test_mask = 0x3FFF;
int fingerlist_test_useoctave = 0;

extern const char *fingerRecorder[];

ReadFingeringTree fingering;

int getBinaryDigit(int i, int b)
{
    if (i & 1 << b)
        return 1;
    return 0;
}

char *getbin(int i, int bits)
{
    static char x[32];
    for (int b = 0; b < bits; b++)
    {
        if (i & 1 << (bits - b - 1))
            x[b] = '1';
        else
            x[b] = '0';
    }
    x[bits] = 0;
    return x;
}

void findinvalid()
{
    for (int i = 0; i <= 0x3FFF; i++)
    {
        int note = fingering.getNoteFromTree(i);
        if (note == -1)
        {
            printf("invalid %d %s\n", i, getbin(i, 16));
        }
    }
}

#if 0
void printnotetable()
{
    printf("const int notetablelen = 2048;\n");
    printf("const int notetable[] = {\n");
    for (int i = 0; i <= 0x7FF; i++)
    {
        int note = fingering.getNoteFromTree(i);
        if (i == 0)
            printf("  ");
        else
        {
            printf(",");
            if (i % 20 == 0)
            {
                printf("\n  ");
            }
        }
        printf("%d", note);
    }
    printf("\n};\n");
}

void dumpnotetable()
{
    printf("const int notetablelen = 2048;\n");
    printf("const int notetable[] = {\n");
    for (int i = 0; i <= 0x7FF; i++)
    {
        int note = notetable[i];
        if (i == 0)
            printf("  ");
        else
        {
            printf(",");
            if (i % 20 == 0)
            {
                printf("\n  ");
            }
        }
        printf("%d", note);
    }
    printf("\n};\n");
#if 0
  for (int i=0;i<=0x7FF; i++) {
    int note = notetable[i];
    if (note==255) {
      printf("invalid %d %s\n", i, getbin(i, 16));
    }
  }
#endif
}
#endif

char *getNoteName(int note)
{
    static char b[4];
    const char *onames[] = {"0", "1", "2", "3", "4", "5", "6", "7", "8"};
    const char *names[] = {"A", "A#", "B", "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#"};
    int note12 = (note + 14) % 12;
    int oct = (note + 14) / 12;
    b[1] = b[2] = b[3] = 0;
    b[0] = names[note12][0];
    b[1] = names[note12][1];
    if (b[1])
        b[2] = '2' + oct;
    else
        b[1] = '2' + oct;
    return b;
}

#if 0

void csvnotetable()
{
    for (int b = 10; b >= 0; b--)
    {
        printf("b%d,", 10 - b + 1);
    }
    printf("num,mnum,midi,note\n");
    for (int i = 0; i <= 0x7FF; i++)
    {
        for (int b = 10; b >= 0; b--)
        {
            printf("%d,", getBinaryDigit(i, b));
        }
        int note = notetable[i];
        printf("%d,%d,%d,%s\n", i, note, note + 10 + 69 - 12 - 12, getNoteName(note));
    }
}

void csv2notetable()
{
    printf("note,num,pathString\n");
    for (int i = 0; i <= 0x7FF; i++)
    {
        int note = notetable[i];
        printf("%s,%d,b", getNoteName(note), note);
        for (int b = 10; b >= 0; b--)
        {
            printf("/%d", getBinaryDigit(i, b));
        }
        printf("\n");
    }
}

void printnotetabledetail()
{
    for (int i = 0; i <= 0x3FFF; i++)
    {
        int note = fingering.getNoteFromTree(i);
        printf("%04d %s %d %s\n", i, getbin(i, 16), note, noteNames[note]);
    }
}

#endif

/*
Benchmark getNote vs getNoteList
real    0m0.878s
user    0m0.874s
sys     0m0.002s

real    0m2.666s
user    0m2.661s
sys     0m0.002s
*/

// #include "../fast_float.h"

int main()
{
    fingering.setKeysCustom(fingerlist_test, fingerlist_test_bits, fingerlist_test_mask, fingerlist_test_useoctave);
    // dumpnotetable();
    // csv2notetable();
    // printnotetabledetail();
    findinvalid();

    // globalFingerTree.load(fingerlist_test);
    // for(int b=0; b<=0b11111111111; b++) {
    //     int n0 = globalFingerTree.getNote(b);
    //     int n = globalFingerTree.getNoteList(b);
    //     if (n==-1) {
    //         printf("invalid b=%s\n", getbin(b,11));            
    //     }
    //     if (n0!=n) {
    //         printf("b=%s n0=%d n=%d %s\n", getbin(b,11), n0, n, noteNames[n]);
    //     }
    // }
    
    /*
    for(int i=0;i<100000;i++) {
        for(int b=0; b<0b11111111111; b++) {
            // int n = globalFingerTree.getNote(b);
            int n = globalFingerTree.getNoteList(b);
        }
    }
    */

    
    #if 0
    float y1(1);
    float y2(-1);
    float x0(10);
    for(int c0=0; c0<1000000000; c0++) {
        x0=x0+y1;
        x0=x0+y2;
    }
    printf("%f\n", x0);
    /*
    ffloat x(10);
    float y =-7.0f;
    x.dumpFloat(y);
    printf("m=%i e=%i %f\n", x.data.part.m, x.data.part.e, x.getFloat());
    */
   #endif

    return 0;
}
