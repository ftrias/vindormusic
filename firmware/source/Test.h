#ifdef TEST_FINGERING
#include "Instrument.h"
extern const char *noteNames[];
void setup() {
    MAIN_SETUP();
    globalFingerTree.load(fingerSax);
    Fingering::setKeys(1);
    
#ifdef TEST_FINGERING_SOUND
    currentOutput.setHeadphone(true);
    currentOutput.setSpeaker(true);
#endif
}
void loop() {
    static int pb = -1;
    int b = getButtonsState();
    if (b==pb) return;
    pb = b;
    int bonly = b & 0b11111111111;
    int note = globalFingerTree.getNoteTree(bonly);
    int note2 = globalFingerTree.getNote(bonly);
    int noteX = fingering.getNote();
    Serial.print(b+0b10000000000000, BIN);
    Serial.print(" table=");
    Serial.print(note2);
    Serial.print("  ");
    Serial.print(noteNames[note2]);
    Serial.print("  tree=");
    Serial.print(note);
    Serial.print("  ");
    Serial.print(noteNames[note]);
    Serial.print("  ");
    Serial.println(noteX);
    Midi::midiSendDebug(noteX);
    Midi::midiSendDebug2(b);
#ifdef TEST_FINGERING_SOUND
    eventNoteOn(note+69, 256);
#endif
}
#endif

/*******************
 *
 *  Testing code
 *
 *******************/

#ifdef TEST
#define TEST_SAY_NUMBERS
#endif

#ifdef TEST_TOUCH
void pnum(int num)
{
    if (num>=0) Serial.print(" ");
    int x = abs(num);
    if (x<10) Serial.print("  ");
    else if (x<100) Serial.print(" ");
    // else if (x<1000) Serial.print(" ");
    Serial.print(num);
}
static int bcap[13];
static int pcap[13];
static int cap[13];
static int maxcap[13];
static int mincap[13];

int first = 1;

extern volatile bool dataAvailable;
void setup() {
    delay(1000);
    Serial.println("touch test main begin");
    MAIN_SETUP();
    configTouch(7, 1, 5, 2);
    delay(200);
    Serial.println("touch test start");
    Event::eventNoteOn(69, 1024);
}
void loop() {
    const int static interval = 300; // every 100 ms
    // static int count=0;
    static long ms = 0;
    pressure.getPressure();
    
    if (! dataAvailable) return;
    dataAvailable = 0;
    // startReadPins();
    // waitbusy();
    for (int i = 0; i < 13; i++) {
        pcap[i] = cap[i];
        cap[i] = readTouchPin(pinTouch[i]);
        if (first) {
            bcap[i] = cap[i];
            maxcap[i] = cap[i];
            mincap[i] = cap[i];
        }
        if (cap[i] > maxcap[i]) maxcap[i] = cap[i];
        if (cap[i] < mincap[i]) mincap[i] = cap[i];
        //cap[i] = touchRead(pinTouch[i]);
        //cap[i] = touchReadTune(pinTouch[i], 15, 1, 3, 3);
    }
    first = 0;
    long elapse = micros() - ms;
    ms = micros();
    
    static long lastprint = 0;
    if (millis() - lastprint > interval) {
        lastprint = millis();
        for (int i = 0; i < 13; i++) {
            Serial.print(pinTouch[i]);
            Serial.print("=");
            pnum(cap[i]-bcap[i]);
            pnum(cap[i]);
            pnum(pcap[i]-cap[i]);
            pnum(maxcap[i]-mincap[i]);
            Serial.print(" #");
            maxcap[i] = 0;
            mincap[i] = 999999;
        }
        Serial.print("  T=");
        Serial.print((float)elapse/1000.0);
        Serial.print(" ms");
        Serial.println();
        Serial.flush();
    }
    /*
     static long lastsw = 0;
     if (millis() - lastsw > 5000) {
     lastsw = millis();
     static int nxval;
     nxval++;
     Serial.print(" Next ");
     Serial.println(nxval);
     configTouch(7, 1, 5, nxval);
     first = 1;
     delay(1000);
     }
     */
}
#endif

#ifdef TEST_TOUCH_SIMPLE
void pnum(int num)
{
    if (num>=0) Serial.print(" ");
    else Serial.print("-");
    int x = abs(num);
    if (x<10) Serial.print("    ");
    else if (x<100) Serial.print("   ");
    else if (x<1000) Serial.print("  ");
    else if (x<10000) Serial.print(" ");
    Serial.print(num);
}

extern volatile bool dataAvailable;
int tstart;
int base[13];

void setup() {
    delay(1000);
    MAIN_SETUP();
    Serial.println("touch test start");
    tstart = millis();

    for (int i = 0; i < pinCount; i++) {
        base[i] = readTouchPin(pinTouch[i]);
    }
}

void loop() {
    int ts[13];
    // if (! dataAvailable) return;
    // dataAvailable = 0;
    for (int i = 0; i < pinCount; i++) {
        ts[i] = readTouchPin(pinTouch[i]);
    }
    for (int i = 0; i < pinCount; i++) {
        // pnum(ts[i]);
        pnum(ts[i]-base[i]);
    }
    if (millis()-tstart > 5000) {
        Serial.println();
        tstart = millis();
    }
    else Serial.print("\r");
    Midi::midiSendDebug3(13, ts);
    delay(10);
}
#endif

#ifdef TEST_TOUCH_BEND
void setup() {
    delay(1000);
    MAIN_SETUP();
    Serial.println("touch bend start");
}
void loop() {
    int ts[13];
    // if (! dataAvailable) return;
    // dataAvailable = 0;
    for (int i = 0; i < 13; i++) {
        ts[i] = readTouchPin(pinTouch[i]);
    }
    float r = (float(ts[1]) / float(ts[2]));
    Serial.print(r);
    Serial.print("        ");
    int rr = ts[1] * 1000 / ts[2];
    if (rr<800) rr=-(rr-800);
    else if (rr>1500) rr=rr-1500;
    else rr=0;
    Serial.print(rr);
    Serial.print("     ");
    Serial.print(ts[1]);
    Serial.print(" / ");
    Serial.println(ts[2]);
    delay(100);
}
#endif

#ifdef TEST_TOUCH_GRAPH
void setup() {
    configTouch();
    enableTouchPin(pinTouch[2]);
}
void loop() {
    startReadPins();
    int v = readTouchPin(pinTouch[2]);
    analogWrite(A14, vx);
    delayMicroseconds(10);
}
#endif

