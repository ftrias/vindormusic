#include "ProgramFlash.h"

#define FCMD_READ_1S_BLOCK            0x00
#define FCMD_READ_1S_SECTION          0x01
#define FCMD_PROGRAM_CHECK            0x02
#define FCMD_READ_RESOURCE            0x03
#define FCMD_PROGRAM_LONG_WORD        0x06
#define FCMD_ERASE_FLASH_BLOCK        0x08
#define FCMD_ERASE_FLASH_SECTOR       0x09
#define FCMD_PROGRAM_SECTOR           0x0b
#define FCMD_READ_1S_ALL_BLOCKS       0x40
#define FCMD_READ_ONCE                0x41
#define FCMD_PROGRAM_ONCE             0x43
#define FCMD_ERASE_ALL_BLOCKS         0x44
#define FCMD_SET_FLEXRAM        0x81

#define FCMD_SET_FLEXRAM_EEPROM     0x00
#define FCMD_SET_FLEXRAM_RAM      0xff

#define FTFL_STAT_MGSTAT0         0x01  // Error detected during sequence
#define FTFL_STAT_FPVIOL          0x10  // Flash protection violation flag
#define FTFL_STAT_ACCERR          0x20  // Flash access error flag
#define FTFL_STAT_CCIF            0x80  // Command complete interrupt flag

#define FTFL_READ_MARGIN_NORMAL     0x00

/*
  ==================================================================
  Internal Functions
  ==================================================================
*/

void *flexram_data = (void*)0x14000000; // 2KB
void *flexnvm_data = (void*)0x10000000; // 32KB


#define FLASH_ALIGN(address,align) address=((unsigned long*)((unsigned long)address & (~(align-1))))

FASTRUN void ProgramFlash::flashExec(volatile uint8_t *fstat)                // Execute Flash Command in RAM
{                                         
  *fstat = FTFL_STAT_CCIF;
  while (!(*fstat & FTFL_STAT_CCIF)) {;}
}

void ProgramFlash::flashInitCommand(unsigned char command, const void *address)
{
  while (!(FTFL_FSTAT & FTFL_STAT_CCIF)) {;}
  FTFL_FSTAT  = 0x30;
  FTFL_FCCOB0 = command;
  FTFL_FCCOB1 = ((unsigned long)address) >> 16;
  FTFL_FCCOB2 = ((unsigned long)address) >> 8;
  FTFL_FCCOB3 = ((unsigned long)address);
}

/*
  ==================================================================
  User Functions
  ==================================================================
*/

// Check if sector is erased
int ProgramFlash::flashCheckSectorErased(const void *address)                  
{
  FLASH_ALIGN(address, FLASH_SECTOR_SIZE);
  flashInitCommand(FCMD_READ_1S_SECTION, address);
  const int num = FLASH_SECTOR_SIZE >> 2;
  FTFL_FCCOB4 = (unsigned char)(num >> 8);                    // Number of longwords
  FTFL_FCCOB5 = (unsigned char)(num);
  FTFL_FCCOB6 = FTFL_READ_MARGIN_NORMAL;
  __disable_irq();
  flashExec(&FTFL_FSTAT);
  __enable_irq();
  return (FTFL_FSTAT & (FTFL_STAT_ACCERR | FTFL_STAT_FPVIOL | FTFL_STAT_MGSTAT0));
}

int ProgramFlash::flashEraseSector(const void *address, bool allowFirstSector)         // Erase Flash Sector
{
  FLASH_ALIGN(address, FLASH_SECTOR_SIZE);
  if((unsigned long)address<FLASH_SECTOR_SIZE) if(!allowFirstSector) return -1; // Won't erase first sector unless flagged end user is sure.
  if (flashCheckSectorErased(address)  && (address > 0) )
    {
      flashInitCommand(FCMD_ERASE_FLASH_SECTOR, address);
      __disable_irq();
      flashExec(&FTFL_FSTAT);
      __enable_irq();
      return (FTFL_FSTAT & (FTFL_STAT_ACCERR | FTFL_STAT_FPVIOL | FTFL_STAT_MGSTAT0));      
    }
  return 0;
}

int ProgramFlash::flashProgramWord(const unsigned long *address, const unsigned long *data, bool aFS, bool oSFC)  // Program Flash, one long word (32 Bit)
{
  FLASH_ALIGN(address, 0x04);
  if((unsigned long)address<FLASH_SECTOR_SIZE) if(!aFS) return -1;    // no writing in first sector unless end user is sure.
  if (((((unsigned long)address)>=0x400) && ((unsigned long)address)<=0x40C))
    if(!oSFC) return -2;                            // Make sure not to write 0x400 - 0x40F
  flashInitCommand(FCMD_PROGRAM_LONG_WORD, address);
  FTFL_FCCOB4 = (unsigned char)(*data >> 24);                         // Enter the long word to be programmed
  FTFL_FCCOB5 = (unsigned char)(*data >> 16);
  FTFL_FCCOB6 = (unsigned char)(*data >> 8);
  FTFL_FCCOB7 = (unsigned char)*data;
  __disable_irq();
  flashExec(&FTFL_FSTAT);
  __enable_irq();
  return (FTFL_FSTAT & (FTFL_STAT_ACCERR | FTFL_STAT_FPVIOL | FTFL_STAT_MGSTAT0));
}

