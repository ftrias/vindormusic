#ifndef MIDIFILE_H
#define MIDIFILE_H

#include <stdint.h>
#include "Constants.h"

#define MIDI_MSG_FILE 0x18

#define MIDI_MSG_OPEN_READ 0x01
#define MIDI_MSG_OPEN_WRITE 0x02
#define MIDI_MSG_CLOSE 0x03
#define MIDI_MSG_REMOVE 0x04
#define MIDI_MSG_WRITE 0x05
#define MIDI_MSG_READ 0x06
#define MIDI_MSG_DATA 0x10
#define MIDI_MSG_OK 0x20
#define MIDI_MSG_FAIL 0x21

#ifdef ARDUINO

#include "firmware.h"
#include "SdLib.h"

class MidiFileDevice {
    private:
    File file;

    public:
    int processMessage(const unsigned char *message, int bytes);
    int processMessageRead(const unsigned char *message, int bytes);
    int processMessageWrite(const unsigned char *message, int bytes);

    int open(const char *file);
    int close();
    int read(char *buffer, int bytes);
    int write(const char *buffer, int bytes);
};

#else

class MidiFileHandler {
public:
    int sendMessage(unsigned char *data, int len);
    int readMessage(unsigned char *data, int len);
};

class MidiFileRemote {
    MidiFileHandler *midi;
public:
    void setHandler(MidiFileHandler *p);
    int openFile(const char *file, int mode);
    int closeFile();
    int removeFile(const char *dir);
    int readFile(char *buffer, int bytes);
    int writeFile(const char *buffer, int bytes);
    int seekFile(int position);
};

#endif

#endif