#ifdef TEST_TOUCH_CALIBRATE
void pnum(int num)
{
    if (num>=0) Serial.print(" ");
    int x = abs(num);
    if (x<10) Serial.print("   ");
    else if (x<100) Serial.print("  ");
    else if (x<1000) Serial.print(" ");
    Serial.print(num);
}
void sortnum(int *number, int n) {
    int a;
    for (int i = 0; i < n; ++i)
    {
        for (int j = i + 1; j < n; ++j)
        {
            if (number[i] > number[j])
            {
                a = number[i];
                number[i] = number[j];
                number[j] = a;
            }
        }
    }
}
void setup() {
    MAIN_SETUP();
    // Event::eventNoteOn(69,0);
}
void countdown() {
    for(int i=5; i>0; i--) {
        Serial.print("begin in sec ");
        Serial.println(i);
        delay(1000);
    }
    Serial.println("begin");
}
int testpin = 12;
int prev_avg[1024];
int prev_min[1024];
int prev_max[1024];

void printheader() {
    Serial.print("PIN,");
    Serial.print("MIN,");
    Serial.print("MAX,");
    Serial.print("RNG,");
    Serial.print("AVG,");
    Serial.print("PMIN,");
    Serial.print("PMAX,");
    Serial.print("PRNG,");
    Serial.print("PAVG,");
    Serial.print("MAXMINDIF,");
    Serial.print("DIF,");
    Serial.print("T,");
    Serial.print("C,");
    Serial.print("N,");
    Serial.print("P,");
    Serial.print("R,");
    Serial.print("DATA");
    Serial.println();
}

void singletest(int pdebug, int testnum) {
    const int samples = 400;
    static int cap[13][samples];
    
    for (int i = 0; i < 13; i++) {
        enableTouchPin(pinTouch[i]);
    }
    startReadPins();
    for (int i = 0; i < 13; i++) {
        readTouchPin(pinTouch[i]);
    }
    
    long ms = micros();
    
    for (int j = 0; j < samples; j++) {
        //int p = getPressure();
        //eventBreath(512+(j%50));
        startReadPins();
        for (int i = 0; i < 13; i++) {
            cap[i][j] = readTouchPin(pinTouch[i]);
            //cap[i][j] = touchRead2(pinTouch[i]);
            //cap[i][j] = touchReadTune(pinTouch[i], current, nscan, prescale, refchg);
        }
    }
    long elapse = micros() - ms;
    
    static int minv[13];
    static int maxv[13];
    static int avgv[13];
    for (int j = 0; j < samples; j++) {
        for (int i = 0; i < 13; i++) {
            if (j==0) {
                minv[i] = maxv[i] = avgv[i] = cap[i][j];
            }
            else {
                if (cap[i][j] < minv[i]) minv[i] = cap[i][j];
                if (cap[i][j] > maxv[i]) maxv[i] = cap[i][j];
                avgv[i] += cap[i][j];
            }
        }
    }
    for (int i = 0; i < 13; i++) {
        avgv[i] /= samples;
    }
    
    if (!pdebug) return;
    
    Serial.print((float)elapse/(float)samples/1000.0);
    Serial.print("ms,");
    
    for(int i=0; i < 13; i++) {
        if (i!=testpin) continue;
        Serial.print(i);
        Serial.print(",min=");
        Serial.print(minv[i]);
        Serial.print(",max=");
        Serial.print(maxv[i]);
        Serial.print(",dif=");
        Serial.print(maxv[i]-minv[i]);
        Serial.print(",avg=");
        Serial.print(avgv[i]);
        Serial.print(",");
        Serial.print(prev_min[testnum]);
        Serial.print(",");
        Serial.print(prev_max[testnum]);
        Serial.print(",");
        Serial.print(prev_max[testnum]-prev_min[testnum]);
        Serial.print(",");
        Serial.print(prev_avg[testnum]);
        Serial.print(",");
        Serial.print(minv[i] - prev_max[testnum]);
        Serial.print(",");
        Serial.print(avgv[i] - prev_avg[testnum]);
        prev_min[testnum]=minv[i];
        prev_max[testnum]=maxv[i];
        prev_avg[testnum]=avgv[i];
        //Serial.println();
    }
    
    for (int i=0; i < 13; i++) {
        if (i!=testpin) continue;
        //sortnum(cap[i], samples);
        for (int j=0; j < 20 /*samples*/; j++) {
            if (j) Serial.print(":");
            Serial.print(cap[i][j]);
        }
        Serial.println();
    }
}

void runtest(int current, int nscan, int prescale, int refchg, int pdebug, int testnum) {
    Serial.print(current);
    Serial.print(",");
    Serial.print(nscan);
    Serial.print(",");
    Serial.print(prescale);
    Serial.print(",");
    Serial.print(refchg);
    Serial.print(",");
    
    configTouch(current, nscan, prescale, refchg);
    singletest(pdebug, testnum);
}

void loop() {
    singletest(1, 0);
}

void loop2() {
    countdown();
    //for(int i=0;i<=7;i++) {
    for (int x=0;x<2;x++) {
        int tx = 0;
        Serial.print("*************");
        delay(500);
        Serial.println(x?"ON":"OFF");
        printheader();
        delay(1000);
        for(int c=0;c<=15;c++) {
            for(int n=0;n<=1;n++) {
                for(int p=0;p<=2;p++) {
                    for(int r=3;r<=3;r++) {
                        runtest(c,n,p,r,1,tx++);
                    }
                }
            }
        }
    }
    Serial.println("END");
    delay(10000);
}
#endif

#ifdef TEST_SOUND
void setup() {
    MAIN_SETUP();
    // setinstrument(2);
    Serial.println("enable headphones/speakers");
    currentOutput.setHeadphone(true);
    currentOutput.setSpeaker(true);
    //    synth.powerLineout(31);
    Serial.println("start sound test");
}
void playnote(int note, int delayms) {
    const int maxvol = 1024;
    Serial.print("play ");
    Serial.println(note);
    Event::eventNoteOn(note, 0);
    const int step = 1;
    //synth.setModulateVibrato(10000,0);
    for(int i=0;i<=maxvol;i+=step) {
        Event::eventBreath(i+random(15));
        delay(delayms);
    }
    // delay(2000);
    // Serial.println("play down");
    for(int i=maxvol;i>=0;i-=step) {
        Event::eventBreath(i+random(15));
        delay(delayms);
    }
    Event::eventNoteOff();
}
void loop() {
    static int note = 69;
    if (++note>75) note=69;
    playnote(note, 20);
    synth.runSlowTasks();
    synth.runFastTasks();
}
#endif

#ifdef TEST_SILENCE
void setup() {
    MAIN_SETUP();
    Serial.println("start silence test");
    currentOutput.setSpeaker(true);
    eventNoteOff();
}
void loop() {
    //touchRead2(pinTouch[2]);
    delay(1);
}
#endif

