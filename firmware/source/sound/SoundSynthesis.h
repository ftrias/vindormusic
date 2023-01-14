#ifndef SoundSynthesis_h
#define SoundSynthesis_h

// #include "Instrument.h"
#include "synth_waveform.h"
#include "SynthSettings.h"
#include "vutils.h"

/*
namespace Settings {
    extern float baseFreqA4;
}
*/

const float usbVolume = 0.75F;
// if we change codec vol above 0.5, the touch sensors become less responsive 20160609??!?!
// const int hpVolume = 0x18;

const int fil_chorus1_len = 16 * AUDIO_BLOCK_SAMPLES;
extern int16_t fil_chorus1_buffer[];
// const int16_t fil_flange1_len = 8 * AUDIO_BLOCK_SAMPLES;
// extern int16_t fil_flange1_buffer[];

const int scalevolume = 65535;
const int volume2midi = 9;
const int volume2midi_hires = 2;


/**
 * @brief Abstract class used to implement different synthesis models
 * 
 */
class SoundModel {
    public:
        /**
         * @brief Get the Output object for use in  AudioConnection object
         * 
         * @return AudioStream& 
         */
        virtual AudioStream &getOutput();

        /**
         * @brief Set the Volume object
         * 
         * @param playVolume Volume from 0.0 to 1.0
         */
        virtual void setVolume(float playVolume);

        /**
         * @brief Initialize syth parameters
         * 
         * @param sound SynthSettings pointer (must not be deallocated when using)
         */
        virtual void resetFilter(SynthSettings *sound);

        /**
         * @brief Set the Frequency
         * 
         * @param freq Freq in Hz
         */
        virtual void setFreq(float freq);

        /**
         * @brief Start a new note
         * 
         * @param note MIDI note number
         * @param attack 1 to use attack (0 if no attack, as in note changes)
         */
        virtual void noteOn(int note,  int attack);
};

/**
 * @brief Manage instruments and note playing state for built-in synth
 * 
 */
class SoundSynthesis {
public:
    // const Instrument *playInstrument;

    // static float codecVolume;

    int playingNote;
    // int pitchBend;
    EMA<2> pitchBend;
    int newBend;
    float playFreq;
    float playVolume;

#if defined(USE_SIMPLE_CV) || defined(USE_EXTERNAL_CV)
    int useCV = false;
#endif

    typedef enum { EQUAL_TEMPERAMENT = 1, BAGPIPE_TEMPERAMENT,  JUST_TEMPERAMENT, MEANTONE_TEMPERAMENT } TemperamentType;
    TemperamentType temperament = EQUAL_TEMPERAMENT;

    SynthSettings *sound;

    static float invert;

    float lastCodecVolume;

    float midi2freq(int note, float part = 0);
    float midi2freq_hz(int note, float part = 0);
    float midi2freq_cv(int note, float part = 0, int type = 1);
    const char *note2wav(int note);
    void getPrefix(char *prefix, int inst);

public:
    int sdcard;
    int sdcardpresent = false;
    int sdcardinit = false;

    float partvol[4];
    float partratio[4];

    int lastError;

    SoundSynthesis();

    static int getDebug(int id);

    void disable();
    void sleepState(int sleep);
    void powerHeadphone(bool state);
    void powerLineout(bool state);

    int setInstrument(int inst);
    // bool isInstrument(InstrumentMidiType inst);
    // const Instrument *getInstrument();
    
    void runFastTasks();
    void runSlowTasks();

    float deltaCodecVolume(float change);
    float setCodecVolume(float vols);

    void resetFilter();
    void setFreq(float freq, bool nointerrupts = true);
    void updateFilterFreq();

    void changeVolume(int volume);
    void startNote(int note, int volume, int attack);
    void restartNote();
    void stopNote(int note, int volume);
    bool stopWait();
    void setBend(int bend);
    void setModulateVibrato(int number);
    void setModulateTremolo(int tremolo);
    void processBend();

#if defined(USE_SIMPLE_CV) || defined(USE_EXTERNAL_CV)
    void setCV(bool state);
#endif
    void setInvert(int state);
    void setDifferential(int state);

    void setEffect(int effect, float param1, float param2=0);

    // void rampUp();
    void dump_memory();
#ifdef USE_ADD_NOISE
    void setInterference(int state);
#endif
    void setMainVolume(float vol);
    void setOutputMixer(int ch=-99);
    bool initializeSound();
    bool initSD();
    bool begin();
    void enable();

    static void alternate_i2s(void);

    int getNameForInstrument(char *s, int inst);

#ifdef USE_SD_DIR
    int loadDirList(char *dest);
    int setSDInst(int num);
#endif

    // bool readSoundInfo(const char *prefix);
    // bool fileExists(const char *filename);
    bool playSound(const char *filename);
    void playMem(const int8_t *data, int size);
    void playMem(const uint8_t *data, int size);

    // void updateData(int slot, int16_t *data, uint32_t len);
};

float mapParam(float input, float minInput, float maxInput, float minOutput, float maxOutput);

extern SoundSynthesis synth;

#endif
