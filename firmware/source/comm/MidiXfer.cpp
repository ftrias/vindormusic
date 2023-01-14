#include <Arduino.h>
#include "MidiXfer.h"
#include "Constants.h"

void MidiXfer::initBuffer()
{
    buffer[0] = 0xF0;
    buffer[1] = MIDI_MFG1;
    buffer[2] = MIDI_MFG2;
    buffer[3] = MIDI_MFG3;
    buffer[4] = 0xF7; // just in case, put terminator
    buffer_last = 4;
}

int MidiXfer::setString(const char *data)
{
    while(*data) {
        setByte(*data++);
    }
    setByte(0);
    return 1;
}

int MidiXfer::setByteRaw(int data)
{
    buffer[buffer_last++] = data;
    return 1;
}

int MidiXfer::setByte(int data)
{
    buffer[buffer_last++] = data & 0x7F;
    return 1;
}

int MidiXfer::setInt(int data)
{
    setByte(data);
    setByte(data >> 7);
    setByte(data >> 14);
    setByte(data >> 21);
    return 4;
}

int MidiXfer::setFloat(float data, int factor)
{
    int n = data * factor;
    setInt(factor);
    return setInt(n);
}

int MidiXfer::transmitMidi()
{
    setByteRaw(0xF7);
#ifdef ARDUINO
    usbMIDI.sendSysEx(buffer_last, (const uint8_t*)buffer, true);
#else
    // TODO: Win & Mac
#endif
    return 1;
}

bool MidiXfer::sendControl(int control, int data)
{
    initBuffer();
    setByte(DataType::controlint);
    setByte(control);
    setInt(data);
    transmitMidi();
    return true;
}

bool MidiXfer::initKey(const char *data, DataType datatype)
{
    initBuffer();
    setByte(datatype);
    setString(data);
    return true;
}

bool MidiXfer::sendNameInt(const char *name, int data)
{
    initKey(name, DataType::keyinteger);
    setInt(data);
    transmitMidi();    
    return true;
}

bool MidiXfer::sendNameFloat(const char *name, float data)
{
    initKey(name, DataType::keyfloat);    
    setFloat(data);
    transmitMidi();
    return true;
}

bool MidiXfer::sendNameString(const char *name, const char *data)
{
    initKey(name, DataType::keyfloat);    
    setString(data);
    transmitMidi();
    return true;
}

bool MidiXfer::loadBuffer(const char *message, int numbytes)
{
    buffer_last = 0;
    for(int i=0; i<numbytes; i++) {
        buffer[buffer_last++] = message[i];
        if (message[i] == 0xF7) break;
        if (buffer_last > block_max) return false;
    }
    return true;
}

MidiXfer::DataType MidiXfer::getType()
{
    return (MidiXfer::DataType) int(buffer[4]);
}

const char * MidiXfer::getKey()
{
    return buffer + 5;
}

const char * MidiXfer::skipKey()
{
    char *p = buffer + 5;
    while(*p) p++;
    return p+1;
}

int MidiXfer::asInt(const char *place)
{
    int n = place[0];
    n += place[1] << 7;
    n += place[2] << 14;
    n += place[3] << 21;
    return n;
}

float MidiXfer::asFloat(const char *place)
{
    float factor = asInt(place);
    place += 4;
    float n = asInt(place);
    return n / factor;
}

float MidiXfer::getFloat()
{
    DataType t = getType();
    float n;
    const char *p;
    switch(t) {
        case DataType::controlint:
            n = asInt(buffer+6);
            break;
        case DataType::controlfloat:
            n = asFloat(buffer+6);
            break;
        case DataType::keyinteger:
            p = skipKey();
            n = asInt(p);
            break;
        case DataType::keyfloat:
            p = skipKey();
            n = asFloat(p);
            break;
        default:
            n = 0.0F;
            break;
    }
    return n;
}

int MidiXfer::getInt()
{
    return getFloat();
}

const char *MidiXfer::getString()
{
    #if 0 // BUG!!!!
    DataType t = getType();
    switch(t) {
        case DataType::controlstring:
            return buffer + 6;
        case DataType::keystring:
            skipKey();        
            return buffer;
    }    
    #endif
    return "";
}

int MidiXfer::getDataSize()
{
    DataType t = getType();
    if (t == DataType::controlbytes) {
        return asInt(buffer+6);        
    }    
    return 0;
}

int MidiXfer::copyData(unsigned char *dest)
{
    int sz = asInt(buffer+6);
    int offset = asInt(buffer+10);
    unsigned char num;
    unsigned char *src = (unsigned char*)(buffer+14);
    for(int i=0; i<sz; i++) {
        num = *src++;
        num = (*src++) << 7;
        dest[offset] = num;
    }
    return 0;
}