#ifdef TEST_SOUND_USB
#include "AudioSynthWaveformSoft.h"
AudioInputUSB iusb;
// AudioOutputUSB ousb;
AudioMixer4 omixer_R;
AudioMixer4 omixer_L;
AudioControlSGTL5000 ocodec;
AudioOutputI2S odac;
AudioSynthWaveform owav;

AudioConnection cin_R(iusb, 0, omixer_R, 0);
AudioConnection cin_L(iusb, 1, omixer_L, 0);
AudioConnection cout_R(omixer_R, 0, odac, 0);
AudioConnection cout_L(omixer_L, 1, odac, 1);
AudioConnection cwav_R(owav, 0, omixer_R, 1);

void setup() {
    AudioMemory(18);
    delay(1000);
    ocodec.enable();
    ocodec.volume(0.5);
    omixer_L.gain(0, 1);
    omixer_R.gain(0, 1);
    omixer_R.gain(1, 1);
    owav.begin(0.5, 440, WAVEFORM_SINE);
}
void loop() {}
#endif

#ifdef TEST_SOUND_LIB
#include "AudioSynthWaveformSoft.h"
AudioInputUSB iusb;
AudioOutputUSB ousb;
#define LIBSYNTHX
#ifdef LIBSYNTH
AudioSynthWaveform owav;
#else
AudioSynthWaveformSoft owav;
#endif
AudioMixer4 omixer1;

#ifdef USE_SGTL5000
AudioControlSGTL5000 ocodec;
AudioOutputI2S odac;
#else
AudioOutputAnalog odac;
#endif

#define CHX

#ifdef CH
// AudioEffectReverb ch1;
// AudioEffectDelay ch1;
AudioEffectFlange ch1;
// AudioSynthWaveform ch1;
// AudioEffectChorus ch1;
const int CHORUS_DELAY_LENGTH = 16*AUDIO_BLOCK_SAMPLES;
short ch1_delay[CHORUS_DELAY_LENGTH];
AudioConnection oc0(owav, ch1);
AudioConnection oc1(ch1, 0, omixer1, 1);
AudioConnection oc2(owav, 0, omixer1, 0);
#else
AudioConnection oc0(owav, 0, omixer1, 0);
#endif

AudioConnection oco(omixer1, odac);
AudioConnection coU0(omixer1, 0, ousb, 0);
AudioConnection coU1(omixer1, 0, ousb, 1);

void setup() {
    //MAIN_SETUP(); 
    led.begin();
    led.ldo_power(1);
    AudioMemory(18);
    delay(1000);
    Serial.println("begin lib test");
    
#ifdef USE_SGTL5000
    ocodec.enable();
    ocodec.volume(0.5);
#endif
    
#ifdef CH
    // ch1.reverbTime(1);
    // if (! ch1.begin(ch1_delay, CHORUS_DELAY_LENGTH, 3)) Serial.println("Error in effect");
    // ch1.voices(3);    
    // ch1.delay(0, 40);
    // ch1.begin(1.0, 50, WAVEFORM_SQUARE);
    ch1.begin(ch1_delay, CHORUS_DELAY_LENGTH, 10, 5, 2);
    omixer1.gain(1, 0.1);
#endif
    
    omixer1.gain(0, 0.9);
    Serial.println("play sine at 440 Hz");
#ifdef LIBSYNTH
    owav.begin(.01, 440, WAVEFORM_SINE);
#else
    float x[] = {1.0F, 0.5F, 0.3F, 0.2F};
    owav.loadHarmonics(4, x);
    owav.amplitude(.1);
    owav.frequency(440);
#endif
}
void loop() {
    static float vol = 0.1;
    delay(100);
    vol += 0.01;
    if (vol>1) vol=0;
    owav.amplitude(vol);
    // Serial.print(".");
}
#endif

#ifdef TEST_SOUND_RANGE
#include "Audio.h"
AudioSynthWaveform owav;
AudioMixer4 omixer1;
#ifdef USE_SGTL5000
AudioControlSGTL5000 ocodec;
AudioOutputI2S odac;
#endif
AudioConnection oc0(owav, 0, omixer1, 0);
AudioConnection oco(omixer1, odac);

void setup() {
    led.begin();
    led.speaker(1);
    AudioMemory(18);
    delay(1000);
    Serial.println("begin range test");
    
#ifdef USE_SGTL5000
    ocodec.enable();
    ocodec.volume(0.5);
#endif
    
    omixer1.gain(0, 0);
    Serial.println("play sine at 440 Hz");
    owav.begin(.01, 440, WAVEFORM_SINE);
}

void loop() {
    for(float v=0.0; v<0.5; v+=0.01) {
        Serial.println(v);
        omixer1.gain(0, v);     
        delay(10);   
    }
}
#endif

#ifdef TEST_PID
void testpid();
void setup() {Serial.begin(57600); delay(1000); testpid();}
void loop() {}
#endif

#ifdef TEST_PING_MIDI
void setup() {MAIN_SETUP();}
void loop() {static int count=0; usbMIDI.sendNoteOn(count % 10 + 69, 0x7F, 1); delay(1000);}
#endif

#ifdef TEST_PING_UART
void setup() {MAIN_SETUP(); delay(1000); Uart.println("ping start");}
void loop() {static int count=0; Uart.println(count++); delay(1000);}
#endif

#ifdef TEST_PING_MAIN
void setup() {MAIN_SETUP(); delay(1000); Serial.println("ping start");}
void loop() {static int count=0; Serial.println(count++); delay(1000);}
#endif

#ifdef TEST_PING
void setup() {Serial.begin(57600); delay(1000); Serial.println("ping start");}
void loop() {
    static int count=0;
    Serial.print(count++);
    Serial.println(" ping");
    delay(1000);
}
#endif

#ifdef TEST_PLUG
void setup() {
    MAIN_SETUP();
    Event::eventNoteOn(69, 1024);
}
void loop() {
    //    static int count=0;
    Serial.print("HPA ");
    Serial.print(analogRead(AUDIOPLUG_PIN));
    Serial.print("  LNA ");
    Serial.print(analogRead(LINEPLUG_PIN));
    Serial.print("  HPD ");
    Serial.print(digitalRead(AUDIOPLUG_PIN));
    Serial.print("  LND ");
    Serial.print(digitalRead(LINEPLUG_PIN));
    int vy = currentPlugState.isHeadphone();
    Serial.print("  PLUG ");
    Serial.println(vy);
    delay(1000);
    Midi::midiSendDebug(vy);
    Midi::midiSendDebug(analogRead(AUDIOPLUG_PIN));
}
#endif

