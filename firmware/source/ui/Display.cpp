#include "Display.h"
#include "Power.h"
#include "Settings.h"

#include "PlugState.h"
#include "SoundSynthesis.h"
#include "ReadFingering.h"
#include "Event.h"
#include "Pressure.h"
#include "Accelerometer.h"
#include "Lights.h"
#include "Boot.h"

#include <SPI.h>
#include "MyWire.h"

#include "Audio.h"

#ifdef USE_DISPLAY

#ifdef LIP_PRESSURE_PIN
#include "LipPressure.h"
#endif

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#if (SSD1306_LCDHEIGHT != 32)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

#include "vdebug.h"

extern int fingerlist_num;
extern int breathOnly;
extern int playingNote;

#ifdef SIMULATE
extern int simulate_on;
#endif

// Adafruit_SSD1306 display(128, 32, &Wire1, -1);
Adafruit_SSD1306 display(-1);

#if (SSD1306_LCDHEIGHT != 32)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

DisplayMenu menu;

DisplayState Display::state;

extern ReadFingering fingering;
extern const char *compile_time;
extern "C" void usb_force_reset(int configuration);
extern int flag_set_button;

bool Display::displayRender(int force) {
    bool r;
#ifdef DISPLAY_BLOCK_SEND
    r = display.display(true);
    if (! r) return false;
    if (force) {
        do {
            r = display.sendBlock();
        } while(r);
    }
#else
    r = display.display();
    if (! r) return false;
#endif
    return true;
}

#ifdef DISPLAY_BLOCK_SEND
bool Display::displayRenderBlock() {
    return display.sendBlock() >= 0;
}
#endif

/**
 * @brief Turn off the display
 * 
 */
void Display::displayOff() {
    display.ssd1306_command(SSD1306_DISPLAYOFF);
}

/**
 * @brief Turn on the display
 * 
 * @return int 
 */
int Display::displayOn() {
    return display.ssd1306_command_status(SSD1306_DISPLAYON);
}

/**
 * @brief Set the contrast
 * 
 * @param v 0 = full; 1 = none
 */
void Display::displayBrightness(int v) {
    display.ssd1306_command(SSD1306_SETCONTRAST);
    display.ssd1306_command(v);
}

/**
 * @brief In case of fatal error, attempt to reset the i2c bus
 * 
 */
void Display::resetWire() {
    uint8_t scl = 29;
    uint8_t sda = 30;

    // change pin mux to digital I/O
    pinMode(sda, INPUT);
    digitalWrite(scl,HIGH);
    pinMode(scl,OUTPUT);

    for(uint8_t count=0; digitalRead(sda) == 0 && count < 9; count++)
    {
        digitalWrite(scl,LOW);
        delayMicroseconds(5);       // 10us period == 100kHz
        digitalWrite(scl,HIGH);
        delayMicroseconds(5);
    }

    // reconfigure pins for I2C
    // Wire.pinConfigure(I2C_PINS_29_30, I2C_PULLUP_EXT);
    Wire.begin();
}

/**
 * @brief Initialize display
 * 
 * @param force Restart even if already initialized
 * @return int 0 = success
 */
int Display::begin(int force) {
    if (! force) {
        static int started = 0;
        if (started) return 0;
        started = 1;
    }

    // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C, 0);  // initialize with the I2C addr 0x3C (for the 128x32)
    displayBrightness(0);
    // init done
    display.clearDisplay();
    display.display();
    int status = displayOn();
    if (status) {
        return status;
    }
    return 0;
}

// small Vindor logo
static const unsigned char vindor_logo16_glcd_bmp[] = {
    B11010101, B00110011,
    B11010111, B01100110,
    B11011010, B11001100,
    B11011000, B10011100,
    B01011001, B10011000,
    B01001011, B00110000,
    B01001110, B01100000,
    B01001100, B11100000,
    B01101100, B11000000,
    B01101001, B10000000,
    B01100011, B00000000,
    B01100111, B00000000,
    B01100110, B00000000,
    B00101100, B00000000,
    B00111000, B00000000,
    B00111000, B00000000
};

#ifdef ES2_LOGO
static const unsigned char vindor_logo18x128_glcd_bmp[] = {
B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,
B11100011,B10000111,B00011100,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000011,B11000000,B00000000,
B11100011,B10001110,B00111000,B00000000,B00000000,B00011100,B00000000,B00000000,B00000000,B00111111,B11111000,B00001111,B10000000,B00011111,B11110000,B00000000,
B11100011,B10011100,B01110000,B00000000,B00000000,B00011100,B00000000,B00000000,B00000000,B00111111,B11111000,B00111111,B11110000,B00111000,B00111000,B00000000,
B11100011,B10111000,B11100000,B00000000,B00000000,B00011100,B00000000,B00000000,B00000000,B00111000,B00000000,B01110000,B00111000,B00111000,B00011100,B00000000,
B11100011,B11110001,B11000000,B00000000,B00000000,B00011100,B00000000,B00000000,B00000000,B00111000,B00000000,B11100000,B01110000,B00000000,B10001110,B00000000,
B11100011,B11000111,B00000000,B00000000,B00000000,B00011100,B00000000,B00000000,B00000000,B00111000,B00000000,B11100000,B00000000,B00000000,B00001110,B00000000,
B01110011,B10001110,B01001110,B01111100,B00001111,B11111100,B01111111,B10001100,B11110000,B00111000,B00000000,B01110000,B00000000,B00000000,B00001110,B00000000,
B01110011,B00011100,B11001111,B11111111,B00011111,B11111100,B11111111,B11001111,B11111100,B00111111,B11000000,B00111100,B00000000,B00000000,B00001110,B00000000,
B01110010,B01110001,B11001110,B00000111,B00111000,B00011101,B11000000,B11101110,B00001100,B00111111,B11000000,B00000111,B10000000,B00000000,B00011100,B00000000,
B01110010,B01110001,B11001110,B00000111,B00111000,B00011101,B11000000,B11101110,B00000000,B00111000,B00000000,B00000000,B11110000,B00000000,B11110000,B00000000,
B01110000,B11100001,B11001110,B00000111,B00111000,B00011101,B11000000,B11101110,B00000000,B00111000,B00000000,B00000000,B00011100,B00000001,B1100000,B00000000,
B01110001,B11000001,B11001110,B00000111,B00111000,B00011101,B11000000,B11101110,B00000000,B00111000,B00000000,B00000000,B00001110,B00000111,B00000000,B00000000,
B01110001,B11000001,B11001110,B00000111,B00111000,B00011101,B11000000,B11101110,B00000000,B00111000,B00000000,B01110000,B00001100,B00011110,B00000000,B00000000,
B00111011,B10000001,B11001110,B00000111,B00111000,B00011101,B11000000,B11101110,B00000000,B00111000,B00000000,B11100000,B00101100,B00111000,B00000000,B00000000,
B00111110,B00000001,B11001110,B00000111,B00111111,B11111100,B11111111,B11001110,B00000000,B00111111,B11111000,B01111111,B11111000,B01111111,B11111000,B00000000,
B00111100,B00000001,B11001110,B00000111,B00011111,B11111000,B01111111,B10001110,B00000000,B00111111,B11111000,B00011111,B11000000,B11111111,B11110000,B00000000,
B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000
};
#else
static const unsigned char vindor_logo18x128_glcd_bmp[] = {
B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,
B11100011,B10000111,B00011100,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,
B11100011,B10001110,B00111000,B00000000,B00000000,B00011100,B00000000,B00000000,B00000000,B00111111,B11111000,B00001111,B10000000,B00110000,B00000000,B00000000,
B11100011,B10011100,B01110000,B00000000,B00000000,B00011100,B00000000,B00000000,B00000000,B00111111,B11111000,B00111111,B11110000,B00111000,B00000000,B00000000,
B11100011,B10111000,B11100000,B00000000,B00000000,B00011100,B00000000,B00000000,B00000000,B00111000,B00000000,B01110000,B00111000,B01111000,B00000000,B00000000,
B11100011,B11110001,B11000000,B00000000,B00000000,B00011100,B00000000,B00000000,B00000000,B00111000,B00000000,B11100000,B01110000,B11111000,B00000000,B00000000,
B11100011,B11000111,B00000000,B00000000,B00000000,B00011100,B00000000,B00000000,B00000000,B00111000,B00000000,B11100000,B00000000,B00111000,B00000000,B00000000,
B01110011,B10001110,B01001110,B01111100,B00001111,B11111100,B01111111,B10001100,B11110000,B00111000,B00000000,B01110000,B00000000,B00111000,B00000000,B00000000,
B01110011,B00011100,B11001111,B11111111,B00011111,B11111100,B11111111,B11001111,B11111100,B00111111,B11000000,B00111100,B00000000,B00111000,B00000000,B00000000,
B01110010,B01110001,B11001110,B00000111,B00111000,B00011101,B11000000,B11101110,B00001100,B00111111,B11000000,B00000111,B10000000,B00111000,B00000000,B00000000,
B01110010,B01110001,B11001110,B00000111,B00111000,B00011101,B11000000,B11101110,B00000000,B00111000,B00000000,B00000000,B11110000,B00111000,B00000000,B00000000,
B01110000,B11100001,B11001110,B00000111,B00111000,B00011101,B11000000,B11101110,B00000000,B00111000,B00000000,B00000000,B00011100,B00111000,B00000000,B00000000,
B01110001,B11000001,B11001110,B00000111,B00111000,B00011101,B11000000,B11101110,B00000000,B00111000,B00000000,B00000000,B00001110,B00111000,B00000000,B00000000,
B01110001,B11000001,B11001110,B00000111,B00111000,B00011101,B11000000,B11101110,B00000000,B00111000,B00000000,B01110000,B00011100,B00111000,B00000000,B00000000,
B00111011,B10000001,B11001110,B00000111,B00111000,B00011101,B11000000,B11101110,B00000000,B00111000,B00000000,B11100000,B01111000,B00111000,B00000000,B00000000,
B00111110,B00000001,B11001110,B00000111,B00111111,B11111100,B11111111,B11001110,B00000000,B00111111,B11111000,B01111111,B11100000,B11111110,B00000000,B00000000,
B00111100,B00000001,B11001110,B00000111,B00011111,B11111000,B01111111,B10001110,B00000000,B00111111,B11111000,B00011111,B10000000,B11111110,B00000000,B00000000,
B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000
};
#endif

