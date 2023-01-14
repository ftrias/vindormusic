#ifndef OutputMode_H
#define OutputMode_H

class OutputMode {
public:
    bool enableMidi = true;
    // bool enableKeyboard = false;
    // bool enableAmplifier = false;
    
    bool headphoneState = false;
    bool lineoutState = false;
    bool speakerState = false;
    
    int playMode = 0;
    
    void enableSilence(bool on);
    void setHeadphone(bool on, bool save = 1);
    void setLineout(bool on, bool save = 1);
    void setSpeaker(bool on, bool save = 1);
    // void cycleMode();
    // void setMode();
    void begin();
    void powerOff();
};

extern OutputMode currentOutput;

#endif
