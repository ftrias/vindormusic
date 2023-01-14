#ifndef SYNTHSETTINGS_H
#define SYNTHSETTINGS_H

#include "MidiSer.h"

#define WAVEFORM_SINE              0
#define WAVEFORM_SAWTOOTH          1
#define WAVEFORM_SQUARE            2
#define WAVEFORM_TRIANGLE          3
#define WAVEFORM_ARBITRARY         4
#define WAVEFORM_PULSE             5
#define WAVEFORM_SAWTOOTH_REVERSE  6
#define WAVEFORM_SAMPLE_HOLD       7
#define WAVEFORM_TRIANGLE_VARIABLE 8

#define ANALOG_MODEL               0
#define PHYSICAL_MODEL             1
#define WAVETABLE_MODEL            2
#define WAVESF2_MODEL              3

/*
 * Model parameters
 */

class analogModelParams {
public:
  float filterResonance;
  float filterFreq;
  int8_t fil_wave[6];
  float fil_vol[6];
  float fil_vol_noise;
  float fil_freq[6];
  float fil_freq_bias[6];
  float fil_lfo_freq;
  float fil_lfo_delay;
  float fil_lfo_mix[3];
  int8_t fil_lfo_wave;
  int8_t fil_chorus1_num;
  int8_t playAttack;
  // int8_t fil_echo_num;
};

class physicalModelParams {
public:
  int8_t tone;
  int8_t smoothness;
  int8_t echo;
  int8_t table;
};

class wavetableModelParams {
public:
  int8_t table;
  float filterFreq;
};
class SynthSettings {
  public:
    int midiNumber;
    int synth_use;
    union  {
      analogModelParams analog;
      physicalModelParams physical;
      wavetableModelParams wavetable;
    } u;

    void loadDefaults_analog() {
      synth_use = ANALOG_MODEL;
      u.analog.filterResonance = 1.2F;
      u.analog.filterFreq = 5000.0F;
      u.analog.fil_wave[0] = WAVEFORM_SQUARE;
      u.analog.fil_wave[1] = WAVEFORM_SQUARE;
      u.analog.fil_wave[2] = WAVEFORM_TRIANGLE;
      u.analog.fil_wave[3] = WAVEFORM_TRIANGLE;
      u.analog.fil_wave[4] = WAVEFORM_TRIANGLE;
      u.analog.fil_wave[5] = WAVEFORM_TRIANGLE;
      u.analog.fil_vol[0] = 0.5F;
      u.analog.fil_vol[1] = 0.24F;
      u.analog.fil_vol[2] = 0.24F;
      u.analog.fil_vol[3] = 0.0F;
      u.analog.fil_vol[4] = 0.0F;
      u.analog.fil_vol[5] = 0.0F;
      u.analog.fil_freq[0] = 1.0F;
      u.analog.fil_freq[1] = 2.0F;
      u.analog.fil_freq[2] = 0.5F;
      u.analog.fil_freq[3] = 3.0F;
      u.analog.fil_freq[4] = 4.0F;
      u.analog.fil_freq[5] = 5.0F;
      u.analog.fil_freq_bias[0] = 1.0F;
      u.analog.fil_freq_bias[1] = 1.0F;
      u.analog.fil_freq_bias[2] = 1.0F;
      u.analog.fil_freq_bias[3] = 1.0F;
      u.analog.fil_freq_bias[4] = 1.0F;
      u.analog.fil_freq_bias[5] = 1.0F;
      u.analog.fil_lfo_freq = 10.0F;
      u.analog.fil_lfo_delay = 2000.0F;
      u.analog.fil_lfo_mix[0] = 0.0F;
      u.analog.fil_lfo_mix[1] = 0.0F;
      u.analog.fil_lfo_mix[2] = 0.0F;
      u.analog.fil_chorus1_num = 1;
      u.analog.fil_lfo_wave = WAVEFORM_SINE;
      u.analog.fil_vol_noise = 0.0F;
      u.analog.playAttack = 0;
    }

    void loadDefaults_physical() {
      synth_use = PHYSICAL_MODEL;
      u.physical.tone = 64;
      u.physical.smoothness = 0;
      u.physical.echo = 0;
      u.physical.table = 0;
    }

    void loadDefaults_wavesf2() {
      synth_use = WAVESF2_MODEL;
      midiNumber = 67;
    }

    void loadDefaults_synth() {
      switch(synth_use) {
        case ANALOG_MODEL:   loadDefaults_analog(); break;
        case PHYSICAL_MODEL: loadDefaults_physical(); break;
        case WAVESF2_MODEL:  loadDefaults_wavesf2(); break;
      }
    }

    void loadDefaults_0() {
      loadDefaults_wavesf2();
      midiNumber = 67;
    }

    void loadDefaults_1() {
      loadDefaults_physical();
    }