// SD card detected
static const unsigned char bmp_sd[] = {
    B11111111,
    B10000001,
    B10000011,
    B10000001,
    B10000010,
    B11111100
};

// SD card in use
static const unsigned char bmp_sd_on[] = {
    B11111111,
    B11111111,
    B11111111,
    B11111111,
    B11111110,
    B11111100
};

// initernal synth disabled
static const unsigned char bmp_no_synth[] = {
    B01100001,
    B00110111,
    B11011011,
    B11101101,
    B00000110,
    B00000011
};

// headphone connected
static const unsigned char bmp_headphone[] = {
    B00111000,
    B01000100,
    B10000010,
    B11000110,
    B11000110,
    B11000110
};

#ifdef USE_SIMPLE_CV
// simple CV enabled
static const unsigned char bmp_cv[] = {
    B01111001,
    B10001001,
    B10001010,
    B10000110,
    B01110010,
    B00000000
};
#endif

#ifdef USE_EXTERNAL_CV
// external CV enabled
static const unsigned char bmp_cv_ext[] = {
    B01110001,
    B10010010,
    B10010100,
    B10001100,
    B01111000,
    B00000000
};
#endif

static const unsigned char bmp_amp[] = {
    B00111000,
    B01111100,
    B10000010,
    B10111010,
    B10000010,
    B01111100
};

// USB without audio
static const unsigned char bmp_usb[] = {
    B1100010,
    B1100010,
    B1100010,
    B1100010,
    B0111100
};

// USB with audio
static const unsigned char bmp_usb_a[] = {
    B1101010,
    B1101010,
    B1100010,
    B1100010,
    B0111100
};

// wifi followed by states
static const unsigned char bmp_wifi[] = {
    B01111110,
    B10000001,
    B00111100,
    B01000010,
    B00011000
};

static const unsigned char bmp_wifi_present3[] = {
    B01111110,
    B10000001,
    B00111100,
    B01000010,
    B00011000
};

static const unsigned char bmp_wifi_present2[] = {
    B00000000,
    B00000000,
    B00111100,
    B01000010,
    B00011000
};

static const unsigned char bmp_wifi_present1[] = {
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00011000
};

// some form of Midi serial output (not USB)
static const unsigned char bmp_midi[] = {
    B11000110,
    B11101110,
    B11010110,
    B11010110,
    B00000000
};

// Flat designator for key
static const unsigned char bmp_flat[] = {
    B0100000,
    B0100000,
    B0101100,
    B0110010,
    B0100100,
    B0111000
};

// Sharp for key
static const unsigned char bmp_sharp[] = {
    B0000100,
    B0101110,
    B1110100,
    B0101110,
    B1110100,
    B0100000
};


DisplayState::DisplayState() {
    key[0] = instrument[0] = 0;
    debug_number = 0;
}

/**
 * @brief Converta note number to a string
 * 
 * @param base Midi note number 0-11
 * @param x    Buffer at least 4 chars wide
 * @param link Optional char between note and accidental
 */
void DisplayState::note2text(int base, char *x, char link) {
    if (settings.notation == NOTATION_SOLFEGE) {
        switch(base) {
            case 0: x[0] = 'D'; x[1] = 'o'; x[3] = 0; break;
            case 1: x[0] = 'R'; x[1] = 'e'; x[3] = 'b'; break;
            case 2: x[0] = 'R'; x[1] = 'e'; x[3] = 0; break;
            case 3: x[0] = 'M'; x[1] = 'i'; x[3] = 'b'; break;
            case 4: x[0] = 'M'; x[1] = 'i'; x[3] = 0; break;
            case 5: x[0] = 'F'; x[1] = 'a'; x[3] = 0; break;
            case 6: x[0] = 'S'; x[1] = 'o'; x[3] = 'b'; break;
            case 7: x[0] = 'S'; x[1] = 'o'; x[3] = 0; break;
            case 8: x[0] = 'L'; x[1] = 'a'; x[3] = 'b'; break;
            case 9: x[0] = 'L'; x[1] = 'a'; x[3] = 0; break;
            case 10: x[0] = 'S'; x[1] = 'i'; x[3] = 'b'; break;
            case 11: x[0] = 'S'; x[1] = 'i'; x[3] = 0; break;
            default:
                sprintf(x, "%d", base);
                break;
        }
        if (x[3]) x[2] = link;
        else x[2] = 0;
        x[4] = 0;
    }
    else if (settings.notation == NOTATION_GERMAN) {
        switch(base) {
            case 0: x[0] = 'C'; x[2] = 0; break;
            case 1: x[0] = 'D'; x[2] = 'b'; break;
            case 2: x[0] = 'D'; x[2] = 0; break;
            case 3: x[0] = 'E'; x[2] = 'b'; break;
            case 4: x[0] = 'E'; x[2] = 0; break;
            case 5: x[0] = 'F'; x[2] = 0; break;
            case 6: x[0] = 'G'; x[2] = 'b'; break;
            case 7: x[0] = 'G'; x[2] = 0; break;
            case 8: x[0] = 'A'; x[2] = 'b'; break;
            case 9: x[0] = 'A'; x[2] = 0; break;
            case 10: x[0] = 'H'; x[2] = 'b'; break;
            case 11: x[0] = 'H'; x[2] = 0; break;
            default:
                sprintf(x, "%d", base);
                break;
        }
        if (link) { x[1] = x[2]; x[2] = 0; }
        else x[1] = 0;
        // if (x[2]) x[1] = link;
        // else x[1] = 0;
        x[3] = 0;
    }
    else {
        switch(base) {
            case 0: x[0] = 'C'; x[2] = 0; break;
            case 1: x[0] = 'D'; x[2] = 'b'; break;
            case 2: x[0] = 'D'; x[2] = 0; break;
            case 3: x[0] = 'E'; x[2] = 'b'; break;
            case 4: x[0] = 'E'; x[2] = 0; break;
            case 5: x[0] = 'F'; x[2] = 0; break;
            case 6: x[0] = 'G'; x[2] = 'b'; break;
            case 7: x[0] = 'G'; x[2] = 0; break;
            case 8: x[0] = 'A'; x[2] = 'b'; break;
            case 9: x[0] = 'A'; x[2] = 0; break;
            case 10: x[0] = 'B'; x[2] = 'b'; break;
            case 11: x[0] = 'B'; x[2] = 0; break;
            default:
                sprintf(x, "%d", base);
                break;
        }
        if (link) { x[1] = x[2]; x[2] = 0; }
        else x[1] = 0;
        // if (x[2]) x[1] = link;
        // else x[1] = 0;
        x[3] = 0;
    }
}