FASTRUN int ProgramFlash::flashProgramSection(const void *address, int numSize, bool aFS, bool oSFC)  // Program Flash, one long word (32 Bit)
{
  FLASH_ALIGN(address, 0x04);
  if((unsigned long)address<FLASH_SECTOR_SIZE) if(!aFS) return -1;    // no writing in first sector unless end user is sure.
  if (((((unsigned long)address)>=0x400) && ((unsigned long)address)<=0x40C))
    if(!oSFC) return -2;                            // Make sure not to write 0x400 - 0x40F
  flashInitCommand(FCMD_PROGRAM_SECTOR, address);
  int sz = FLASH_WRITE_SIZE / numSize;
  FTFL_FCCOB4 = (unsigned char)(sz >> 8);                         // Enter the long word to be programmed
  FTFL_FCCOB5 = (unsigned char)(sz & 0xFF);
  __disable_irq();
  flashExec(&FTFL_FSTAT);
  __enable_irq();
  return (FTFL_FSTAT & (FTFL_STAT_ACCERR | FTFL_STAT_FPVIOL | FTFL_STAT_MGSTAT0));
}

// Disable EEPROM function and enable use of FLEXRAM as RAM
void ProgramFlash::flashSetFlexRAM(bool flexram)
{
  while (!(FTFL_FSTAT & FTFL_FSTAT_CCIF)) {;}
  FTFL_FSTAT  = 0x30;
  FTFL_FCCOB0 = FCMD_SET_FLEXRAM;
  if (flexram) {
    FTFL_FCCOB1 = FCMD_SET_FLEXRAM_RAM;
  }
  else {
    FTFL_FCCOB1 = FCMD_SET_FLEXRAM_EEPROM;
  }
  flashExec(&FTFL_FSTAT);
}

int ProgramFlash::flashGetError() {
  return (FTFL_FSTAT & (FTFL_STAT_ACCERR | FTFL_STAT_FPVIOL | FTFL_STAT_MGSTAT0));
}


/* FTFL_FSEC

B7-B6 KEYEN Backdoor Key Security Enable
  These bits enable and disable backdoor key access to the flash memory module.
  00 Backdoor key access disabled
  01 Backdoor key access disabled (preferred KEYEN state to disable backdoor key access)
  10 Backdoor key access enabled
  11 Backdoor key access disabled (Default)

B5-B6 MEEN Mass Erase Enable Bits
  Enables and disables mass erase capability of the flash memory module. The state of the MEEN bits is
  only relevant when the SEC bits are set to secure outside of NVM Normal Mode. When the SEC field is
  set to unsecure, the MEEN setting does not matter.
  00 Mass erase is enabled
  01 Mass erase is enabled
  10 Mass erase is disabled
  11 Mass erase is enabled

B3–B2 FSLACC Freescale Failure Analysis Access Code
  These bits enable or disable access to the flash memory contents during returned part failure analysis at
  Freescale. When SEC is secure and FSLACC is denied, access to the program flash contents is denied
  and any failure analysis performed by Freescale factory test must begin with a full erase to unsecure the
  part. When access is granted (SEC is unsecure, or SEC is secure and FSLACC is granted), Freescale factory
  testing has visibility of the current flash contents. The state of the FSLACC bits is only relevant when the
  SEC bits are set to secure. When the SEC field is set to unsecure, the FSLACC setting does not matter.
  00 Freescale factory access granted
  01 Freescale factory access denied
  10 Freescale factory access denied
  11 Freescale factory access granted

B1–B0 SEC Flash Security
  These bits define the security state of the MCU. In the secure state, the MCU limits access to flash
  memory module resources. The limitations are defined per device and are detailed in the Chip
  Configuration details. If the flash memory module is unsecured using backdoor key access, the SEC bits
  are forced to 10b.
  00 MCU security status is secure
  01 MCU security status is secure
  10 MCU security status is unsecure (The standard shipping condition of the flash memory module is unsecure.)
  11 MCU security status is secure

    FTFL_FSEC=0x64; // 0b1100100 = 100 = the value robsoles will recommend locking up with.
  
*/