    void loadDefaults_2() {
      loadDefaults_analog();
      synth_use = ANALOG_MODEL;
      u.analog.filterResonance = 1.338000F;
      u.analog.filterFreq = 3900.000000F;
      u.analog.fil_wave[0] = WAVEFORM_SQUARE;
      u.analog.fil_wave[1] = WAVEFORM_SAWTOOTH;
      u.analog.fil_wave[2] = WAVEFORM_SQUARE;
      u.analog.fil_vol[0] = 0.3F;  //1.0;
      u.analog.fil_vol[1] = 0.25F; //0.818;
      u.analog.fil_vol[2] = 0.23F; //0.755;
      u.analog.fil_freq[0] = 1.000000F;
      u.analog.fil_freq[1] = 1.000000F;
      u.analog.fil_freq[2] = 0.250000F;
      u.analog.fil_freq_bias[0] = 1.000000F;
      u.analog.fil_freq_bias[1] = 1.005000F;
      u.analog.fil_freq_bias[2] = 1.000000F;
      u.analog.fil_lfo_freq = 5.199000F;
      u.analog.fil_lfo_delay = 2507.000000F;
      u.analog.fil_lfo_mix[0] = 0.047000F;
      u.analog.fil_lfo_mix[1] = 0.047000F;
      u.analog.fil_lfo_mix[2] = 0.047000F;
      u.analog.fil_chorus1_num = 1;
      u.analog.fil_lfo_wave = WAVEFORM_SINE;
    }  

    void loadDefaults_3() {
      loadDefaults_analog();
      synth_use = ANALOG_MODEL;
      u.analog.filterResonance = 1.574000F;
      u.analog.filterFreq = 7616.000000F;
      u.analog.fil_wave[0] = WAVEFORM_SINE;
      u.analog.fil_wave[1] = WAVEFORM_SINE;
      u.analog.fil_wave[2] = WAVEFORM_SINE;
      u.analog.fil_vol[0] = 0.503000F;
      u.analog.fil_vol[1] = 0.104000F;
      u.analog.fil_vol[2] = 0.052000F;
      u.analog.fil_freq[0] = 1.000000F;
      u.analog.fil_freq[1] = 2.000000F;
      u.analog.fil_freq[2] = 4.000000F;
      u.analog.fil_freq_bias[0] = 1.000000F;
      u.analog.fil_freq_bias[1] = 1.000000F;
      u.analog.fil_freq_bias[2] = 1.000000F;
      u.analog.fil_lfo_freq = 6.599000F;
      u.analog.fil_lfo_delay = 2000.000000F;
      u.analog.fil_lfo_mix[0] = 0.007000F;
      u.analog.fil_lfo_mix[1] = 0.007000F;
      u.analog.fil_lfo_mix[2] = 0.007000F;
      u.analog.fil_chorus1_num = 1;
      u.analog.fil_lfo_wave = WAVEFORM_SINE;
    }

    void loadDefaults_4() {
      loadDefaults_analog();
      synth_use = ANALOG_MODEL;
      u.analog.filterResonance = 1.023000F;
      u.analog.filterFreq = 7900.000000F;
      u.analog.fil_wave[0] = WAVEFORM_SAWTOOTH;
      u.analog.fil_wave[1] = WAVEFORM_SAWTOOTH;
      u.analog.fil_wave[2] = WAVEFORM_SAWTOOTH;
      u.analog.fil_vol[0] = 0.834000F;
      u.analog.fil_vol[1] = 0.700000F;
      u.analog.fil_freq[0] = 1.000000F;
      u.analog.fil_freq[1] = 0.500000F;
      u.analog.fil_freq[2] = 0.250000F;
      u.analog.fil_freq_bias[0] = 1.000000F;
      u.analog.fil_freq_bias[1] = 1.005000F;
      u.analog.fil_freq_bias[2] = 1.000000F;
      u.analog.fil_lfo_freq = 8.300000F;
      u.analog.fil_lfo_delay = 2000.000000F;
      u.analog.fil_lfo_mix[0] = 0.000000F;
      u.analog.fil_lfo_mix[1] = 0.000000F;
      u.analog.fil_lfo_mix[2] = 0.000000F;
      u.analog.fil_chorus1_num = 1;
      u.analog.fil_lfo_wave = WAVEFORM_SINE;
    }

