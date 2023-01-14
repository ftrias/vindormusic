#ifndef MIDIXFER_X
#define MIDIXFER_X

class MidiXfer {
public:
    enum DataType { controlint = 1, controlfloat = 2, controlstring = 3,
        controlbytes = 4,
        keyinteger = 5, keyfloat = 6, keystring = 7 };
private:
    const static int buffer_max = 64;
    const static int block_max = 48;
    char buffer[buffer_max];
    int buffer_last;

    void initBuffer();
    int setByte(int data);
    int setByteRaw(int data);
    int setString(const char *data);
    bool initKey(const char *data, DataType datatype);
    int setInt(int data);
    int setFloat(float data, int factor=1000);
    int asInt(const char *data);
    float asFloat(const char *data);
    const char *skipKey();
    int transmitMidi();
public:
    bool sendControl(int control, int data);
    bool sendNameInt(const char *name, int data);
    bool sendNameFloat(const char *name, float data);
    bool sendNameString(const char *name, const char*data);
    bool loadBuffer(const char *message, int numbytes);
    const char *getKey();
    DataType getType();
    int getInt();
    float getFloat();
    const char *getString();
    int getDataSize();
    int copyData(unsigned char *dest);
};

#endif