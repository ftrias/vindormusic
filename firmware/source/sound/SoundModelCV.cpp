#include "SoundModelCV.h"

#ifdef USE_SIMPLE_CV

// produces linear scale from -1 = 0V to +1 = 3V
const float cv_vol = 0.825;

class AudioCV : public AudioSynthWaveformDc {
    public:
    void setV(float v) {
        // output of HP is offset so -0.3 = 0V
        amplitude(v * 2.2F - 1.0F);
    }
    void setF(float v) {
        // output of the headphones is range is 0V to +3.3V
        // each 0.1 amplitude corresponds to 1V
        // and there is a small offset of 0.08.
        amplitude(0.666667F * v - 1.08F);
    }
};

AudioCV     cv_out_freq; // for CV controls
AudioCV     cv_out_vol; // for CV controls

AudioStream &SoundModelCV::getOutput() {
    return cv_out_vol;
}

AudioStream &SoundModelCV::getFrequencyOutput() {
    return cv_out_freq;
}

AudioStream &SoundModelCV::getVolumeOutput() {
    return cv_out_vol;
}

void SoundModelCV::setVolume(float vol) {
    cv_out_vol.setV(vol);
}

void SoundModelCV::resetFilter(SynthSettings *sound) {
    setOneWire(1);
}

void SoundModelCV::setFreq(float freq) {
    cv_out_freq.setF(freq);
}

void SoundModelCV::noteOn(int note, int attack)  {
    cv_out_freq.setVolume(10);
}

#endif

#ifdef USE_EXTERNAL_CV

#include <Audio.h>

#include "Display.h"
#include "vdebug.h"

#include <OneWire.h>
#include "DS28E17.h"

OneWire m_onewire(UART_SENSE_PIN_TX);
DS28E17 m_ds(&m_onewire);

class OneWireCV {
    private:

    const int i2c_addr = 0x0f;
    const int family_1w = 25;
    uint8_t sensor_addr[8];

    public:

    bool connect() {
        m_onewire.reset_search();
        m_onewire.target_search(family_1w);
        sensor_addr[2] = 0xff;
        bool ok = m_onewire.search(sensor_addr);
        if (! ok) {
            // showDebug("1W search failed");
            return false;
        }
        if (sensor_addr[2]==0 || sensor_addr[2]==0xff) {
            return false;
        }
        m_ds.setAddress(sensor_addr);

        char b[64];
        sprintf(b, "1W:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:", 
            sensor_addr[0], sensor_addr[1], sensor_addr[2], sensor_addr[3],
            sensor_addr[4], sensor_addr[5], sensor_addr[6], sensor_addr[7]);
        showDebug(b);

        delay(1);
        return true;
    }

    void setV(float v) {
        int dac = 65535 * v;
        uint8_t cmd[3];
        cmd[0] = 0b00011000;
        cmd[1] = dac >> 8;
        cmd[2] = dac & 0xff;
        bool ok = m_ds.write(i2c_addr, cmd, 3);
        if (! ok) showDebug("1W Volume write failed");
    }

    void setF(float v) {
        int dac = 65535 * v;
        uint8_t cmd[3];
        cmd[0] = 0b00011001;
        cmd[1] = dac >> 8;
        cmd[2] = dac & 0xff;
        bool ok = m_ds.write(i2c_addr, cmd, 3);
        debugOutputDecimal(dac);
        if (! ok) showDebug("1W Frequency write failed");
    }

    void setTrigger(int state) {
        int dac = state ? 65535 : 0;
        uint8_t cmd[3];
        cmd[0] = 0b00011001;
        cmd[1] = dac >> 8;
        cmd[2] = dac & 0xff;
        bool ok = m_ds.write(i2c_addr, cmd, 3);
        debugOutputDecimal(dac);
        if (! ok) showDebug("1W Trigger write failed");
    }
};

OneWireCV onewire_cv;

void SoundModelCV::setOneWire(int state) {
    // showDebug("OW");
    digitalWrite(SWITCH_HP_MIDI_PIN, LOW);
    // pinMode(UART_SENSE_PIN_TX, OUTPUT);
    // digitalWrite(UART_SENSE_PIN_TX, LOW);
    if (onewire_cv.connect()) {
        onewire_cv.setV(0.0);
        onewire_cv.setF(1.0);
    }
    debugOutputDecimal(0);
}

AudioSynthWaveformDc dummy;

AudioStream &SoundModelCV::getOutput() {
    return dummy;
}

void SoundModelCV::setVolume(float vol) {
    onewire_cv.setV(vol);
}

void SoundModelCV::resetFilter(SynthSettings *sound) {
    setOneWire(1);
}

void SoundModelCV::setFreq(float freq) {
    onewire_cv.setF(freq);
}

void SoundModelCV::noteOn(int note, int attack)  {
    onewire_cv.setTrigger(1);
}

#endif