/**
 * @brief Set the display key
 * 
 * @param note 
 * @return  
 */
void DisplayState::setKey(int note) {
    note2text((note + 24) % 12, key);
}

/**
 * @brief Set the display instrument number
 * 
 * @param inst 
 */
void DisplayState::setInstrument(int inst) {
    if (inst  == -1) {
        instrument[0] = 'S';
    }
    else {
        instrument[0] = '1' + inst;
    }
    instrument[1] = 0;
}

/**
 * @brief Set the display octave
 * 
 * @param o 
 */
void DisplayState::setOctave(int o) {
    moctave=o;
}

/**
 * @brief Set the octave "mode" (2 octave cs 4 octave)
 * 
 * @param m 
 */
void DisplayState::setOctaveMode(int m) {
    octave_mode = m;
}

/**
 * @brief Utility to draw text at current cursor
 * 
 * @param text String text
 */
void Display::drawText(const char *text) {
    display.setRotation(45);
    for (uint8_t i=0; i < 168; i++) {
        if (text[i] == 0) break;
        display.write(text[i]);
        if ((i > 0) && (i % 21 == 0))
            display.println();
    }
}

/**
 * @brief Draw the battery level
 * 
 * @param perc Percent (0-100)
 */
void Display::drawBattery(int perc) {
    display.drawRect(2, 114, 28, 12, WHITE);
    display.fillRect(30, 117, 3, 5, WHITE);
    display.fillRect(4, 116, 24*perc/100, 8, WHITE);
}

/**
 * @brief Draw the octave strip
 * 
 */
void Display::drawOctave() {
    display.setTextSize(1);
    int id = 3 - state.moctave;
    for(int i=0;i<7; i++) {
        int y=10*(i+1)+1;
        if (i==3) {
            display.drawLine(20,y+3,20+2,y+3,WHITE);
            display.drawLine(30,y+3,30+2,y+3,WHITE);
        }
        if (id==i) {
            display.drawRect(22,y,8,7,WHITE);
            display.fillRect(24,y+2,4,3,WHITE);
        }
        else {
            display.drawRect(22,y,8,7,WHITE);
        }
    }
    if (id < 0) {
        const int x=22;
        display.drawLine(x,8,x+3,6,WHITE);
        display.drawLine(x+3,6,x+7,8,WHITE);
        display.drawLine(22,8,22+7,8,WHITE);
    }
    if (id >= 7) {
        const int x=22;
        int y = 10*8+1;
        display.drawLine(x,y,x+3,y+2,WHITE);
        display.drawLine(x+3,y+2,x+7,y,WHITE);
        display.drawLine(22,y,22+7,y,WHITE);
    }
}

/**
 * @brief Display a message on entire screen
 * 
 * @param msg      String message
 * @param delayms  Number of milliseconds to display
 * @param size     Text size
 */
void Display::drawMessage(const char *msg, int delayms, int size) {
    display.clearDisplay();
    display.setTextColor(WHITE);    
    display.setTextSize(size);
    display.setRotation(0);
    display.setCursor(0,0);
    display.print(msg);
    displayRender(1);
    if (delayms>0) delay(delayms);
}

/**
 * @brief Draw system information on screen
 * 
 */
void Display::drawInfo() {
    display.clearDisplay();
    display.setTextColor(WHITE);    
    display.setTextSize(1);
    display.setRotation(0);
    display.setCursor(0,0);
    char msg[256];
#ifdef DEBUGTIMING
    for(int i=0;i<time_slot;i++) {
        sprintf(msg, "%s=%ld;", time_slot_name[i], time_slot_millis[i]);
        display.print(msg);
    }
#else
    sprintf(msg, "%s\nR%d F%d P%d", compile_time, REVISION, FIRMWARE_VERSION, PLATFORM_VERSION);
    display.print(msg);
    #ifdef USE_WIFI
    sprintf(msg, "\nW:%s:%s", settings.ssid, settings.pass);
    display.print(msg);
    #endif
#endif
    displayRender();
}

/**
 * @brief Display debug/diagnosic message on screen
 * 
 * @param text    String message
 * @param text2   Optional text to append
 */
void Display::displayDebug(const char *text, const char *text2) {
    // debugOutputDecimal(2);
    if (text) {
        state.message_end = millis();
        strcpy(state.message, text);
        if (text2) strcat(state.message, text2);
        drawMessage(state.message, 0);
    }
    else {
        state.message_end = 0;
    }
}

/**
 * @brief Display debug/diagnositc
 * 
 * @param text   String message
 * @param num    Number to append
 */
void Display::displayDebug(const char *text, int num) {
    if (text) {
        sprintf(state.message, "%s%d", text, num);
        state.message_end = millis() + 3000;
        drawMessage(state.message, 0);
    }
}

/**
 * @brief Display an error message
 * 
 * @param text 
 */
void Display::displayError(const char *text) {
    displayOn();
    sprintf(state.message, "Error: %s", text);
    state.message_end = millis() + 10000; // extra 10 seconds
    drawMessage(state.message, 0);
}

/**
 * @brief Set debug code to display on updates
 * 
 * @param number 
 */
void Display::displayNumber(int number) {
    state.debug_number = number;
}

/**
 * @brief Draw the current state if display is active
 * 
 * @param complete 1 = redraw completely
 */
