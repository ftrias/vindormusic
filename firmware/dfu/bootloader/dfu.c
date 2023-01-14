/*
 * Fadecandy DFU Bootloader
 * 
 * Copyright (c) 2013 Micah Elizabeth Scott
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <stdbool.h>
#include "mk20dx128.h"
#include "usb_dev.h"
#include "dfu.h"
#include "serial.h"

// Internal flash-programming state machine
static unsigned fl_current_addr = 0;
static enum {
    flsIDLE = 0,
    flsERASING,
    flsPROGRAMMING
} fl_state;

static dfu_state_t dfu_state = dfuIDLE;
static dfu_status_t dfu_status = OK;
static unsigned dfu_poll_timeout = 1;

// Programming buffer in MK20DX128 FlexRAM, where the flash controller can quickly access it.
static __attribute__ ((section(".flexram"))) uint8_t dfu_buffer[DFU_TRANSFER_SIZE];

extern uint8_t appflash[];

static void *memcpy(void *dst, const void *src, size_t cnt) {
    uint8_t *dst8 = dst;
    const uint8_t *src8 = src;
    while (cnt > 0) {
        cnt--;
        *(dst8++) = *(src8++);
    }
    return dst;
}

static bool ftfl_busy()
{
    // Is the flash memory controller busy?
    return 0 == (FTFL_FSTAT_CCIF & FTFL_FSTAT);
}

static void ftfl_busy_wait()
{
    // Wait for the flash memory controller to finish any pending operation.
    while (ftfl_busy());
}

static void ftfl_launch_command()
{
    // Begin a flash memory controller command
    FTFL_FSTAT = FTFL_FSTAT_ACCERR | FTFL_FSTAT_FPVIOL | FTFL_FSTAT_RDCOLERR;
    FTFL_FSTAT = FTFL_FSTAT_CCIF;
}

static void ftfl_set_flexram_function(uint8_t control_code)
{
    // Issue a Set FlexRAM Function command. Busy-waits until the command is done.
    ftfl_busy_wait();
    FTFL_FCCOB0 = 0x81;
    FTFL_FCCOB1 = control_code;
    ftfl_launch_command();
    ftfl_busy_wait();
}

static void ftfl_begin_erase_sector(uint32_t address)
{
#if 0
    serial_debug("ERASE");
    serial_phex32(address);
    serial_write("\r\n",2);
#endif

    FTFL_FCCOB0 = 0x09;
    FTFL_FCCOB1 = address >> 16;
    FTFL_FCCOB2 = address >> 8;
    FTFL_FCCOB3 = address;
    ftfl_launch_command();
}

#if 0
static void dump(uint32_t address)
{
    uint8_t *mem = (uint8_t *)address;
    serial_phex32((uint32_t)mem);
    serial_write("\r\n",2);
    int i;
    for(i=0;i<16;i++) {
      serial_phex(mem[i]);
      serial_putchar(' ');
    }
    serial_write("\r\n",2);
}
#endif

static void ftfl_begin_program_section(uint32_t address, uint32_t numLWords)
{
#if 0
    serial_debug("WRITE");
    serial_phex32(address);
    serial_putchar(':');
    serial_phex32(numLWords);
    serial_write("\r\n",2);
    //dump(dfu_buffer);
#endif
    FTFL_FCCOB0 = 0x0B;
    FTFL_FCCOB1 = address >> 16;
    FTFL_FCCOB2 = address >> 8;
    FTFL_FCCOB3 = address;
    FTFL_FCCOB4 = numLWords >> 8;
    FTFL_FCCOB5 = numLWords;
    ftfl_launch_command();
}

static void ftfl_begin_verify_section(uint32_t address, uint32_t numLWords)
{
    // TODO
    //uint8_t *mem = (uint8_t *)address;
    //serial_debug("VERIFY");
    //dump(appflash);
}

static uint32_t address_for_block(unsigned blockNum)
{
    return ((uint32_t)appflash) + (blockNum << 10);
}

/******* crypt **********/

#ifdef USE_CRYPT

#include "ecrypt-sync.h"

#define CHUNKSIZE 64

const u8 key[ECRYPT_MAXKEYSIZE] = {10,4,2,7};
const u8 IV[ECRYPT_MAXIVSIZE] = {0};
ECRYPT_ctx ctx;

void crypt_init() {
    ECRYPT_init();
    ECRYPT_keysetup(&ctx, key, ECRYPT_MAXKEYSIZE, ECRYPT_MAXIVSIZE);
    ECRYPT_ivsetup(&ctx, IV);
}

void crypt_decrypt(const uint8_t *data, uint8_t *out, int len) {
    ECRYPT_decrypt_bytes(&ctx, (u8*)data, (u8*)out, len);
}

#endif

/********************/

// Cache control
#define FMC_PFAPR                       (*(volatile uint32_t *)0x4001F000)
#define FMC_PFB0CR                      (*(volatile uint32_t *)0x4001F004)
#define FMC_PFB1CR                      (*(volatile uint32_t *)0x4001F008)

void dfu_init()
{
    // Use FlexRAM (dfu_buffer) as normal RAM.
    ftfl_set_flexram_function(0xFF);
#ifdef USE_CRYPT
    crypt_init();
#endif
}

void dfu_end()
{
}

uint8_t dfu_getstate()
{
    return dfu_state;
}

