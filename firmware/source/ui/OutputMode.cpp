#include "firmware.h"
#include "OutputMode.h"
#include "SoundSynthesis.h"
#include "Lights.h"


void OutputMode::enableSilence(bool on) {
    if (on) {
        // setHeadphone(0, 0);
        synth.sleepState(true);
        setSpeaker(0, 0);
        setLineout(0, 0);
        setHeadphone(0, 0);
    }
    else {
        // setHeadphone(headphoneState, 0);
        synth.sleepState(false);
        setSpeaker(speakerState, 0);
        setLineout(lineoutState, 0);
        setHeadphone(headphoneState, 0);
    }
}

void OutputMode::setHeadphone(bool on, bool save) {
#ifdef HEADPHONE_PIN
    if (on) {
        digitalWrite(HEADPHONE_PIN, HIGH);
    }
    else {
        digitalWrite(HEADPHONE_PIN, LOW);
    }
#endif
#ifdef USE_SGTL5000
    synth.powerHeadphone(on);
#endif
    if (save) headphoneState = on;
}

void OutputMode::setLineout(bool on, bool save) {
#ifdef USE_SGTL5000
    synth.powerLineout(on);
#endif
    if (save) lineoutState = on;
}

void OutputMode::setSpeaker(bool on, bool save) {
    if (on) {
        synth.powerLineout(true);
        synth.setDifferential(true);
        digitalWrite(AMP_PIN, HIGH);
    } else {
        synth.setDifferential(false);
        digitalWrite(AMP_PIN, LOW);
    }
    if (save) speakerState = on;
}

void OutputMode::powerOff() {
    synth.powerLineout(false);
    synth.powerHeadphone(false);
    setSpeaker(0);
}

void OutputMode::begin() {
    playMode = 0;
    synth.powerLineout(false);
    synth.powerHeadphone(false);
}