#ifdef TEST_USBSTATUS
void setup() {
    Serial.begin(57600);
    MAIN_SETUP();
}
void loop() {
    delay(100);
    int x = USBDCD_STATUS;
    sayNumberInternal(x);
    blink(x);
    Serial.println(x);
    delay(1000);
}
#endif

#ifdef FINGER_TABLE
#include "Instrument.h"

extern const char *noteNames[];

void findinvalid() {
    Serial.println("missing fingerings");
    globalFingerTree.load(fingerSax);
    for (int i=0;i<=0x7FF; i++) {
        int note = globalFingerTree.getNoteTree(i);
        if (note==-1) {
            Serial.println(i | (1<<12), BIN);
        }
    }
    Serial.println("done");
}

void printnotetable() {
    //globalFingerTree.load(fingerSax);
    // Serial.println("const int notetablelen = 2048;");
    // Serial.println("const int notetable[] = {");
    Serial.println("const uint8_t baseNoteTable[notetablelen] = {");
    for (int i=0;i<=0x7FF; i++) {
        int note = globalFingerTree.getNoteTree(i);
        if (i==0) Serial.print("/* 0 */  ");
        else {
            Serial.print(",");
            if (i % 16==0) {
                Serial.println();
                delay(10);
                Serial.print("/* ");
                Serial.print(i);
                Serial.print("*/ ");
                Serial.print("  ");
            }
        }
        Serial.print(note);
    }
    Serial.println();
    Serial.println("};");
}

void printnotetabledetail() {
    //globalFingerTree.load(fingerSax);
    for (int i=0;i<=0x7FF; i++) {
        int note = globalFingerTree.getNote(i);
        Serial.print(i,BIN);
        Serial.print("\t");
        Serial.print(i);
        Serial.print("\t");
        Serial.print(note);
        Serial.print("\t");
        Serial.println(noteNames[note]);
    }
}

void setup() {
    Serial.begin(57600);
    delay(3000);
    globalFingerTree.load(fingerSax);
    printnotetable();
    printnotetabledetail();
    findinvalid();
}

void loop() {
}

#endif

#ifdef TEST_BUTTONS
void setup() {
    pinMode(SET_PIN, INPUT_PULLUP);
    pinMode(POWER2_PIN, INPUT_PULLUP);
}
void loop() {
    Serial.print("SET:");
    Serial.println(digitalRead(SET_PIN));
    Serial.print("PWR:");
    Serial.println(digitalRead(POWER2_PIN));
    delay(500);
}
#endif

#ifdef TEST_LIGHTS
void setup() {
    led.begin();
    Serial.begin(57600);
}
void loop() {
    /*
     for(int i=0;i<4;i++) {
     led.bars(i);
     delay(1000);
     }
     */
    Serial.println("sequence 0");
    led.on(1);
    delay(1000);
    led.on(0);
    delay(1000);
    Serial.println("sequence 1");
    led.usb(1);
    delay(1000);
    led.usb(0);
    delay(1000);
    Serial.println("sequence 2");
    led.midi(1);
    delay(1000);
    led.midi(0);
    delay(1000);
    Serial.println("sequence 3");
    led.headphones(1);
    delay(1000);
    led.headphones(0);
    delay(1000);
    
    /*
     for(int i=1;i<10;i++) {
     led.set(i,0);
     delay(1000);
     led.set(i,1);
     delay(1000);
     }
     */
}
#endif

#ifdef TEST

void test_button(int num) {
    int state = 0;
    startReadPins();
    int base = readTouchPin(pinTouch[num]);
    startReadPins();
    int min = 9999;
    int max = 0;
    long starttime = 0;
    long stoptime = 0;
    int timeout = millis();
    while (1) {
        int v = readTouchPin(pinTouch[num]);
        startReadPins();
        if (v > max)
            max = v;
        if (v < min)
            min = v;
        if (state == 0) { // wait for initial touch
            if (v > base + 50) {
                state = 1;
                starttime = micros();
            }
        } else if (state == 1) { // wait for release
            if (v > base + 100) { // time from level 100 to 200
                stoptime = micros();
                break;
            }
        }
        if (millis() - timeout > 5000) {
            Serial.println("Timeout");
            break;
        }
    }
    Serial.print("Max ");
    Serial.println(max);
    Serial.print("Min ");
    Serial.println(min);
    Serial.print("Base ");
    Serial.println(base);
    Serial.print("Uprate ");
    Serial.print(((float) (stoptime - starttime)) / 1000.0);
    Serial.println("  ms");
}

#endif

#ifdef TEST_TOUCH_TRIGGER
void loop() {
    for(int i=0;i<13;i++) {
        Serial.print("** Button ");
        Serial.println(i);
        test_button(i);
        delay(2);
    }
}
void setup() {
    Serial.begin(57600);
    delay(1000);
    MAIN_SETUP();
    //currentPlugState.plugUsb = false;
    //currentPlugState.headphones = false;
    //currentPlugState.setOutput();
    //currentOutput.setSpeaker(true);
    //eventNoteOn(69, 1024);
}
#endif

#ifdef TEST_MULTI

Sd2Card card;
SdVolume volume;
SdFile root;
const int chipSelect = SDCARD_CS;

void test_sd_card()
{
    Serial.print("\nInitializing SD card...");
    pinMode(chipSelect, OUTPUT);
    if (!card.init(SPI_HALF_SPEED, chipSelect)) {
        Serial.println("initialization failed. Things to check:");
        Serial.println("* is a card is inserted?");
        Serial.println("* Is your wiring correct?");
        Serial.println("* did you change the chipSelect pin to match your shield or module?");
        Serial.print("Error=");
        Serial.println(card.errorCode());
        return;
    } else {
        Serial.println("Wiring is correct and a card is present.");
    }
    
    // print the type of card
    Serial.print("\nCard type: ");
    switch(card.type()) {
        case SD_CARD_TYPE_SD1:
            Serial.println("SD1");
            break;
        case SD_CARD_TYPE_SD2:
            Serial.println("SD2");
            break;
        case SD_CARD_TYPE_SDHC:
            Serial.println("SDHC");
            break;
        default:
            Serial.println("Unknown");
    }
    
    // Now we will try to open the 'volume'/'partition' - it should be FAT16 or FAT32
    if (!volume.init(card)) {
        Serial.println("Could not find FAT16/FAT32 partition.\nMake sure you've formatted the card");
        Serial.print("Error=");
        Serial.println(card.errorCode());
        return;
    }
    
    // print the type and size of the first FAT-type volume
    uint32_t volumesize;
    Serial.print("\nVolume type is FAT");
    Serial.println(volume.fatType(), DEC);
    Serial.println();
    
    volumesize = volume.blocksPerCluster();// clusters are collections of blocks
    volumesize *= volume.clusterCount();// we'll have a lot of clusters
    volumesize *= 512;// SD card blocks are always 512 bytes
    Serial.print("Volume size (bytes): ");
    Serial.println(volumesize);
    Serial.print("Volume size (Kbytes): ");
    volumesize /= 1024;
    Serial.println(volumesize);
    Serial.print("Volume size (Mbytes): ");
    volumesize /= 1024;
    Serial.println(volumesize);
    
    Serial.println("\nFiles found on the card (name, date and size in bytes): ");
    root.openRoot(volume);
    
    // list all files in the card with date and size
    root.ls(LS_R | LS_DATE | LS_SIZE);
}

