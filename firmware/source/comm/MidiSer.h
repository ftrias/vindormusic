#ifndef MIDISER_H
#define MIDISER_H

#include <stdint.h>
#include "Constants.h"

#ifdef ARDUINO
#include "firmware.h"
#endif

class MidiSerial
{
private:
    unsigned char *buffer;
    unsigned char *mybuffer;
    unsigned char *current;
    unsigned char *end;
    int bufferlen;
    
    void requestSize(int newsize);
    
public:
    MidiSerial();
    ~MidiSerial();
    
    unsigned char *getBufferData();
    unsigned char *getBufferEnd();
    unsigned char *getCurrent();
    int getLength();
    void resetCurrent();
    
    void terminate();
    void init();
    // void clear();
    
    void setType(int datatype);
    int getType();
    
    void addFloat(float number);
    void addFloat2(float number);
    void addByte(unsigned char item);
    void addSByte(int8_t item);
    void addByteArray(unsigned char *array, int len);
    void addBufferArray(unsigned char *array, int len);
    void addWord(int number);
    void addWord2(int number);
    void addWord3(int number);
    void addWord4(int number);
    void addString(const char *string);
    
    float getFloat();
    float getFloat2();
    unsigned char getByte();
    int8_t getSByte();
    
    unsigned char *getByteArray(unsigned char *array, int len);
    void getBufferArray(unsigned char *array, int len);
    char *getString(char *string);
    int getWord();
    int8_t getWordH();
    int getWord2();
    int getWord3();
    int getWord4();
    
    static float formatMidi2Float(const unsigned char *data);
    static int formatFloat2Midi(float number, unsigned char *data);
    
    static int rev;
};

class MidiChunk {
private:
    MidiSerial *serial;
    unsigned char *beginning;
    unsigned char *current;
    unsigned char *end;
    int first;
    
    unsigned char buffer[64];
    int length;
    int maxlength;
    int remain;
    void init(unsigned char *data);
    
public:
    MidiChunk(MidiSerial *ser);
    void loadAll(unsigned char *data, int *len);
    int loadNextChunk();
    int appendNextChunk(unsigned char *data);
    unsigned char *getChunk(int *len);
};

#endif
