#include <string.h>
#include "MidiSer.h"

unsigned char staticBuffer[3*1024];

int MidiSerial::rev=2;

MidiSerial::MidiSerial()
{
    buffer = staticBuffer;
    bufferlen = sizeof(staticBuffer);
    end = buffer + bufferlen;
    init();
}

MidiSerial::~MidiSerial()
{
    
}

void MidiSerial::init()
{
    buffer[0] = 0x0; // placeholder for type of msg
    current = buffer+1;
}

unsigned char *MidiSerial::getBufferData()
{
    return buffer;
}

unsigned char *MidiSerial::getBufferEnd()
{
    return current+1;
}

unsigned char *MidiSerial::getCurrent()
{
    return current;
}

void MidiSerial::resetCurrent()
{
    current = buffer+1;
}

int MidiSerial::getLength()
{
    return (int)(current - buffer);
}

void MidiSerial::setType(int dtype)
{
    buffer[0] = (unsigned char)dtype;
}

int MidiSerial::getType()
{
    return buffer[0];
}

void MidiSerial::addFloat(float number)
{
    uint16_t n = (uint16_t)(number * 1000.0F);
    addWord(n);
}

void MidiSerial::addFloat2(float number)
{
    uint32_t i = (uint32_t)number;
    addWord2(i);
    uint32_t n = (uint32_t)((number - i) * 1000.0F);
    addWord(n);
}

void MidiSerial::addByte(unsigned char item)
{
    current[0] = item;
    current++;
}

void MidiSerial::addSByte(int8_t item)
{
    current[0] = (unsigned char)item;
    current++;
}

void MidiSerial::addByteArray(unsigned char *array, int len)
{
    for(int i=0; i<len; i++) {
        current[i] = array[i];
    }
    current += len;
}

void MidiSerial::addBufferArray(unsigned char *array, int len)
{
    if (buffer[0]==0x00) { // no type set, start at beginning
        current = buffer;
    }
    for(int i=0; i<len; i++) {
        current[i] = array[i];
    }
    current += len;
}

void MidiSerial::addString(const char *string)
{
    int len = (int)strlen(string)+1; // include 0 at end of string
    addWord(len);
    addByteArray((unsigned char*)string, len);
}

void MidiSerial::addWord(int number)
{
    current[0] = number & 0x7F;
    current[1] = (number >> 7) & 0x7F;
    current += 2;
}

void MidiSerial::addWord2(int number)
{
    current[0] = number & 0x7F;
    current[1] = (number >> 7) & 0x7F;
    current[2] = (number >> 14) & 0x7F;
    current[3] = (number >> 21) & 0x7F;
    current += 4;
}

void MidiSerial::addWord4(int number)
{
    current[0] = number & 0x7F;
    for (int i=1; i<8; i++) {
        current[i] = (number >> (i*7)) & 0x7F;
    }
    current += 8;
}

void MidiSerial::addWord3(int number)
{
    current[0] = number & 0x7F;
    for (int i=1; i<6; i++) {
        current[i] = (number >> (i*7)) & 0x7F;
    }
    current += 6;
}

float MidiSerial::getFloat()
{
    int num = getWord();
    return num / 1000.0F;
}

float MidiSerial::getFloat2()
{
    float num = (float)getWord2();
    float dec = (float)getWord();
    return num + dec / 1000.0F;
}

unsigned char MidiSerial::getByte()
{
    return *(current++);
}

int8_t MidiSerial::getSByte()
{
    return (int8_t)(*(current++));
}

unsigned char *MidiSerial::getByteArray(unsigned char *array, int length)
{
    for(int i=0; i<length; i++) {
        array[i] = current[i];
    }
    current += length;
    return array;
}

void MidiSerial::getBufferArray(unsigned char *array, int length)
{
    for(int i=0; i<length; i++) {
        array[i] = buffer[i];
    }
}

char * MidiSerial::getString(char *string)
{
    int len = getWord(); // includes 0 at end of string
    unsigned char * ret = getByteArray((unsigned char *)string, len);
    //ret[len-1] = 0;
    return (char*)ret;
}

int MidiSerial::getWord()
{
    uint16_t n;
    n = current[0] & 0x7F;
    n |= current[1] << 7;
    current += 2;
    return n;
}

int8_t MidiSerial::getWordH()
{
    int8_t n;
    n = current[0] & 0x7F;
    current += 2;
    return n;
}

int MidiSerial::getWord2()
{
    int n;
    n = current[0];
    n += (current[1] << 7);
    n += (current[2] << 14);
    n += (current[3] << 21);
    current += 4;
    return n;
}

int MidiSerial::getWord4()
{
    int n;
    n = current[0];
    for(int i=1; i<8; i++) {
        n += current[i] << (i*7);        
    }
    current += 8;
    return n;
}

int MidiSerial::getWord3()
{
    int n;
    n = current[0];
    for(int i=1; i<6; i++) {
        n += current[i] << (i*7);
    }
    current += 6;
    return n;
}

//
//
//

MidiChunk::MidiChunk(MidiSerial *ser)
{
    serial = ser;
    beginning = serial->getBufferData();
    end = serial->getCurrent();
    remain = (int)(end - beginning);
    current = beginning;
    maxlength = 48;
    first = 1;
    init(buffer);
}

void MidiChunk::init(unsigned char *data)
{
    data[0] = 0xF0;
    data[1] = MIDI_MFG1;
    data[2] = MIDI_MFG2;
    data[3] = MIDI_MFG3;
    data[4] = 0x00;
    data[5] = 0x00;
}

int MidiChunk::loadNextChunk()
{
    int ret = 0;
    if (remain > maxlength) { // truncate
        buffer[4] = 0x10 | (unsigned char)first;
        length = maxlength;
        ret = 1;
    }
    else {
        buffer[4] = 0x20 | (unsigned char)first;
        length = remain;
        ret = 0;
    }
    first = 0;
    remain -= length;
    buffer[5] = (unsigned char)length;
    unsigned char *start = buffer+6;
    for(int i=0; i<length; i++) {
        *start = *current;
        start++;
        current++;
    }
    return ret;
}

unsigned char *MidiChunk::getChunk(int *len)
{
    if (len) *len = length+7;
    buffer[6+length] = 0xF7;
    return buffer;
}

int MidiChunk::appendNextChunk(unsigned char *data)
{
    int cont = data[4];
    int len = data[5];
    unsigned char *start = data+6;
    if (cont & 1) {
        serial->init();
    }
    serial->addBufferArray(start, len);
    if (cont & 0x20) {
        serial->resetCurrent();
        return 0;
    }
    return 1;
}

float MidiSerial::formatMidi2Float(const unsigned char *data)
{
    float sign;
    if (data[0]) sign = -1;
    else sign = 1;
    int num = data[1];
    num += data[2] << 7;
    num += data[3] << 14;
    num += data[4] << 21;
    return sign * float(num) / 1000.0F;
}

int MidiSerial::formatFloat2Midi(float number, unsigned char *data)
{
    if (number < 0) {
        data[0] = 1;
        number = -number;
    }
    else {
        data[0] = 0;
    }
    int num = (int)(number * 1000.0F);
    data[1] = num & 0x7F;
    data[2] = (num >> 7) & 0x7F;
    data[3] = (num >> 14) & 0x7F;
    data[4] = (num >> 21) & 0x7F;
    return 5;
}