void setup() {
    Serial.begin(57600);
    delay(1000);
    MAIN_SETUP();
    
    Serial.println("Sound on A4 max vol 2 seconds");
    eventNoteOn(69,1024);
    delay(2000);
    eventNoteOff();
    Serial.println("Sound off");
    
    Serial.println("Plug in headphones");
    while(! currentPlugState.plugHeadphones) {
        delay(1);
    }
    Serial.println("Headphone detected");
    eventNoteOn(69,1024);
    delay(2000);
    eventNoteOff();
    Serial.println("Sound off");
    
    Serial.println("Unplug headphones");
    while(currentPlugState.plugHeadphones) {
        delay(1);
    }
    Serial.println("Headphone unplug detected");
    
    Serial.println("Plug in amplifier");
    while(! currentPlugState.plugHeadphones) {
        delay(1);
    }
    Serial.println("Amplifier detected");
    eventNoteOn(69,1024);
    delay(2000);
    eventNoteOff();
    Serial.println("Sound off");
    
    Serial.println("Unplug amplifier");
    while(currentPlugState.plugHeadphones) {
        delay(1);
    }
    Serial.println("Amplifier unplug detected");
    
    int pvalue = getPressure();
    Serial.println("Blow");
    while(pvalue<500) {
        Serial.println(pvalue);
        delay(100);
    }
    Serial.println("Blow OK");
    
    Serial.println("Press all buttons");
    const int bmask = 0b11111111111;
    int b = getButtonsState();
    while(b != bmask) {
        b = getButtonsState();
        Serial.print(b, BIN);
        delay(10);
    }
    Serial.println("Buttons OK");
    
    Serial.println("Insert SD card");
    
    Serial.println("Press SET button");
    while(digitalRead(SET_PIN)==0) {delay(1);}
    Serial.println("SET OK");
    
    test_sd_card();
    Serial.println("Done SD");
    
    Serial.println("Touch Button 1 for speed test");
    test_button(2);
    Serial.println("Done Touch Speed");
    
    Serial.println("All OK");
}
void loop() {}
#endif

#ifdef TEST_PRESSURE_SIMPLE
void setup() {
    MAIN_SETUP();
}
int max, min, pvalue;
void loop() {
    max = 0; min=99999;
    for (int i=0; i<1000; i++) {
        pvalue = pressure.getPressure();
        if (pvalue > max) max = pvalue;
        if (pvalue < min) min = pvalue;
    }
    Serial.print(pvalue);
    Serial.print(" min=");
    Serial.print(min);
    Serial.print(" max=");
    Serial.print(max);
    Serial.println();
    Midi::midiSendDebug(pvalue);
    //delay(100);
}
#endif

#ifdef TEST_PRESSURE
void setup() {
    Serial.begin(57600);
    delay(1000);
    Serial.println("test pressure");
    MAIN_SETUP();
    Serial.println("test pressure start");
#ifdef PRESSUREPOWER_PIN
    pinMode(PRESSUREPOWER_PIN, OUTPUT);
    digitalWrite(PRESSUREPOWER_PIN, HIGH);
#endif
    pressure.initPressure();
    pinMode(BREATH_PIN, INPUT);
    Serial.println("MAP START");
    int pressurecount = 5;
    for(int i=0;i<pressurecount;i++) {
        Serial.print(pressure.pressuremap[i]);
        Serial.print(":");
        if (i%16==0) Serial.println();
    }
    Serial.println("MAP END");
    Serial.print("base=");
    Serial.println(base_pressure);
    
    // pinMode(BREATH_PIN, INPUT_PULLDOWN);
}

void loop2() {
    int value = analogRead(BREATH_PIN);
    int pvalue = pressure.getPressure(value);
    Serial.print(value);
    Serial.print("->");
    Serial.println(pvalue);
    delay(10);
}

void loop() {
    elapsedMillis p;
    int count = 0;
    int pmin=999999;
    int pmax=0;
    int psum=0;
    while(p<100) {
        int pvalue = analogRead(BREATH_PIN);
        if (pvalue<pmin) pmin=pvalue;
        if (pvalue>pmax) pmax=pvalue;
        psum += pvalue;
        count++;
    }
    Serial.print(pmin);
    Serial.print(" ");
    Serial.print(pmax);
    Serial.print(" ");
    Serial.print(pmax-pmin);
    Serial.print(" ");
    Serial.print(psum / count);
    Serial.print(" ");
    Serial.println(count);
    delay(1000);
    Midi::midiSendDebug(psum / count);
}

#endif

#ifdef TEST_TOUCH_SPEED
void setup() {
    MAIN_SETUP();
}
void loop() {
    static int cap[13];
    static int pcap[13];
    static int ppcap[13];
    long ms = micros();
    for (int i = 0; i < 13; i++) {
        ppcap[i] = pcap[i];
        pcap[i] = cap[i];
        cap[i] = readTouchPin(pinTouch[i]);
    }
    long elapse = micros() - ms;
    
    for (int i = 0; i < 13; i++) {
        int diff1 = cap[i] - pcap[i];
        int diff2 = pcap[i] - ppcap[i];
        if (diff1 * diff2 > 2000) {
            //if (diff1>50 || diff<-50) {
            if (diff1>0) Serial.print("up ");
            else Serial.print("dn ");
            Serial.print(i);
            Serial.print(" ");
            Serial.print(diff1);
            Serial.print(" ");
            Serial.print(diff2);
            Serial.print(" ");
            Serial.print(cap[i]);
            Serial.print(" ");
            Serial.print(pcap[i]);
            Serial.print(" ");
            Serial.print(ppcap[i]);
            Serial.println();
            //}
        }
    }
}
#endif

