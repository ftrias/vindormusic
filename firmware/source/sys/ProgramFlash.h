#ifndef ProgramFlash_h_
#define ProgramFlash_h_

#include "kinetis.h"
#include "WProgram.h"

#if defined(__MK20DX128__)
#define FLASH_SIZE      0x0001FFFF
#define FLASH_SECTOR_SIZE 0x800
#define FLASH_WRITE_SIZE 0x400
#elif defined(__MK20DX256__)
#define FLASH_SIZE      0x0003FFFF
#define FLASH_SECTOR_SIZE 0x800
#define FLASH_WRITE_SIZE 0x400
#else
#error NOT SUPPORTED
#endif

// #define FLASH_SECTORS(name, number) __attribute__ ((section(".apptext"), used, aligned(FLASH_SECTOR_SIZE))) const uint8_t name[FLASH_SECTOR_SIZE * number] = {0};

#define FLASH_SECTORS(name, dtype, number) __attribute__ ((section(".flashdata"), used, aligned(FLASH_SECTOR_SIZE))) \
    const dtype name[FLASH_SECTOR_SIZE * number / sizeof(dtype)] = {0};

class ProgramFlash {
private:
    static void flashExec(volatile uint8_t *fstat);
    static void flashInitCommand(unsigned char command, const void *address);
    static int flashGetError();

    static int flashProgramWord(const unsigned long *address, const unsigned long *data, bool allowFirstSector=false, bool overrideSafetyForConfig=false);
    static int flashProgramSection(const void *address, int numSize, bool allowFirstSector, bool overrideSafetyForConfig);

    static void flashSetFlexRAM(bool flexram);

    static int flashSecurityLockBits(uint8_t newValueForFSEC=0x64); // 0x64 appears to be one of the safer values to use here.
    static void flashQuickUnlockBits();

    static int flashCheckSectorErased(const void *address);
    static int flashEraseSector(const void *address, bool allowFirstSector=false);

public:
    static void *getWrite(const void *dest);
    static int commitWrite(const void *dest);
    static int flashWriteSectors(void *dest, const uint8_t *src, int num);
};
#endif