    void loadDefaults_5() {
      loadDefaults_analog();
      synth_use = ANALOG_MODEL;
      u.analog.playAttack = 1;
      u.analog.filterResonance = 2.300000F;
      u.analog.filterFreq = 500.000000F;
      u.analog.fil_wave[0] = WAVEFORM_SAWTOOTH;
      u.analog.fil_wave[1] = WAVEFORM_SINE;
      u.analog.fil_wave[2] = WAVEFORM_SQUARE;
      u.analog.fil_vol[0] = 0.758000F;
      u.analog.fil_vol[1] = 0.10000F;
      u.analog.fil_vol[2] = 0.20000F;
      u.analog.fil_freq[0] = 1.000000F;
      u.analog.fil_freq[1] = 0.500000F;
      u.analog.fil_freq[2] = 2.000000F;
      u.analog.fil_freq_bias[0] = 1.000000F;
      u.analog.fil_freq_bias[1] = 1.011000F;
      u.analog.fil_freq_bias[2] = 1.000000F;
      u.analog.fil_lfo_freq = 3.600000F;
      u.analog.fil_lfo_delay = 700.000000F;
      u.analog.fil_lfo_mix[0] = 0.05F;
      u.analog.fil_lfo_mix[1] = 0.000000F;
      u.analog.fil_lfo_mix[2] = 0.000000F;
      u.analog.fil_chorus1_num = 1;
      u.analog.fil_lfo_wave = WAVEFORM_TRIANGLE;
    }

    void loadDefaults_6() {
      loadDefaults_analog();
      synth_use = ANALOG_MODEL;
      u.analog.filterResonance = 2.047000F;
      u.analog.filterFreq = 4700.000000F;
      u.analog.fil_wave[0] = WAVEFORM_SQUARE;
      u.analog.fil_wave[1] = WAVEFORM_SQUARE;
      u.analog.fil_wave[2] = WAVEFORM_SQUARE;
      u.analog.fil_vol[0] = 0.842000F;
      u.analog.fil_vol[1] = 0.826000F;
      u.analog.fil_vol[2] = 0.732000F;
      u.analog.fil_freq[0] = 1.000000F;
      u.analog.fil_freq[1] = 1.000000F;
      u.analog.fil_freq[2] = 0.500000F;
      u.analog.fil_freq_bias[0] = 1.000000F;
      u.analog.fil_freq_bias[1] = 1.005000F;
      u.analog.fil_freq_bias[2] = 1.005000F;
      u.analog.fil_lfo_freq = 7.500000F;
      u.analog.fil_lfo_delay = 0.000000F;
      u.analog.fil_lfo_mix[0] = 0.047000F;
      u.analog.fil_lfo_mix[1] = 0.047000F;
      u.analog.fil_lfo_mix[2] = 0.047000F;
      u.analog.fil_chorus1_num = 1;
      u.analog.fil_lfo_wave = WAVEFORM_SINE;
    }

    void loadDefaults_7() {
      loadDefaults_analog();
      synth_use = ANALOG_MODEL;
      u.analog.filterResonance = 1.4F;
      u.analog.filterFreq = 2800.000000F;
      u.analog.fil_wave[0] = WAVEFORM_SAWTOOTH;
      u.analog.fil_wave[1] = WAVEFORM_SQUARE;
      u.analog.fil_wave[2] = WAVEFORM_TRIANGLE;
      u.analog.fil_vol[0] = 0.85F;
      u.analog.fil_vol[1] = 0.50F;
      u.analog.fil_vol[2] = 0.45F;
      u.analog.fil_freq[0] = 1.000000F;
      u.analog.fil_freq[1] = 1.000000F;
      u.analog.fil_freq[2] = 2.0F;
      u.analog.fil_freq_bias[0] = 1.000000F;
      u.analog.fil_freq_bias[1] = 1.005000F;
      u.analog.fil_freq_bias[2] = 1.005000F;
      u.analog.fil_lfo_freq = 6.8F;
      u.analog.fil_lfo_delay = 1100.0F;
      u.analog.fil_lfo_mix[0] = 0.047000F;
      u.analog.fil_lfo_mix[1] = 0.047000F;
      u.analog.fil_lfo_mix[2] = 0.047000F;
      u.analog.fil_chorus1_num = 1;
      u.analog.fil_lfo_wave = WAVEFORM_TRIANGLE;
    }

    void loadDefaults_8() {
      loadDefaults_analog();
      synth_use = ANALOG_MODEL;
      u.analog.filterResonance = 3.464000F;
      u.analog.filterFreq = 12200.000000F;
      u.analog.fil_wave[0] = WAVEFORM_TRIANGLE;
      u.analog.fil_wave[1] = WAVEFORM_TRIANGLE;
      u.analog.fil_wave[2] = WAVEFORM_TRIANGLE;
      u.analog.fil_vol[0] = 1.000000F;
      u.analog.fil_vol[1] = 0.889000F;
      u.analog.fil_vol[2] = 0.598000F;
      u.analog.fil_freq[0] = 1.000000F;
      u.analog.fil_freq[1] = 1.000000F;
      u.analog.fil_freq[2] = 0.500000F;
      u.analog.fil_freq_bias[0] = 1.000000F;
      u.analog.fil_freq_bias[1] = 1.017000F;
      u.analog.fil_freq_bias[2] = 1.000000F;
      u.analog.fil_lfo_freq = 10.000000F;
      u.analog.fil_lfo_delay = 2000.000000F;
      u.analog.fil_lfo_mix[0] = 0.000000F;
      u.analog.fil_lfo_mix[1] = 0.000000F;
      u.analog.fil_lfo_mix[2] = 0.000000F;
      u.analog.fil_chorus1_num = 1;
      u.analog.fil_lfo_wave = WAVEFORM_SINE;
    }