#ifdef TEST_TOUCH_SPEED2
double maxrate[13];
double bestrate = 0;
int bestc = 0;
int bestn = 0;
int bestp = 0;
int bestr = 0;
void setup() {
    MAIN_SETUP();
    delay(2000);
}
void setupPins(int current, int nscan, int prescale, int refchg) {
    configTouch(current, nscan, prescale, refchg);
    for (int i = 0; i < 13; i++) {
        enableTouchPin(pinTouch[i]);
    }
    /*
     startReadPins();
     for (int i = 0; i < 13; i++) {
     readTouchPin(pinTouch[i]);
     }
     */
    for (int i = 0; i < 13; i++) {
        maxrate[i] = 0.0;
    }
}
void testPins(int num) {
    int cap[13];
    int pcap[13];
    for (int i = 0; i < 13; i++) {
        cap[i] = 0;
        pcap[i] = 0;
    }
    for(int p=0; p<num; p++) {
        long ms = micros();
        for (int i = 0; i < 13; i++) {
            pcap[i] = cap[i];
            cap[i] = readTouchPin(pinTouch[i]);
        }
        long elapse = micros() - ms;
        
        for (int i = 0; i < 13; i++) {
            int diff = cap[i] - pcap[i];
            double rate = ((double)diff) / (double)elapse;
            if (rate > maxrate[i]) {
                maxrate[i] = rate;
            }
        }
    }
}
void showResults() {
    static int lastprint = 0;
    if (millis() - lastprint > 2000) {
        lastprint = millis();
        for (int i = 0; i < 13; i++) {
            Serial.print(maxrate[i] * 100.0);
            Serial.print("  ");
        }
        Serial.println();
    }
}
boolean saveBest() {
    double mx = maxrate[0];
    for (int i = 1; i < 13; i++) {
        if (maxrate[i] > mx) mx = maxrate[i];
    }
    if (mx > bestrate) {
        bestrate = mx;
        return true;
    }
    return false;
}
void printInfo(int c, int n, int p, int r) {
    Serial.print("c=");
    Serial.print(c);
    Serial.print(" ");
    Serial.print("n=");
    Serial.print(n);
    Serial.print(" ");
    Serial.print("p=");
    Serial.print(p);
    Serial.print(" ");
    Serial.print("r=");
    Serial.print(r);
    Serial.println();
}
void printRate() {
    for (int i=0;i<13; i++) {
        Serial.print(maxrate[i]);
        Serial.print(" ");
    }
    Serial.println();
}
void loop() {
    for(int c=0;c<=15;c++) {
        for(int n=0;n<=1;n++) {
            for(int p=0;p<=2;p++) {
                for(int r=3;r<=3;r++) {
                    printInfo(c,n,p,r);
                    setupPins(c,n,p,r);
                    testPins(1000);
                    printRate();
                    if (saveBest()) {
                        bestc = c;
                        bestn = n;
                        bestp = p;
                        bestr = r;
                    }
                }
            }
        }
    }
    Serial.println("*************");
    Serial.print(bestrate);
    printInfo(bestc,bestn,bestp,bestr);
    Serial.println("*************");
    // testPins(1);
    // static int lastprint = 0;
    // if (millis() - lastprint > 2000) {
    //    lastprint = millis();
    //    showResults();
    // }
}
#endif

#ifdef TEST_POWER
void setup() {
    led.begin();
    led.power(1);
    Serial.begin(57600);
    power.begin(POWER2_PIN);
}
void loop() {
    int v = power.getStatus();
    Serial.println(v);
    if (v==HIGH) led.on(1);
    else led.on(0);
    delay(1000);
}
#endif

#ifdef TEST_BATTERY

int tm;
void setup() {
    MAIN_SETUP();
    Serial.begin(57600);
    tm = millis();
    led.begin();
}
void loop() {
    float v = Power::volt();
    // Serial.print(v);
    // Serial.print("\r");
    // if (millis() - tm > 1000*60*10) { // every 10 mins
    if (millis() - tm > 1000) { //  1 second
        int vv = Power::getDirectVoltage();
        tm = millis();
        Serial.print(tm / 1000 / 60);
        Serial.print(":");
        int s = tm / 1000 % 60;
        if (s<10) Serial.print("0");
        Serial.print(s);
        Serial.print("  ");
        Serial.print(v);
        Serial.print("     ");
        Serial.println(vv);
    }
    /*
     sayNumber(0);
     sayNumber((int)v);
     int d = (v*10.0F + 0.5F);
     sayNumber(d % 10);
     */
}
#endif


#ifdef TEST_SD_CARD
const int chipSelect = SDCARD_CS;

void setup()
{
    delay(1000);
#ifdef USE_SDCARD
    pinMode(SDCARD_CS, OUTPUT);
    digitalWrite(SDCARD_CS, HIGH);    
#endif
#ifdef USE_ACCELEROMETER
    pinMode(USE_ACCELEROMETER_CS, OUTPUT);
    digitalWrite(USE_ACCELEROMETER_CS, HIGH);
#endif
    // Open serial communications and wait for port to open:
    Serial.begin(115200);
    delay(1000);
    
    Serial.print("\nInitializing SD lib...");
    
    // we'll use the initialization code from the utility libraries
    // since we're just testing if the card is working!
    if (!SD.begin(chipSelect)) {
        Serial.println("initialization failed. Things to check:");
        Serial.println("* is a card is inserted?");
        Serial.println("* Is your wiring correct?");
        Serial.println("* did you change the chipSelect pin to match your shield or module?");
        return;
    } else {
        Serial.println("Wiring is correct and a card is present.");
    }
}

void loop() {}
#endif

#ifdef TEST_SD_CARD2
// set up variables using the SD utility library functions:
Sd2Card card;
SdVolume volume;
SdFile root;

// change this to match your SD shield or module;
// Arduino Ethernet shield: pin 4
// Adafruit SD shields and modules: pin 10
// Sparkfun SD shield: pin 8
// Teensy audio board: pin 10
// Wiz820+SD board: pin 4
// Teensy 2.0: pin 0
// Teensy++ 2.0: pin 20
const int chipSelect = SDCARD_CS;