bool dfu_download(unsigned blockNum, unsigned blockLength,
    unsigned packetOffset, unsigned packetLength, const uint8_t *data)
{
    if (packetOffset + packetLength > DFU_TRANSFER_SIZE ||
        packetOffset + packetLength > blockLength) {
        // serial_debug("D1");
        // Overflow!
        dfu_state = dfuERROR;
        dfu_status = errADDRESS;
        return false;
    }

#ifdef USE_CRYPT
    static uint8_t out[64];
    crypt_decrypt(data, out, packetLength);
    // Store more data...
    memcpy(dfu_buffer + packetOffset, out, packetLength);
#else
    memcpy(dfu_buffer + packetOffset, data, packetLength);
#endif

    if (packetOffset + packetLength != blockLength) {
        // Still waiting for more data.
        return true;
    }

    if (dfu_state != dfuIDLE && dfu_state != dfuDNLOAD_IDLE) {
        // Wrong state! Oops.
        dfu_state = dfuERROR;
        dfu_status = errSTALLEDPKT;
        // serial_debug("D3");
        return false;
    }

    if (ftfl_busy() || fl_state != flsIDLE) {
        // Flash controller shouldn't be busy now!
        dfu_state = dfuERROR;
        dfu_status = errUNKNOWN;
        // serial_debug("D4");
        return false;       
    }

    if (!blockLength) {
        // End of download
        dfu_state = dfuMANIFEST_SYNC;
        dfu_status = OK;
        // serial_debug("D5");
        //dump(appflash);
        return true;
    }

    fl_current_addr = address_for_block(blockNum);
    
    if (fl_current_addr % DFU_SECTOR_SIZE == 0) {
        // Erase at start of 2K block
	    // Start programming a block by erasing the corresponding flash sector
    	fl_state = flsERASING;
    	ftfl_begin_erase_sector(fl_current_addr);
    }
    else {
        // Not at start of block, so don't erase, just program
        fl_state = flsPROGRAMMING;
        ftfl_begin_program_section(fl_current_addr, DFU_TRANSFER_SIZE / DFU_TRANSFER_WORDSIZE);
	}
	
    dfu_state = dfuDNLOAD_SYNC;
    dfu_status = OK;
    return true;
}

static bool fl_handle_status(uint8_t fstat, unsigned specificError)
{
    /*
     * Handle common errors from an FSTAT register value.
     * The indicated "specificError" is used for reporting a command-specific
     * error from MGSTAT0.
     *
     * Returns true if handled, false if not.
     */


    if (0 == (fstat & FTFL_FSTAT_CCIF)) {
        // Still working...
        return true;
    }

    if (fstat & FTFL_FSTAT_RDCOLERR) {
        // Bus collision. We did something wrong internally.
        dfu_state = dfuERROR;
        dfu_status = errUNKNOWN;
        fl_state = flsIDLE;
        // serial_debug("DxRD");
        return true;
    }

    if (fstat & (FTFL_FSTAT_FPVIOL | FTFL_FSTAT_ACCERR)) {
        // Address or protection error
        dfu_state = dfuERROR;
        dfu_status = errADDRESS;
        fl_state = flsIDLE;
        // serial_debug("DxER");
        return true;
    }

    if (fstat & FTFL_FSTAT_MGSTAT0) {
        // Command-specifid error
        dfu_state = dfuERROR;
        dfu_status = specificError;
        fl_state = flsIDLE;
        // serial_debug("DxMG");
        return true;
    }

    //serial_write("RES ", 4);
    //serial_phex(fstat);
    //serial_write("\r\n",2);
    
    return false;
}

static void fl_state_poll()
{
    // Try to advance the state of our own flash programming state machine.
    uint8_t fstat = FTFL_FSTAT;
    switch (fl_state) {

        case flsIDLE:
            break;

        case flsERASING:
            // serial_debug("De");
            if (!fl_handle_status(fstat, errERASE)) {
                // serial_debug("Do");
                // Done! Move on to programming the sector.
                fl_state = flsPROGRAMMING;
                ftfl_begin_program_section(fl_current_addr, DFU_TRANSFER_SIZE / DFU_TRANSFER_WORDSIZE);
            }
            break;

        case flsPROGRAMMING:
            // serial_debug("Dp");
            if (!fl_handle_status(fstat, errVERIFY)) {
                // Done!
                ftfl_begin_verify_section(fl_current_addr, DFU_TRANSFER_SIZE / DFU_TRANSFER_WORDSIZE);
                fl_state = flsIDLE;
            }
            break;
    }
}

bool dfu_getstatus(uint8_t *status)
{
    switch (dfu_state) {

        case dfuDNLOAD_SYNC:
        case dfuDNBUSY:
            // Programming operation in progress. Advance our private flash state machine.
            fl_state_poll();

            if (dfu_state == dfuERROR) {
                // An error occurred inside fl_state_poll();
            } else if (fl_state == flsIDLE) {
                dfu_state = dfuDNLOAD_IDLE;
            } else {
                dfu_state = dfuDNBUSY;
            }
            break;

        case dfuMANIFEST_SYNC:
            // Ready to reboot. The main thread will take care of this. Also let the DFU tool
            // know to leave us alone until this happens.
            dfu_state = dfuMANIFEST;
            dfu_poll_timeout = 1000;
            break;

        default:
            break;
    }

    status[0] = dfu_status;
    status[1] = dfu_poll_timeout;
    status[2] = dfu_poll_timeout >> 8;
    status[3] = dfu_poll_timeout >> 16;
    status[4] = dfu_state;
    status[5] = 0;  // iString

    return true;
}

bool dfu_clrstatus()
{
    switch (dfu_state) {

        case dfuERROR:
            // Clear an error
            dfu_state = dfuIDLE;
            dfu_status = OK;
            return true;

        default:
            // Unexpected request
            dfu_state = dfuERROR;
            dfu_status = errSTALLEDPKT;
            return false;
    }
}

bool dfu_abort()
{
    dfu_state = dfuIDLE;
    dfu_status = OK;
    return true;
}