    void loadInstrument(int n) {
        midiNumber = 81+n;
        switch(n) {
            case 0: loadDefaults_0(); break;
            case 1: loadDefaults_1(); break;
            case 2: loadDefaults_2(); break;
            case 3: loadDefaults_3(); break;
            case 4: loadDefaults_4(); break;
            case 5: loadDefaults_5(); break;
            case 6: loadDefaults_6(); break;
            case 7: loadDefaults_7(); break;
            case 8: loadDefaults_8(); break;
        }
    }

    void getSerialize(MidiSerial *ser) const
    {
      if (synth_use == ANALOG_MODEL) {
        ser->addByte(1);
        ser->addFloat(u.analog.filterResonance);
        ser->addFloat2(u.analog.filterFreq);
        ser->addByte(6);
        for (int i=0; i<6; i++) {
            ser->addByte(u.analog.fil_wave[i]);
            ser->addFloat(u.analog.fil_vol[i]);
            ser->addFloat(u.analog.fil_freq[i]);
            ser->addFloat(u.analog.fil_freq_bias[i]);
        }
        ser->addFloat(u.analog.fil_vol_noise);
        ser->addByte(u.analog.playAttack);
        ser->addWord(u.analog.fil_lfo_wave);
        ser->addFloat2(u.analog.fil_lfo_freq);
        ser->addFloat2(u.analog.fil_lfo_delay);
        ser->addByte(3);
        for (int i=0; i<3; i++) {
            ser->addFloat(u.analog.fil_lfo_mix[i]);
        }
        ser->addByte(u.analog.fil_chorus1_num);
        // ser->addByte(u.analog.fil_echo_num);
        ser->addWord(midiNumber);
      }
      else if (synth_use == PHYSICAL_MODEL) {
        ser->addByte(2);
        ser->addWord(u.physical.tone);
        ser->addWord(u.physical.smoothness);
        ser->addWord(u.physical.echo);
        ser->addWord(midiNumber);
        ser->addWord(u.physical.table);
      }
      else if (synth_use == WAVETABLE_MODEL) {
        ser->addByte(3);
        ser->addWord(u.wavetable.table);
        ser->addWord(midiNumber);
      }
      else if (synth_use == WAVESF2_MODEL) {
        ser->addByte(4);
        ser->addWord(midiNumber);
        ser->addWord(0);
      }
    }

    void setSerialize(MidiSerial *ser)
    {
        int ver = ser->getByte();
        if (ver==1) {
            synth_use = ANALOG_MODEL;
            u.analog.filterResonance = ser->getFloat();
            u.analog.filterFreq = ser->getFloat2();
            int count = ser->getByte();
            if (count>6) count = 6;
            for (int i=0; i<count; i++) {
                u.analog.fil_wave[i] = ser->getByte();
                u.analog.fil_vol[i] = ser->getFloat();
                u.analog.fil_freq[i] = ser->getFloat();
                u.analog.fil_freq_bias[i] = ser->getFloat();
            }
            u.analog.fil_vol_noise = ser->getFloat();
            u.analog.playAttack = ser->getByte();
            u.analog.fil_lfo_wave = ser->getWordH();
            u.analog.fil_lfo_freq = ser->getFloat2();
            u.analog.fil_lfo_delay = ser->getFloat2();
            count = ser->getByte();
            if (count>3) count=3;
            for (int i=0; i<count; i++) {
                u.analog.fil_lfo_mix[i] = ser->getFloat();
            }
            u.analog.fil_chorus1_num = ser->getByte();
            // u.analog.fil_echo_num = ser->getByte();
            midiNumber = ser->getWordH();
        }
      else if (ver==2) {
        synth_use = PHYSICAL_MODEL;
        u.physical.tone = ser->getWordH();
        u.physical.smoothness = ser->getWordH();
        u.physical.echo = ser->getWordH();
        midiNumber = ser->getWordH();
        u.physical.table = ser->getWordH();
      }
      else if (ver==3) {
        synth_use = WAVETABLE_MODEL;
        u.wavetable.table = ser->getWordH();
        midiNumber = ser->getWord();
      }
      else if (ver==4) {
        synth_use = WAVESF2_MODEL;
        midiNumber = ser->getWord();
        ser->getWord();
      }
    }
};

#endif