void setup()
{
    // SPI.setMOSI(11);
    // SPI.setSCK(13);
#ifdef USE_SDCARD
    pinMode(SDCARD_CS, OUTPUT);
    // digitalWrite(SDCARD_CS, HIGH);    
#endif
#ifdef USE_ACCELEROMETER
    pinMode(USE_ACCELEROMETER_CS, OUTPUT);
    digitalWrite(USE_ACCELEROMETER_CS, HIGH);
#endif

    // Open serial communications and wait for port to open:
    Serial.begin(115200);
    delay(2000);
    
    Serial.print("\nInitializing SD library...");
    // On the Ethernet Shield, CS is pin 4. It's set as an output by default.
    // Note that even if it's not used as the CS pin, the hardware SS pin 
    // (10 on most Arduino boards, 53 on the Mega) must be left as an output 
    // or the SD library functions will not work. 
    //pinMode(10, OUTPUT);     // change this to 53 on a mega
    
    pinMode(chipSelect, OUTPUT);
    
    // we'll use the initialization code from the utility libraries
    // since we're just testing if the card is working!
    if (!card.init(SPI_HALF_SPEED, chipSelect)) {
        Serial.println("initialization failed. Things to check:");
        Serial.println("* is a card is inserted?");
        Serial.println("* Is your wiring correct?");
        Serial.println("* did you change the chipSelect pin to match your shield or module?");
        // Serial.print("Error=");
        // Serial.println(card.errorCode());
        return;
    } else {
        Serial.println("Wiring is correct and a card is present.");
    }
    
    // print the type of card
    Serial.print("\nCard type: ");
    switch(card.type()) {
        case SD_CARD_TYPE_SD1:
            Serial.println("SD1");
            break;
        case SD_CARD_TYPE_SD2:
            Serial.println("SD2");
            break;
        case SD_CARD_TYPE_SDHC:
            Serial.println("SDHC");
            break;
        default:
            Serial.println("Unknown");
    }
    
    // Now we will try to open the 'volume'/'partition' - it should be FAT16 or FAT32
    if (!volume.init(card)) {
        Serial.println("Could not find FAT16/FAT32 partition.\nMake sure you've formatted the card");
        // Serial.print("Error=");
        // Serial.println(card.errorCode());
        return;
    }
    
    // print the type and size of the first FAT-type volume
    uint32_t volumesize;
    Serial.print("\nVolume type is FAT");
    Serial.println(volume.fatType(), DEC);
    Serial.println();
    
    volumesize = volume.blocksPerCluster();// clusters are collections of blocks
    volumesize *= volume.clusterCount();// we'll have a lot of clusters
    volumesize *= 512;// SD card blocks are always 512 bytes
    Serial.print("Volume size (bytes): ");
    Serial.println(volumesize);
    Serial.print("Volume size (Kbytes): ");
    volumesize /= 1024;
    Serial.println(volumesize);
    Serial.print("Volume size (Mbytes): ");
    volumesize /= 1024;
    Serial.println(volumesize);
    
    Serial.println("\nFiles found on the card (name, date and size in bytes): ");
    root.openRoot(volume);
    
    // list all files in the card with date and size
    root.ls(LS_R | LS_DATE | LS_SIZE);
    
    delay(2000);
    rebootBootloader();
}

void loop() {}
#endif

#ifdef TEST_SD_SPEED
// SD chip select pin
const uint8_t chipSelect = SDCARD_CS;

#define FILE_SIZE_MB 5
#define FILE_SIZE (1000000UL*FILE_SIZE_MB)
#define BUF_SIZE 100

uint8_t buf[BUF_SIZE];

// test file
File file;

//------------------------------------------------------------------------------
// store error strings in flash to save RAM
void error(char* s) {
    Serial.println(s);
}
//------------------------------------------------------------------------------
void setup() {
    Serial.begin(57600);
    delay(2000);
}
//------------------------------------------------------------------------------
void loop() {
    uint32_t maxLatency;
    uint32_t minLatency;
    uint32_t totalLatency;
    
    if (!SD.begin(chipSelect)) error("begin failed");
    
    // open or create file - truncate existing file.
    file = SD.open("BENCH.DAT", FILE_WRITE | O_TRUNC);
    //  file = SD.open("BENCH.DAT", O_CREAT | O_TRUNC | O_CREAT);
    if (!file) {
        error("open failed");
    }
    
    // fill buf with known data
    for (uint16_t i = 0; i < (BUF_SIZE-2); i++) {
        buf[i] = 'A' + (i % 26);
    }
    buf[BUF_SIZE-2] = '\r';
    buf[BUF_SIZE-1] = '\n';
    
    Serial.print("File size ");
    Serial.print(FILE_SIZE_MB);
    Serial.println("MB");
    Serial.print("Buffer size ");
    Serial.print(BUF_SIZE);
    Serial.println(" bytes");
    Serial.println("Starting write test.  Please wait up to a minute");
    
    // do write test
    uint32_t n = FILE_SIZE/sizeof(buf);
    maxLatency = 0;
    minLatency = 999999;
    totalLatency = 0;
    uint32_t t = millis();
    for (uint32_t i = 0; i < n; i++) {
        uint32_t m = micros();
        if (file.write(buf, sizeof(buf)) != sizeof(buf)) {
            error("write failed");
        }
        m = micros() - m;
        if (maxLatency < m) maxLatency = m;
        if (minLatency > m) minLatency = m;
        totalLatency += m;
    }
    file.flush();
    t = millis() - t;
    double s = file.size();
    Serial.print("Write ");
    Serial.print(s/t);
    Serial.print(" KB/sec\n");
    Serial.print("Maximum latency: ");
    Serial.print(maxLatency);
    Serial.print(" usec, Minimum Latency: ");
    Serial.print(minLatency);
    Serial.print(" usec, Avg Latency: ");
    Serial.print(totalLatency/n);
    Serial.print(" usec\n\n");
    Serial.print("Starting read test.  Please wait up to a minute\n");
    // do read test
    file.seek(0);
    maxLatency = 0;
    minLatency = 99999;
    totalLatency = 0;
    t = millis();
    for (uint32_t i = 0; i < n; i++) {
        buf[BUF_SIZE-1] = 0;
        uint32_t m = micros();
        if (file.read(buf, sizeof(buf)) != sizeof(buf)) {
            error("read failed");
        }
        m = micros() - m;
        if (maxLatency < m) maxLatency = m;
        if (minLatency > m) minLatency = m;
        totalLatency += m;
        if (buf[BUF_SIZE-1] != '\n') {
            error("data check");
        }
    }
    t = millis() - t;
    Serial.print("Read ");
    Serial.print(s/t);
    Serial.print(" KB/sec\n");
    Serial.print("Maximum latency: ");
    Serial.print(maxLatency);;
    Serial.print(" usec, Minimum Latency: ");
    Serial.print(minLatency);;
    Serial.print(" usec, Avg Latency: ");
    Serial.print(totalLatency/n);
    Serial.print(" usec\n\n");
    Serial.print("Done\n\n");
    file.close();
    delay(5000);
}
#endif

#ifdef TEST_WDOG
void setup() {
    Serial.begin(57600);
    Serial.println("boot!");
    watchdog.begin();
}
void loop() {
    static int n=0;
    n+=10;
    watchdog.kick();
    Serial.println(n);
    delay(n);
}
#endif

