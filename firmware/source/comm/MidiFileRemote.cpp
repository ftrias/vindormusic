#include "MidiFileRemote.h"

#include <string.h>

void initMessage(uint8_t *msg) {
    msg[0] = 0xF0;
    msg[1] = MIDI_MFG1;
    msg[2] = MIDI_MFG2;
    msg[3] = MIDI_MFG3;
    msg[4] = MIDI_MSG_FILE;
}

#ifdef ARDUINO

#include "Display.h"

extern unsigned char staticBuffer[];
extern int MidiSerial;

int MidiFileDevice::processMessage(const unsigned char *message, int bytes) {
    // showDebug("MidiFile");
    if (message[0] != 0xF0) return 0;

    if (message[1] == MIDI_MFG1 && message[2] == MIDI_MFG2 && message[3] == MIDI_MFG3) {
    }
    else return 0;

    if (message[4] != MIDI_MSG_FILE) return 0;

    message += 5;

    const char *fn;

    switch(message[0]) {
        case MIDI_MSG_OPEN_READ:
            fn = (const char *) message+2;
            // Display::displayDebug("open_r:", fn);
            file = SD.open(fn, FILE_READ);
            // if (! file) {
            //     Display::displayDebug("fail_r:", fn);
            // }
            break;
        case MIDI_MSG_OPEN_WRITE:
            fn = (const char *) message+2;
            // Display::displayDebug("open_w:", fn);
            if (SD.exists(fn)) {
                SD.remove(fn);
            }
            // file = SD.open(fn, FILE_WRITE);
            file.open("bench.dat", O_CREAT | O_TRUNC | O_RDWR);
            // if (! file) {
            //     Display::displayDebug("fail_w:", fn);
            // }
            break;
        case MIDI_MSG_CLOSE:
            // Display::displayDebug("close");
            file.close();
            break;
        case MIDI_MSG_READ:
            // Display::displayDebug("read");
            processMessageRead(message, bytes);
            break;
        case MIDI_MSG_WRITE:
            // Display::displayDebug("write");
            processMessageWrite(message, bytes);
            break;
        case MIDI_MSG_REMOVE:
            fn = (const char *) message+2;
            // Display::displayDebug("rm:", fn);
            SD.remove(fn);
            break;
    }
    return 0;
}

int MidiFileDevice::processMessageRead(const unsigned char *message, int bytes) {
    uint8_t buffer[64];
    int len = message[1];
    file.read(buffer, len);
    uint8_t msg[128];
    initMessage(msg);
    msg[5] = MIDI_MSG_DATA;
    msg[6] = len;
    for(int i=0, j=0; i<len; i++, j+=2) {
        msg[7+j] = buffer[i] >> 4;
        msg[8+j] = buffer[i] & 0x0F;
    }
    usbMIDI.sendSysEx(len * 2 + 7, msg, false);
    return 0;
}

int MidiFileDevice::processMessageWrite(const unsigned char *message, int bytes) {
    uint8_t buffer[64];
    int len = message[1];
    for(int i=0, j=0; i<len; i++, j+=2) {
        buffer[i] = (message[2+j] << 4) + message[3+j]; 
    }
    file.write(buffer, len);
    return 0;
}

#else

void MidiFileRemote::setHandler(MidiFileHandler *p) {
    midi = p;
}

int MidiFileRemote::openFile(const char *file, int mode) {
    uint8_t msg[64];
    initMessage(msg);
    if (mode == 1) {
        msg[5] = MIDI_MSG_OPEN_READ;
    }
    else {
        msg[5] = MIDI_MSG_OPEN_WRITE;
    }
    int len = strlen(file);
    msg[6] = len;
    strcpy((char*)(msg+7), file);
    msg[len+8] = 0xF7;
    midi->sendMessage(msg, len+9);
    return 0;
}

int MidiFileRemote::removeFile(const char *file) {
    uint8_t msg[64];
    initMessage(msg);
    msg[5] = MIDI_MSG_REMOVE;
    int len = strlen(file);
    msg[6] = len;
    strcpy((char*)(msg+7), file);
    msg[len+8] = 0xF7;
    midi->sendMessage(msg, len+9);
    return 0;
}

int MidiFileRemote::closeFile() {
    uint8_t msg[64];
    initMessage(msg);
    msg[5] = MIDI_MSG_CLOSE;
    msg[6] = 0xF7;
    midi->sendMessage(msg, 7);
    return 0;
}

int MidiFileRemote::writeFile(const char *buffer, int bytes) {
    uint8_t msg[128];
    initMessage(msg);
    int len;
    msg[5] = MIDI_MSG_WRITE;
    while(bytes > 0) {
        if (bytes > 32) {
            len = 32;
        }
        else {
            len = bytes;
        }
        bytes -= len;
        msg[6] = len;
        for(int i=0, j=0; i<len; i++, j+=2) {
            msg[7+j] = buffer[i] >> 4;
            msg[8+j] = buffer[i] & 0x0F;
        }
        msg[len * 2 + 6] = 0xF7;
        midi->sendMessage(msg, len * 2 + 7);
    }
    return 0;
}

int MidiFileRemote::readFile(char *buffer, int bytes) {
    uint8_t msg_cmd[8];
    initMessage(msg_cmd);
    msg_cmd[5] = MIDI_MSG_READ;
    int len;
    uint8_t *current = (uint8_t *)buffer;
    while(bytes > 0) {
        if (bytes > 32) {
            len = 32;
        }
        else {
            len = bytes;
        }
        bytes -= len;
        msg_cmd[6] = len;
        msg_cmd[7] = 0xF7;
        midi->sendMessage(msg_cmd, 8);
        uint8_t message[128];
        midi->readMessage(message, 128);
        if (message[5] == MIDI_MSG_DATA) {
            int rlen = message[6];
            for(int i=0, j=0; i<rlen; i++, j+=2) {
                *(current++) = (message[7+j] << 4) + message[8+j];
            }
        }
    }
    return 0;
}

#endif