int ProgramFlash::flashSecurityLockBits(uint8_t newValueForFSEC)
{
  while (!(FTFL_FSTAT & FTFL_STAT_CCIF)) {;}
  FTFL_FSTAT  = 0x30;
  FTFL_FCCOB0 = FCMD_PROGRAM_LONG_WORD;
  FTFL_FCCOB1 = 0;
  FTFL_FCCOB2 = 4;
  FTFL_FCCOB3 = 0xC;
  
  FTFL_FCCOB4 = 0xFF; // It is not possible to turn bits on without erasing a larger block, I am
  FTFL_FCCOB5 = 0xFF; // using all on value 
  FTFL_FCCOB6 = 0xFF; // 
  FTFL_FCCOB7 = newValueForFSEC;
  __disable_irq();
  flashExec(&FTFL_FSTAT);
  __enable_irq();
  return (FTFL_FSTAT & (FTFL_STAT_ACCERR | FTFL_STAT_FPVIOL | FTFL_STAT_MGSTAT0));
}

void ProgramFlash::flashQuickUnlockBits()
{
  while (!(FTFL_FSTAT & FTFL_STAT_CCIF)) {;}
  FTFL_FSTAT  = 0x30;
  FTFL_FCCOB0 = FCMD_ERASE_FLASH_SECTOR;
  FTFL_FCCOB1 = 0;
  FTFL_FCCOB2 = 0;
  FTFL_FCCOB3 = 0;

  __disable_irq();
  FTFL_FSTAT = FTFL_STAT_CCIF;
  while (!(FTFL_FSTAT & FTFL_STAT_CCIF)) {;}
  FTFL_FSTAT  = 0x30;
  FTFL_FCCOB0 = FCMD_PROGRAM_LONG_WORD;
  FTFL_FCCOB1 = 0;
  FTFL_FCCOB2 = 4;
  FTFL_FCCOB3 = 0xC;
  
  FTFL_FCCOB4 = 0xFF;
  FTFL_FCCOB5 = 0xFF;
  FTFL_FCCOB6 = 0xFF;
  FTFL_FCCOB7 = 0xFE;
  FTFL_FSTAT = FTFL_STAT_CCIF;
  while (!(FTFL_FSTAT & FTFL_STAT_CCIF)) {;}
}

void *ProgramFlash::getWrite(const void *dest) {
    flashSetFlexRAM(true);
    if (dest) {
        memcpy(flexram_data, dest, FLASH_WRITE_SIZE * 2);
    }
    return flexram_data;
}

int ProgramFlash::commitWrite(const void *dest) {
    int result;
    const uint8_t *flash = (const uint8_t *)dest;    

    if (flashCheckSectorErased(flash)) {
        // not erased, so erase now
        if (flashEraseSector(flash)) {
            flashSetFlexRAM(false);     
            return -1;
        }
    }

    // copy 2nd half of FlexRAM because it gets corrupted
    // FlexRAM is 2 KB, but only 1 KB is written at a time
    uint8_t buffer[FLASH_WRITE_SIZE];
    uint8_t *src = (uint8_t *)flexram_data;
    src += FLASH_WRITE_SIZE;
    memcpy(buffer, src, FLASH_WRITE_SIZE);

    int wordSize = 8;
    if (flash > flexnvm_data) wordSize = 4;

    result = flashProgramSection(flash, wordSize, false, false);
    if (result) {   
      flashSetFlexRAM(false);     
      return -2;
    }

    flash += FLASH_WRITE_SIZE;
    memcpy(flexram_data, src, FLASH_WRITE_SIZE);
    
    result = flashProgramSection(flash, wordSize, false, false);
    if (result) {   
      flashSetFlexRAM(false);     
      return -3;
    }

    flashSetFlexRAM(false);    
    return 0;
}

int ProgramFlash::flashWriteSectors(void *dest, const uint8_t *src, int num)
{    
    int blockSize = FLASH_WRITE_SIZE;
    int result;

    flashSetFlexRAM(true);     

    const uint8_t *flash = (const uint8_t *)dest;

    for(int i=0; i<num; i++) {
      if (flashCheckSectorErased(flash)) {
        // not erased, so erase now
        if (flashEraseSector(flash)) {
          flashSetFlexRAM(false);     
          return -1;
        }
      }
  
      int wordSize = 8;
      if (flash > flexnvm_data) wordSize = 4;

      memcpy(flexram_data, src, blockSize);
      
      result = flashProgramSection(flash, wordSize, false, false);
      if (result) {   
        flashSetFlexRAM(false);     
        return -2;
      }

      flash += blockSize;
      src += blockSize;

      memcpy(flexram_data, src, blockSize);

      result = flashProgramSection(flash, wordSize, false, false);
      if (result) { 
        flashSetFlexRAM(false);     
        return -3;
      }

      flash += blockSize;
      src += blockSize;
    }

    flashSetFlexRAM(false);     
    
    return 0;
}