#ifdef TEST_POWER_OLD
void setup() {
    led.begin();
    led.power(1);
    pinMode(POWERSWITCH_PIN, INPUT);
    Serial.begin(57600);
}
void loop() {
    int v = digitalRead(POWERSWITCH_PIN);
    //int v = analogRead(POWERSWITCH_PIN);
    Serial.println(v);
    if (v==HIGH) led.on(1);
    else led.on(0);
    delay(1000);
}
#endif

#ifdef TEST_USB_AUDIO
#include <usb_audio.h>
#define MAX_BUFFER_SIZE 16
#define LSB(n) ((n) & 255)
#define MSB(n) (((n) >> 8) & 255)

IntervalTimer PIT = IntervalTimer();
uint16_t audioBuffer[MAX_BUFFER_SIZE];
uint8_t bufferLength;
uint8_t sampleSize;
uint8_t sampleCounter = 0;
uint8_t counter = 0;
bool active=false;
void initStuff() {
    sampleCounter = 0;
    sampleSize = 2; //2 bytes per sample
    bufferLength = 16;//8 samples per buffer/packet
    uint8_t i;
    for (i=0;i<MAX_BUFFER_SIZE;i++) {
        audioBuffer[i] = 0x0000;
    }
}
void setup() {
    initStuff();
    usbAudio.begin();
}

void doAudioStuff();

void loop() {
    if (active) {
        if(!usbAudio.getAlternateSettingTX()) {
            active=false;
        } else {
            if (usbAudio.getSOF()) doAudioStuff();
        }
    } else {
        if (usbAudio.getAlternateSettingTX()) {
            active=true;
        }
    }
}

void doAudioStuff() {
    for (sampleCounter=0;sampleCounter<bufferLength;sampleCounter++) {
        if (sampleCounter<8) {
            audioBuffer[sampleCounter] = 0x7FFF;
        } else {
            audioBuffer[sampleCounter] = 0x8000;
        }
    }
    usbAudio.sendAudio(audioBuffer,bufferLength*sampleSize);
    usbAudio.clearSOF();
}
#endif

#ifdef TEST_SNOOZE
SnoozeAudio audio;
SnoozeTouch touch;
SnoozeDigital digital;
SnoozeTimer timer;
SnoozeBlock smalldelay(digital, touch, timer);

void setup() {
    MAIN_SETUP();
    sayNumber(0);
}

void loop() {
    int who;
    touch.pinMode(PIN_TOUCH_WAKE, 100); // pin, threshold
    timer.setTimer(10000);
    digital.pinMode(SET_PIN, INPUT_PULLUP, FALLING); //pin, mode, type
    delay(5000);
    for(int i=0; i<100; i++) {
        led.sleep(0);
        delay(10);
        sayNumber(9);
        who = Snooze.sleep(smalldelay);
        int x = digitalRead(SET_PIN);
        sayNumbers(who);
        sayNumbers(x);
        if (who==SET_PIN) break;
        led.sleep(1);
        delay(10);
    }
}
#endif

#ifdef TEST_WIFI_MIDI
unsigned int timex;
int nstate;
int istate;

void setup() {
    led.begin();
    Power::begin();
    Serial.begin(57600);
    Uart.begin(115200);
    timex = millis() + 5000;
    nstate = 0;
    settings.enableWifi = 1;
}

void loop() {
    if (istate==0 && millis() > 5000) {
        Serial.println("send login");
        WifiMidi::setLogin("gw2", "elephant");
        WifiMidi::sendLogin();
        istate = 1;
    }
    if(Uart.available() > 0) {
        char x = Uart.read();
        Serial.write(x);
    }
    if (millis() - timex > 2000) {
        if (nstate) {
            nstate = 0;
            Midi::midiNoteOff(69, 64, 1);
        }
        else {
            nstate = 1;
            Midi::midiNoteOn(69, 64, 1);
        }
        timex = millis();
    }
}
#endif

#ifdef TEST_DISPLAY
#include "Display.h"
void setup() {
    // MAIN_SETUP();
    delay(100);
    Wire1.begin();
    // Wire1.pinConfigure(I2C_PINS_29_30, I2C_PULLUP_EXT);
    // Wire1.setDefaultTimeout(500);
    
    Display::begin();
    // Display::state.setOctave(2);
    // Display::state.setInstrument(3);
    // Display::state.setKey(10);
    // Display::drawLogo();
}

void loop() {
    Display::drawDebug();   
    Display::displayRender();
    delay(500);
}
#endif


#ifdef TEST_ACCELEROMETER
void setup() {
    Accelerometer::begin();
}

void loop() {
    Accelerometer::simulate();
    // Accelerometer::debug();
    delay(100);
}
#endif

#ifdef TEST_VARIABLE_RESISTOR

void setup() {
    delay(1000);
    Wire.begin();
    chargeresistor.begin();
    if (chargeresistor.testConnection()) {
        Serial.println("set");
        chargeresistor.setWiper(128);
        chargeresistor.setNVWiper(26);
        
        int16_t x = chargeresistor.getWiper();
        Serial.println(x);
    }
    else {
        Serial.println("error");
    }
}

void loop() {}
#endif


#ifdef TEST_MIDI

void setup() {
    MAIN_SETUP();
}
void loop() {
    if (usbMIDI.read()) {
        int mtype = usbMIDI.getType();
        Serial.print("MIDI ");
        Serial.print(mtype);
        Serial.println();        
    }
}

#endif

#ifdef TEST_ONEWIRE

#include <OneWire.h>
#include "DS28E17.h"

class OneWireCV {
    private:
        const int i2c_addr = 0x0f;
        uint8_t sensor_addr[8];
        OneWire *w;
        DS28E17 *ds;
    public:
    bool connect() {
        w = new OneWire(UART_SENSE_PIN_TX);
        w->search(sensor_addr);
        ds = new DS28E17(w);
        ds->setAddress(sensor_addr);
        return true;
    }
    void setV(float v) {
        int dac = 65535 * v;
        uint8_t cmd[3];
        cmd[0] = 0b00011000;
        cmd[1] = dac >> 8;
        cmd[2] = dac & 0xff;
        ds->write(i2c_addr, cmd, 3);
    }
    void setF(float v) {
        int dac = 65535 * v;
        uint8_t cmd[3];
        cmd[0] = 0b00011001;
        cmd[1] = dac >> 8;
        cmd[2] = dac & 0xff;
        ds->write(i2c_addr, cmd, 3);
    }
};

// OneWireCV o;

void setup() {
    Serial.begin(57600);
    Serial.println("START");
    led.begin();
    led.wait(1);
    // o.connect();
}
void loop() {
    Serial.println("LOOP");
    // o.setV(0.5);
    delay(5000);
}

#endif