void Display::drawState(int complete) {
    if (menu.getState()) return;

    if (state.message_end) {
        // debug messages live for 3 seconds
        if (millis() < state.message_end) {
            drawMessage(state.message, 0);
            return;
        }
        state.message_end = 0;
    }

    display.clearDisplay();

    state.setKey(settings.noteAdj);
    state.setInstrument(settings.currentInstrument);
    state.setOctave(settings.octAdj);
    state.setOctaveMode(settings.octMode);
    
    display.setTextColor(WHITE);
    
    display.setTextSize(1);
    display.setCursor(0,102);
    char n[8];
    DisplayState::note2text(9, n, 0);
    char x[12];
    int a4 = round(settings.baseFreqA4);
    sprintf(x, "%s%d", n, a4);
    drawText(x);

    if (synth.sdcard) {
        display.setCursor(2,58);
        display.drawRect(0, 56, 15, 11, WHITE);
        drawText("SD");
        // display.drawBitmap(0, 60,  bmp_sd_on, 8, 6, WHITE);
    }
    else if (synth.sdcardpresent) {
        display.setCursor(2,58);
        drawText("SD");
        // display.drawBitmap(0, 60,  bmp_sd, 8, 6, WHITE);        
    }

#ifdef USE_WIFI
    if (currentPlugState.plugUsb || midi_wifi.isConnected) {
#else
    if (currentPlugState.plugUsb) {
#endif
        if (settings.usbMode==0) {
            display.drawBitmap(0, 80,  bmp_usb_a, 8, 5, WHITE);
        }
        else {
            display.drawBitmap(0, 80,  bmp_usb, 8, 5, WHITE);
        }
        
        // display.setTextSize(1);
        display.setCursor(0,68);
        if (breathOnly) {
            drawText("XBC");
        }
        else {
            switch(settings.useVolume) {
                case 0: break;
                case 1: drawText("BC"); break;
                case 2: drawText("Vol"); break;
                case 3: drawText("Exp"); break;
                case 4: drawText("AT"); break;
                case 5: drawText("Mod"); break;
                case 99:
                default: {
                    char x[8];
                    if (settings.midiCC < 10) {
                        sprintf(x, "CC%d", settings.midiCC);
                    }
                    else {
                        sprintf(x, "C%d", settings.midiCC);
                    }
                    drawText(x);
                };
            }
        }
    }

    if (currentPlugState.plug35) {
        if (currentPlugState.hasHeadphone()) {
            display.drawBitmap(8, 80,  bmp_headphone, 8, 6, WHITE);
        }
#ifdef USE_SIMPLE_CV
        else if (currentPlugState.plug35CV) {
            display.drawBitmap(8, 80,  bmp_cv, 8, 6, WHITE);
        }
#endif
#ifdef USE_EXTERNAL_CV
        else if (currentPlugState.plug35CVext) {
            display.drawBitmap(8, 80,  bmp_cv_ext, 8, 6, WHITE);
        }
#endif
        else if (currentPlugState.hasMidi()) {
            display.drawBitmap(8, 80,  bmp_midi, 8, 5, WHITE);
        }
    }

    if (currentPlugState.plugLine) {
        display.drawBitmap(16, 80,  bmp_amp, 8, 6, WHITE);
    }

#ifdef USE_WIFI
    if (midi_wifi.isWifi) {
        display.drawBitmap(24, 80,  bmp_wifi, 8, 5, WHITE);
    }
    else if (midi_wifi.isPresent) {
        static int present_idx = 0;
        switch(present_idx++) {
            case 0: 
                display.drawBitmap(24, 80,  bmp_wifi_present1, 8, 5, WHITE);
                break;
            case 1: 
                display.drawBitmap(24, 80,  bmp_wifi_present2, 8, 5, WHITE);
                break;
            case 2: 
                display.drawBitmap(24, 80,  bmp_wifi_present3, 8, 5, WHITE);
                present_idx = 0;
                break;
        }
    }
#endif

    if (settings.synthActive == 0) {
        display.drawBitmap(24, 80,  bmp_no_synth, 8, 6, WHITE);
    }

    drawOctave();
    
    display.setTextSize(2);
    display.setCursor(0,15);
    drawText(state.instrument);
    
    display.setCursor(0,34);
    if (settings.notation == NOTATION_SOLFEGE) {
        display.setTextSize(1);
        drawText(state.key);
        if (state.key[3]) {
            if (state.key[3]=='b') {
                display.drawBitmap(10, 34,  bmp_flat, 8, 6, WHITE);
            }
            else {
                display.drawBitmap(10, 34,  bmp_sharp, 8, 6, WHITE);
            }
        }
    }
    else {
        display.setTextSize(2);
        drawText(state.key);
        if (state.key[2]) {
            if (state.key[2]=='b') {
                display.drawBitmap(10, 34,  bmp_flat, 8, 6, WHITE);
            }
            else {
                display.drawBitmap(10, 34,  bmp_sharp, 8, 6, WHITE);
            }
        }
    }
    
    if (fingerlist_num != 0) {
        const char *finger_text[] = {"sax","cnt","rec","tmp","flu","cus"};
        display.setTextSize(1);
        display.setCursor(0,50);
        // char x3[8];
        // sprintf(x3, "k%d", fingerlist_num);
        drawText(finger_text[fingerlist_num]);
    }

    if (state.debug_number) {
        char n[16];
        sprintf(n, "%d", state.debug_number);
        display.setTextSize(1);
        display.setCursor(0,90);
        drawText(n);
    }
    else  if (state.octave_mode==0) {
        display.setTextSize(1);
        display.setCursor(0,90);
        drawText("4-8v");
    }
    
    drawBattery(Power::getBatteryPercent());

    if (complete) displayRender();
}

/**
 * @brief Draw debugging information; usually activated by menu
 * 
 * @param level What info to display
 */
void Display::drawDebug(int level) {
    display.clearDisplay();
    display.setTextColor(WHITE);
    display.setTextSize(1);
    display.setCursor(0,0);
    display.setRotation(45);
    char x[2];
    int v;
    switch(level) {
        case 1:
        case 2:
        case 3:
        case 4:
            display.print("Tch");
            display.println(level);
            x[1] = 0;
            for(int i=pinCount-1; i>=0; i--) {
                x[0] = 'A'+i;
                display.print(x);
                switch(level) {
                    case 1:
                        v = readTouchPin(pinTouch[i]);
                        display.println(v - baseTouch[i]);
                        break;
                    case 2:
                        v = readTouchPin(pinTouch[i]);
                        display.println(v);
                        break;
                    case 3:
                        display.println(baseTouch[i]);
                        break;
                    case 4:
                        display.println(resetPin[i]);
                        break;
                }
            }
            break;
        case 5:
            display.println("Volt");
            display.println(analogRead(PIN_INTERNAL_REF));
            display.println(analogRead(PIN_BATTERY));
            display.println("HP");
            display.println(analogRead(AUDIOPLUG_PIN));
            display.println(digitalRead(AUDIOPLUG_PIN));
#ifdef LINEPLUG_PIN_ANALOG
            display.println(analogRead(LINEPLUG_PIN));
#else
            display.println(digitalRead(LINEPLUG_PIN));
#endif
            // digitalWrite(SWITCH_HP_MIDI_PIN, LOW);
            // pinMode(UART_SENSE_PIN_TX, INPUT_PULLUP);
            // delay(1);
            // display.println(digitalRead(UART_SENSE_PIN_TX));
            break;
        case 6:
            display.println("Chips");
            v = SoundSynthesis::getDebug(1);
            display.println(v >> 8);
            display.println(v & 0xff);
            v = Accelerometer::getChipID();
            display.println(v);
            break;
        case 7:
            display.println("Accel");
            Accelerometer::updateAngles();
            display.println(Accelerometer::raw_x);
            display.println(Accelerometer::raw_y);
            display.println(Accelerometer::raw_z);
            display.println(int(Accelerometer::pitch));
            display.println(int(Accelerometer::roll));
            // display.println(Accelerometer::prev_z);
            // display.println(Accelerometer::cum_z);
            break;
        default:
            break;
    }
    displayRender();
}

/**
 * @brief Draw screen if facing up; called periodically
 * 
 */
void Display::update() {
    if (Accelerometer::isFaceUp()) {
        displayOn();
        drawState(1);
    }
    else {
        if (! menu.isActive()) {
            displayOff();
        }
    }
}

/**
 * @brief Turn display off (when facing down)
 * 
 */
void Display::updateOff() {
    if (! Accelerometer::isFaceUp()) {
        displayOff();
    }
}

#if 0
void Display::drawVolume(int vol) {
    int o_x = 70;
    int o_y = 0;
    int h = 12;
    int w = vol / 20;
    display.fillRect(o_x, o_y, 1024/20, h, BLACK);
    display.drawRect(o_x, o_y, w, h, WHITE);
    displayRender();
}

void Display::drawNote(int note) {
    char x[4];
    
    display.fillRect(0, 0, 50, 16, BLACK);
    
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(0,0);
    
    int oct = note / 12 - 1;
    int base = note % 12;
    DisplayState::note2text(base, x);
    
    if (x[1]==0) { x[1] = '0' + oct; x[2] = 0; }
    else { x[2] = '0' + oct; x[3] = 0; }
    
    drawText(x);
}
#endif

void Display::drawLogo() {
    display.setRotation(0);
    display.drawBitmap(2, 7, vindor_logo18x128_glcd_bmp, 128, 18, 1);
}

/*******************
 * 
 * MENU 
 * 
 **********/

#include "Pressure.h"
#include "WatchDog.h"

DisplayMenu::DisplayMenu() {
    clear();
}

int DisplayMenu::clear() {
    count = 0;
    currentTop =  0;
    menutype = MENU_MENU;
    min = -999;
    max = 999;
    offset = 0;
    notes[0] = 0;
    var_int = NULL;
    var_int8 = NULL;
    var_float = NULL;
    return 0;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstringop-truncation"

/**
 * @brief Add a menu option
 * 
 * @param opt String to display
 * @param id Value to set or menu id to jump to
 * @return int Menu id
 */
int DisplayMenu::addOption(const char *opt, int id) {
    if (count > menuCount) return 0;
    strncpy(menu[count], opt, 6);
    menu[count][5] = 0;
    if (id==EMPTY_VAL) id = count;
    ret[count] = id;
    count++;
    return id;
}

#pragma GCC diagnostic pop

/**
 * @brief Append to notes in display
 * 
 * @param txt 
 */
void DisplayMenu::addNotes(const char *txt) {
    strcat(notes, txt);
}

/**
 * @brief Get a text code for a MIDI CC code
 * 
 * @param current CC code
 * @param x String to fill
 * @return int 
 */
int DisplayMenu::strCC(int current, char *x) {
    switch(current) {
        case 0:
            sprintf(x, "AT");
            return 2;
        case -1:
            sprintf(x, "PB+");
            return 1;
        case -2:
            sprintf(x, "PB-");
            return 1;
        case -3:
            sprintf(x, "None");
            return 1;
        case 1:
            strcpy(x, "MOD1");
            return 1;
        case 2:
            strcpy(x, "BC2");
            return 1;
        case 7:
            strcpy(x, "VOL7");
            return 1;
        case 10:
            strcpy(x, "BAL10");
            return 1;
        case 11:
            strcpy(x, "EXP11");
            return 1;
        case 64:
            strcpy(x, "SUS64");
            return 1;
        default:
            sprintf(x, "%d", current);
            return current < 100 ? 2 : 1;
    }
}

/**
 * @brief Display a number and allow user to modify it with up/down buttons
 * 
 */
void DisplayMenu::renderNumber() {
    display.setRotation(45);
    display.setTextSize(1);
    display.setCursor(12, 20);
    display.print("+");
    display.setCursor(12, 80);
    display.print("-");
    
    if (menutype == MENU_FREQ) {
        char x[16];
        int num = current / 10;
        int dec = current % 10;
        sprintf(x, "%d.%d", num, dec);

        display.setCursor(1, 30);
        display.print("Hz");
        display.setCursor(1, 40);
        display.print(x);

        *var_float = current / 10.0F;

        float cents = 1200.0F * log2 (*var_float / 440.0F);
        num = (cents + 0.5F);
        if (num > 0) {
            sprintf(x, "+%dc", num);
        }
        else {
            sprintf(x, "%dc", num);
        }

        display.setCursor(1, 60);
        display.print("cents");
        display.setCursor(1, 70);
        display.print(x);

        if (notes[0]) {
            display.setCursor(0, 100);
            display.print(notes);
        }

        synth.restartNote();
    }
    else if (menutype == MENU_CC) {
        char x[16];
        if (current < -3) current = -3;
        if (current > 127) current = 127;
        int sz = strCC(current, x);
        display.setTextSize(sz);
        display.setCursor(1, 40);
        display.print(x);
        if (var_int) *var_int = current;
        if (var_int8) *var_int8 = current;

        if (var_int == &settings.midiRate) {
            Midi::midiSetRate(settings.midiRate);
        }
    }
    else if (menutype == MENU_INT) {
        char x[16];
        if (current < min) current = min;
        if (current > max) current = max;
        if (current < 0) {
            sprintf(x, "%d", current);
        }
        else {
            sprintf(x, "+%d", current);
        }
        if (current < 10 && current > -10) {
            display.setTextSize(2);
        }
        else {
            display.setTextSize(1);
        }
        display.setCursor(1, 40);
        display.print(x);
        if (var_int) *var_int = current + offset;
        if (var_int8) *var_int8 = current + offset;

        if (var_int == &settings.midiRate) {
            Midi::midiSetRate(settings.midiRate);
        }
    }
    else if (menutype == MENU_PERCENT) {
        char x[16];
        if (current < 0) current = 0;
        if (current > 100) current = 100;
        sprintf(x, "%d%%", current);
        display.setTextSize(1);
        display.setCursor(1, 40);
        display.print(x);
        if (var_int) *var_int = current;
        if (var_int8) *var_int8 = current;
        if (var_float) *var_float = ((float)current)  / 100.0F;

        if (var_float == &settings.hpVolume) {
            synth.setCodecVolume(settings.hpVolume);
        }
    }
}

/**
 * @brief Render a menu and highlight a specific item
 * 
 * @param highlight 
 */
void DisplayMenu::render(int highlight) {
    display.clearDisplay();
    if (menutype != MENU_MENU) {
       renderNumber();
    }
    else {
        display.setRotation(45);
        display.setTextSize(1);
        const int p = 15;
        for(int i=0; i < rowCount; i++) {
            int row = i + currentTop;
            if (row >= count) break;
            if (i == highlight) {
                display.fillRect(0, p + i * 12+1, 32, 10, WHITE);
                display.setTextColor(BLACK);
            }
            else {
                display.setTextColor(WHITE);  
            }
            display.setCursor(0, p + i * 12 + 2);
            display.print(menu[row]);   
        }
        if (notes[0]) {
            display.setTextColor(WHITE);
            int y = p + count * 12 + 2;
            display.drawLine(0, y, 32, y, WHITE);
            display.setCursor(0, y + 4);
            display.print(notes);
        }
    }
    Display::displayRender();
}

/**
 * @brief Get state of button
 * 
 * @return int 
 */
int DisplayMenu::getSetButton() {
    if (flag_set_button) {
        flag_set_button = 0;
        if (digitalRead(SET_PIN) == HIGH) return 0;
        delay(10);
        if (digitalRead(SET_PIN) == HIGH) return 0;        
        return 1;
    }
    return 0;
}

/**
 * @brief Get state of up/down buttons
 * 
 * @return int 
 */
int DisplayMenu::getButton() {
    watchdog.kick();
    if (getSetButton()) return 9;
    int s = getButtonsState();
    int oct = (s >> 11) & 0b11;
    if (oct == last) return 0;
    delay(100); // if you touch both, there is a delay between 
                // touching one and the other, so just wait for fingers to sync
    s = getButtonsState();
    oct = (s >> 11) & 0b11;
    last = oct;
    return oct;
}

/**
 * @brief Render a specific menu
 * 
 * @param mid Menu ID to display
 */
void DisplayMenu::showSubMenu(int mid) {
    menuId = mid;
    clear();
    if (menuId == EXIT_VAL) {
        closeMenu();
        return;
    }

    int initial_selection = 0;

    if (menuId == 0) {
        addOption("Sound", 1);
        // addOption("Test", 2);
        // addOption("Octav", 2);
        // addOption("Key", 3);
        addOption("MIDI", 5);
        addOption("Calib", 6);
        addOption("UI", 7);
        addOption("Sys", 8);
#ifdef SIMULATE
        addOption("Sim", 9);
#endif
        addOption("<Exit", EXIT_VAL);
    }
    else if (menuId == 1) {
        addOption("Inst", 11);
#ifdef BASE_SOUNDS_MULTIPLE
        addOption("Basic", 14);
#endif
#ifdef USE_SD_DIR
        if (synth.sdcardpresent) {
            addOption("SD", 112);
        }
#endif
        addOption("Octav", 12);
        addOption("Key", 13);
        addOption("<Exit", EXIT_VAL);
    }
    else if (menuId == 112) {
#ifdef USE_SD_DIR
        char buf[16];
        int i = 0, j =  0;
        if (synth.loadDirList(NULL)) {
            while(synth.loadDirList(buf)) {
                if (buf[0] != '.' && buf[0] != 's') {
                    addOption(buf, i);
                    if (++j >= menuCount) break;
                }
                i++;
            }
            if (j == 0) {
                addNotes("Empty");
            }
        }
        else {
            addNotes("None");
        }
        addOption("<Exit", EXIT_VAL);
#endif
#if 0
        addOption("1", 1);
        addOption("2", 2);
        addOption("3", 3);
        addOption("4", 4);
        addOption("5", 5);
        addOption("6", 6);
        addOption("7", 7);
        addOption("8", 8);
        addOption("9", 9);
        addOption("10", 10);
        addOption("11", 11);
        addOption("12", 12);
        addOption("<Exit", EXIT_VAL);
#endif
    }
    else if (menuId == 11) {
        // menutype = MENU_INT;
        // min = 1;
        // max = 9;
        // offset = -1;
        // current = settings.currentInstrument + offset;
        char s[64];
        for(int i=0; i<9; i++) {
            if (synth.getNameForInstrument(s, i) == 0) {
                sprintf(s, "%d", i+1);
            }
            addOption(s, i);
        }
        addOption("<Exit", EXIT_VAL);
        set(&settings.currentInstrument);
    }
    else if (menuId == 12) {
        addOption("Mode", 21);
        addOption("Shift", 22);
        addOption("<Exit", EXIT_VAL);
    }
    else if (menuId == 21) {
        set(&settings.octMode);
        addOption("4-oct", 0);
        addOption("2-oct", 1);
        addOption("<Exit", EXIT_VAL);
        addNotes("Use\nboth\noctav\nkeys");
    }
    else if (menuId == 22) {
        set(&settings.octAdj);
        addOption("+3", 3);
        addOption("+2", 2);
        addOption("+1", 1);
        addOption("0", 0);
        addOption("-1", -1);
        addOption("-2", -2);
        addOption("-3", -3);
        addOption("<Exit", EXIT_VAL);
    }
    else if (menuId == 13) {
        addOption("Quick", 31);
        addOption("Trnsp", 32);
        addOption("<Exit", EXIT_VAL);
    }
    else if (menuId == 31) {
        set(&settings.noteAdj);
        char x[5];
        DisplayState::note2text(0, x, '-'); // C
        addOption(x, 0);
        DisplayState::note2text(10, x, '-'); // Bb
        addOption(x, -2);
        DisplayState::note2text(3, x, '-'); // Eb
        addOption(x, +3);
        DisplayState::note2text(9, x, '-'); // A
        addOption(x, -3);
        DisplayState::note2text(5, x, '-'); // F
        addOption(x, +5);
        DisplayState::note2text(2, x, '-'); // D
        addOption(x, +2);
        addOption("<Exit", EXIT_VAL);
    }
    else if (menuId == 32) {
        menutype = MENU_INT;
        current = settings.noteAdj;
        set(&settings.noteAdj);
    }
    else if (menuId == 14) {
        addOption("Sax", 1);
        addOption("Flute", 2);
        addOption("Clrnt", 3);
        addOption("<Exit", EXIT_VAL);
        addNotes("Base\nfor\nInst\n#1");
    }
    else if (menuId == 41) {
        set(&settings.usbMode);
        addOption("On", 0);
        addOption("Off", 1);
        addOption("<Exit", EXIT_VAL);
        addNotes("Use\nUSB\nas\nMIDI\nand\nAudio");
    }
    else if (menuId == 5) {
        addOption("Easy", 51);
        addOption("Adv", 59);
        addOption("<Exit", EXIT_VAL);
    }
    else if (menuId == 59) {
        addOption("Effct", 53);
        addOption("NoteV", 52);
        addOption("Rate", 591);
        addOption("HiRes", 592);
        addOption("Chanl", 593);
        addOption("Dupl", 594);
        addOption("Actv", 595);
        addOption("<Exit", EXIT_VAL);
    }
    else if (menuId == 51) {
        set(&settings.useVolume);
        addOption("None", 0);
        addOption("Brth", 1);
        addOption("Vol", 2);
        addOption("Expr", 3);
        addOption("A/T", 4);
        addOption("ModWh", 5);
        addOption("<Exit", EXIT_VAL);
    }
    else if (menuId == 53) {
        addOption("Air", 531);
        addOption("CwUp", 532);
        addOption("CwDwn", 533);
        addOption("CW", 533);
        addOption("CCW", 534);
        addOption("Up", 535);
        addOption("Dwn", 536);
        addOption("Lip", 537);
        addOption("<Exit", EXIT_VAL);
    }
    else if (menuId == 531) {
        menutype = MENU_CC;
        settings.useVolume = 99;
        current = settings.midiCC;
        set(&settings.midiCC);
    }
    else if (menuId == 532) {
        menutype = MENU_CC;
        current = settings.midiCC_accel_ccw_up;
        set(&settings.midiCC_accel_ccw_up);
    }
    else if (menuId == 533) {
        menutype = MENU_CC;
        current = settings.midiCC_accel_ccw_down;
        set(&settings.midiCC_accel_ccw_down);
    }
    else if (menuId == 533) {
        menutype = MENU_CC;
        current = settings.midiCC_accel_cw;
        set(&settings.midiCC_accel_cw);
    }
    else if (menuId == 534) {
        menutype = MENU_CC;
        current = settings.midiCC_accel_ccw;
        set(&settings.midiCC_accel_ccw);
    }
    else if (menuId == 535) {
        menutype = MENU_CC;
        current = settings.midiCC_accel_tilt_up;
        set(&settings.midiCC_accel_tilt_up);
    }
    else if (menuId == 536) {
        menutype = MENU_CC;
        current = settings.midiCC_accel_tilt_down;
        set(&settings.midiCC_accel_tilt_down);
    }
    else if (menuId == 537) {
        menutype = MENU_CC;
        current = settings.midiCC_lip;
        set(&settings.midiCC_lip);
    }
    else if (menuId == 591) {
        menutype = MENU_INT;
        current = settings.midiRate;
        addNotes("ms\nper\nmsg");
        set(&settings.midiRate);
    }
    else if (menuId == 592) {
        addOption("On", 1);
        addOption("Off", 0);
        addNotes("Send\n14\nbit\nCC");
        set(&settings.midiHires);
    }
    else if (menuId == 593) {
        menutype = MENU_INT;
        min = 1;
        max = 16;
        current = settings.channel;
        set(&settings.channel);
    }
    else if (menuId == 594) {
        addOption("On", 1);
        addOption("Off", 0);
        addNotes("Send\ndupl\nNote\nOff");
        set(&settings.midiNoteOffRepeat);
    }
    else if (menuId == 595) {
        addOption("On", 1);
        addOption("Off", 0);
        addNotes("Send\nActiv\nSense");
        set(&settings.midiActiveSense);
    }
    else if (menuId == 52) {
        set(&settings.midiNoteOnVol);
        addOption("120", 120);
        addOption("80", 80);
        addOption("60", 60);
        addOption("40", 40);
        addOption("20", 20);
        addOption("Air", 0);
        addOption("<Exit", EXIT_VAL);
        addNotes("Vlcty\nfor\nNote\nOn\nmsg");
    }
    else if (menuId == 6) {
        addOption("Air", 61);
        addOption("HPVol", 62);
        addOption("Tune", 63);
        addOption("Respo", 64);
        addOption("Pbend", 65);
        addOption("<Exit", EXIT_VAL);
    }
    else if (menuId == 61) {
        // menutype = MENU_PERCENT;
        // current = (int)(settings.pressurefactor * 100.0F);
        addOption("Norm", 10);
        addOption("+10", 10);
        addOption("+20", 20);
        addOption("+30", 30);
        addOption("+40", 40);
        addOption("+50", 50);
        addOption("+60", 60);
        addOption("<Exit", EXIT_VAL);
        set(&settings.pressurefactor);
    }
    else if (menuId == 62) {
        menutype = MENU_PERCENT;
        current = (int)(settings.hpVolume * 100.0F);
        set(&settings.hpVolume);
    }
    else if (menuId == 63) {
        menutype = MENU_FREQ;
        current = settings.baseFreqA4 * 10.0F;
        set(&settings.baseFreqA4);
    }
    else if (menuId == 64) {
        addOption("XFast", 0);
        addOption("Fast", 20);
        addOption("Med", 50);
        addOption("Slow", 80);
        addOption("XSlow", 100);
        addOption("<Exit", EXIT_VAL);
        addNotes("Note\ntrans\ntime");
        set(&settings.fingerChangeDelay);
    }
    else if (menuId == 65) {
        addOption("Low", 40);
        addOption("Med", 100);
        addOption("High", 200);
        addOption("<Exit", EXIT_VAL);
        addNotes("Pitch\nbend");
        set(&settings.midiBendFactor);
    }
    else if (menuId == 66) {
        addOption("Buttn", 0);
        addOption("Twist", 1);
        addOption("DTwst", 2);
        addOption("Auto", 3);
        addNotes("Trigr\nfor\nbend\nand\nvibr");
        set(&settings.useGestures);
    }
    else if (menuId == 641) {
        menutype = MENU_INT;
        min = 0;
        max = 100;
        current = settings.fingerChangeDelay;
        set(&settings.fingerChangeDelay);
    }
    else if (menuId == 8) {
        addOption("Info", 81);
        addOption("Reset", 82);
        addOption("Flash", 83);
        addOption("TmOut", 84);
        addOption("Synth", 85);
        addOption("USB", 86);
        addOption("Debug", 87);
        addOption("<Exit", EXIT_VAL);
    }
    else if (menuId == 7) {
        addOption("Jack", 78);
        addOption("Octv", 21);
        addOption("Gestr", 66);
        addOption("Notes", 74);
        addOption("Fingr", 79);
        // addOption("Menu", 71);
        addOption("Spkr", 72);
        addOption("Shrt", 73);
        addOption("<Exit", EXIT_VAL);
    }
    // else if (menu == 71) {
    //     addOption("Yes", 711);
    //     addOption("No", EXIT_VAL);
    //     addOption("<Exit", EXIT_VAL);
    //     addNotes("Erase\nall.\nFactory\nreset");
    // }
    else if (menuId == 72) {
        addOption("Yes", 1);
        addOption("No", 0);
        addOption("<Exit", EXIT_VAL);
        addNotes("Enabl\nspker\nwhen\nMIDI");
        set(&settings.enable_usb_speaker);
    }
    else if (menuId == 73) {
        addOption("Yes", 1);
        addOption("No", 0);
        addOption("<Exit", EXIT_VAL);
        addNotes("Enabl\nshort\ncuts.");
        set(&settings.enable_shortcuts);
    }
#ifdef SWITCH_HP_MIDI_PIN
    else if (menuId == 78) {
        addNotes("Auto\ndetct\nor\nspec");
        addOption("None", 786);
        addOption("Auto", 781);
        addOption("HP", 782);
        addOption("MIDI", 783);
        // support for CV
    #ifdef USE_SIMPLE_CV
        addOption("CV", 784);
    #endif
    #ifdef USE_EXTERNAL_CV
        addOption("CVe", 785);
    #endif
        addOption("<Exit", EXIT_VAL);
        if (settings.switch_hp_midi == SWITCH_NONE) initial_selection = 0;
        else if (settings.switch_hp_midi == SWITCH_AUTO) initial_selection = 1;
        else if (settings.switch_hp_midi == SWITCH_HEADPHONES) initial_selection = 2;
        else if (settings.switch_hp_midi == SWITCH_MIDI) initial_selection = 3;
        else if (settings.switch_hp_midi == SWITCH_CV) initial_selection = 4;
        else if (settings.switch_hp_midi == SWITCH_CV_EXT) initial_selection = 4;
        else initial_selection = 1;
    }
#else
    else if (menuId == 78) {
        addNotes("Head\nphone\nor\nCV");
        addOption("None", 786);
        addOption("HP", 781);
        // support for CV
    #ifdef USE_SIMPLE_CV
        addOption("CV", 784);
    #endif
        addOption("<Exit", EXIT_VAL);
        if (settings.switch_hp_midi == SWITCH_NONE) initial_selection = 0;
        else if (settings.switch_hp_midi == SWITCH_CV) initial_selection = 2;
        else initial_selection = 1;
    }
#endif
    else if (menuId == 784) {
        settings.switch_hp_midi = SWITCH_CV;
        settings.saveSettings();
        addOption("V/oct", 1);
        addOption("Hz/V", 2);
        addOption("<Exit", EXIT_VAL);
        addNotes("CV\ntype");
        set(&settings.cv_type);
    }
    else if (menuId == 79) {
        addOption("Sax", 0);
        addOption("Clrnt", 1);
        addOption("Recrd", 2);
        addOption("Trmpt", 3);
        addOption("Flute", 4);
        addOption("Custm", 5);
        addOption("<Exit", EXIT_VAL);
        set(&settings.keyLayout);
    }
    else if (menuId == 83) {
        addOption("Yes", 72);
        addOption("No", EXIT_VAL);
        addOption("<Exit", EXIT_VAL);
        addNotes("Start\nFrmwr\nUpdat");
    }
    else if (menuId == 82) {
        addOption("Yes", 711);
        addOption("No", EXIT_VAL);
        addOption("<Exit", EXIT_VAL);
        addNotes("Factory\nReset");
    }
    else if (menuId == 74) {
        addOption("CDEFG", NOTATION_LETTERS);
        addOption("Do/Re", NOTATION_SOLFEGE);
        addOption("AHCDE", NOTATION_GERMAN);
        addOption("<Exit", EXIT_VAL);
        set(&settings.notation);
    }
    else if (menuId == 85) {
        addOption("On", 1);
        addOption("Off", 0);
        addOption("<Exit", EXIT_VAL);
        addNotes("Turn\non/\noff\nbuilt\nin\nsynth");
        set(&settings.synthActive);
    }
    else if (menuId == 86) {
        addOption("Audio", 41);
        addOption("<Exit", EXIT_VAL);
        addNotes("Use\nUSB\nas\nAudio\nand\nMIDI");
    }
    else if (menuId == 87) {
        addOption("Tdif", 871);
        addOption("Tval", 872);
        addOption("Tbase", 873);
        addOption("Tstdy", 874);
        addOption("Pins", 875);
        addOption("Chips", 876);
        addOption("Accel", 877);
        addOption("<Exit", EXIT_VAL);
    }
    else if (menuId == 84) {
        addOption("Never", 0);
        addOption("10min", 10 * 60 * 1000);
        addOption("20min", 20 * 60 * 1000);
        addOption("30min", 30 * 60 * 1000);
        addOption("1hr", 60 * 60 * 1000);
        addOption("2hr", 120 * 60 * 1000);
        addOption("<Exit", EXIT_VAL);
        set(&settings.idleSleep_ms);
    }
#ifdef SIMULATE
    else if (menuId == 9) {
        set(&simulate_on);
        addOption("On", 1);
        addOption("Off", 0);
        addOption("<Exit", EXIT_VAL);
        addNotes("Sim");
    }
#endif
    showMenu(0, initial_selection);
}

/**
 * @brief Handle menu item selection
 * 
 * @param item Item number selected
 * @return int 
 */
int DisplayMenu::select(int item) {
    if (ret[item] == EXIT_VAL) {
        // Handle exit meny
        closeMenu();
        return 0;
    }
    else {
        // Handle settings and other menu selections
        int val;
        if (count == 0) {
            val = item;
        }
        else {
            val = ret[item];
        }

        // Changing settings by reference pointer
        if (var_int) {
            // integer settings
            if (*var_int != val) {
                *var_int = val;
                settings.saveSettings();
            }

            closeMenu();
            return 1;
        }
        if (var_int8) {
            // 8-bit settings
            if (*var_int8 != val) {
                *var_int8 = val;
                // special cases that require additional action
                if (var_int8 == &settings.usbMode) {
                    settings.saveSettings();
                    showMessage("USB Reset");
                    usb_force_reset(settings.usbMode);
                }
                else if (var_int8 == &settings.currentInstrument) {
                    setinstrument(*var_int8);
                    settings.saveSettings();
                }
                else if (var_int8 == &settings.useVolume) {
                    settings.setVolumeMessage(val);
                    settings.saveSettings();
                }
                else if (var_int8 == &settings.synthActive) {
                    synth.setMainVolume(-1);
                }
                else if (var_int8 == &settings.keyLayout) {
                    fingering.setKeys(*var_int8);
                    settings.saveSettings();
                }
                else if (var_int8 == &settings.enable_usb_speaker) {
                    currentPlugState.setOutput();
                    settings.saveSettings();
                }
                else {
                    settings.saveSettings();
                }
            }
            closeMenu();
            return 1;
        }
        if (var_float) {
            // float settings
            float v;

            // special cases since numbers are scaled
            if (menutype == MENU_PERCENT) {
                v = ((float)val) / 100.0F;
            }
            else {
                v = ((float)val) / 10.0F;
            }

            // special cases requiring more action
            if (var_float == &settings.hpVolume) {
                settings.saveSettings();
            }
            else if (*var_float != v) {
                *var_float = v;
                if (var_float == &settings.pressurefactor) {
                    showMessage("Calibrating...");
                    pressure.buildPressureMap();
                }
                settings.saveSettings();
            }
            closeMenu();
            return 1;
        }
        
        //
        // Now handle more complex menu options
        //
        if (menuId == 8) {
            if (val == 83) {
                closeMenu();
                showMessage("FLASH UPDATE");    
                rebootBootloader();    
                return 1;
            }
            else if (val == 81) {
                Display::drawInfo();
                menuId = EXIT_VAL;
                return 1;
            }
        }
        else if (menuId == 14) {
            setinstrument(0);
            switch (val) {
                case 2: synth.sound->midiNumber = 74; break;
                case 3: synth.sound->midiNumber = 72; break;
                default:
                    synth.sound->midiNumber = 67;
            }
            synth.resetFilter();
            settings.saveSettings();
            closeMenu();
            return 1;
        }
        else if (menuId == 82) {
            if (val == 711) {
                closeMenu();
                showMessage("FACTORY RESET");        
                settings.factorySettings();
                return 1;
            }
        }
        else if (menuId == 78) {
            if (val == 786) {
                settings.switch_hp_midi = SWITCH_NONE;
                currentPlugState.setSwitch(SWITCH_NONE);
                settings.saveSettings();
                closeMenu();
                showMessage("No headphones");
                return 1;
            }
            else if (val == 781) {
                settings.switch_hp_midi = SWITCH_AUTO;
                currentPlugState.setSwitch(SWITCH_AUTO);
                settings.saveSettings();
                closeMenu();
                showMessage("Audo sense active");
                return 1;
            }
            else if (val == 782) {
                settings.switch_hp_midi = SWITCH_HEADPHONES;
                currentPlugState.setSwitch(SWITCH_HEADPHONES);
                settings.saveSettings();
                closeMenu();
                showMessage("Headphone active");
                return 1;
            }
            else if (val == 783) {
                settings.switch_hp_midi = SWITCH_MIDI;
                currentPlugState.setSwitch(SWITCH_MIDI);
                settings.saveSettings();
                closeMenu();
                showMessage("MIDI active");
                return 1;
            }
            else if (val == 784) {
                settings.switch_hp_midi = SWITCH_CV;
                currentPlugState.setSwitch(SWITCH_CV);
                settings.saveSettings();
                closeMenu();
                showMessage("CV active");
                return 1;
            }
            else if (val == 785) {
                settings.switch_hp_midi = SWITCH_CV_EXT;
                currentPlugState.setSwitch(SWITCH_CV_EXT);
                settings.saveSettings();
                closeMenu();
                showMessage("CV active");
                return 1;
            }
        }
        else if (menuId == 87) {
            int level = val - 870;
            Display::drawDebug(level);
            return 1;
        }
#ifdef USE_SD_DIR
        else if (menuId == 112) {
            synth.setSDInst(val);
            // Display::state.setInstrument(-1);
            closeMenu();
            return 1;
        }
#endif

        showSubMenu(val);
        return 1;
    }
}

/**
 * @brief Handle button presses and movement
 * 
 * @param act 
 * @return int 
 */
int DisplayMenu::action(int act) {
    if (act == 0) {
        last = 0;
        return 0;
    }

    if (state == 0) {
        showSubMenu(0);
        return 0;
    }

    if (menuId == EXIT_VAL) {
        closeMenu();
        return 0;
    }

    if (act==99) act = 9;
    else act = getButton();

    switch(act) {
        case 0b01: current += count ? 1 : -1; break;
        case 0b10: current += count ? -1 : 1; break;
        case 9: return select(current + currentTop);
    }

    if (count > rowCount) { // more than one page?
        if (current >= rowCount) { // scroll below bottom
            if (currentTop + rowCount < count) { // any left?
                currentTop++;
            }
            current = rowCount-1;
        }
        if (current < 0) { // scroll above top
            if (currentTop > 0) {
                currentTop--;
            }
            current = 0;
        }
    }
    else if (count > 0) {
        if (current >= count) current = count-1;
        else if (current < 0) current = 0;
    }
    render(current);
    return 0;
}

/**
 * @brief Return value of selected item
 * 
 * @return int 
 */
int DisplayMenu::getSelected() {
    return ret[current + currentTop];
}

int DisplayMenu::getState() {
    return state;
}

int DisplayMenu::isActive() {
    return state != 0;
}

void DisplayMenu::set(unsigned int *var) {
    var_int = (int*)var;
}

void DisplayMenu::set(int *var) {
    var_int = var;
}

void DisplayMenu::set(int8_t *var) {
    var_int8 = var;
}

void DisplayMenu::set(float *var) {
    var_float = var;
}

/**
 * @brief Initialize menu and render it
 * 
 * @param idefault The menu to show
 * @param initialselection The item to select
 */
void DisplayMenu::showMenu(int idefault, int initialselection) {
    state = 1;
    fingering.octMask = 0;
    flag_set_button = 0;

    if (count > 0) {
        current = initialselection;

        if (var_int) idefault = *var_int;
        if (var_int8) idefault = *var_int8;
        if (var_float) idefault = *var_float * 10;

        if (idefault != EMPTY_VAL) {
            for(int i=0; i<count; i++) {
                if (ret[i] == idefault) {
                    current = i;
                    break;
                }
            }
        }
    }

    watchdog.kick();
    render(current);
    watchdog.kick();
}

/**
 * @brief Close menu and return to play mode
 * 
 */
void DisplayMenu::closeMenu() {
    state = 0;
    fingering.octMask = 0b11;
    clear();
    Display::update();
}

void showDebug(const char *text, const char *text2) {
    Display::displayDebug(text, text2);
}
void showDebug(const char *text, int num) {
    Display::displayDebug(text, num);
}

#else // USE_DISPLAY
void showDebug(const char *text, const char *text2) { }
void showDebug(const char *text, int num) { }
#endif // USE_DISPLAY