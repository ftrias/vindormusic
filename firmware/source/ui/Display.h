#ifndef DISPLAY_H
#define DISPLAY_H

#include "firmware.h"

void showDebug(const char *text, const char *text2=NULL);
void showDebug(const char *text, int num);

#ifdef USE_DISPLAY

#include "Power.h"

#include <Touch.h>
extern int value[];

const int OLED_RESET = -1;

class DisplayState {
public:
    char key[4];
    char key_accidental;
    char instrument[3];
    int octave_mode;
    int moctave;
    char message[255];
    unsigned int message_end = 0;
    int debug_number;
    
public:
    DisplayState();
    static void note2text(int base, char *x, char link=0);
    
public:
    void setKey(int note) ;
    void setInstrument(int inst);
    void setOctave(int o);
    void setOctaveMode(int m);
};

class Display {
public:
    static DisplayState state;
    
    static bool displayRender(int force = 0);
    // static bool displayRenderBlock();

    static void displayDebug(const char *text, const char *text2=NULL);
    static void displayDebug(const char *text, int num);
    static void displayError(const char *text);
    static void displayNumber(int number);
    
    static void displayOff();
    static int  displayOn();
    static void displayBrightness(int v);
    static int  begin(int force = 0);
    
    static void drawText(const char *text);
    static void drawBattery(int perc) ;
    static void drawOctave();
    static void drawMessage(const char *msg, int delayms=0, int size=1);
    static void drawState(int complete=0);
    static void drawInfo();
    
    static void drawDebug(int level);
    
    static void update();
    static void updateOff();
#if 0
    static void drawVolume(int vol);
    static void drawNote(int note);
#endif
    static void drawLogo();
private:
    static void resetWire();
};

#define EMPTY_VAL -9999
#define EXIT_VAL 9999

#define MENU_MENU 1
#define MENU_FREQ 2
#define MENU_INT 3
#define MENU_CC 4
#define MENU_PERCENT 5

class DisplayMenu {
private:
    static const int menuCount = 64;
    static const int rowCount = 9;
    int menuId;
    char menu[menuCount][8];
    char notes[128];
    int ret[menuCount];
    int count;
    int current;
    int currentTop = 0;
    int menutype;
    int min, max, offset;
    int state;
    int number;
    int *var_int;
    int8_t *var_int8;
    float *var_float;
    int last;
public:
    DisplayMenu();
    int clear();
    void render(int highlight);
    int strCC(int current, char *x);
    void renderNumber();
    int chooseNumber(int start, int min, int max);
    int getSetButton();
    int getButton();
    void addNotes(const char *txt);
    int addOption(const char *opt, int id=EMPTY_VAL);
    // int choose(int itemdefault=EMPTY_VAL);
    // int choose(int *itemdefault);
    // int choose(int8_t *itemdefault);
    // void waitForClick();

    void set(unsigned int *var);
    void set(int *var);
    void set(int8_t *var);
    void set(float *var);
    void showMenu(int idefault, int initialselection = 0);
    void closeMenu();
    int action(int act);
    int select(int item);
    int getSelected();
    int getState();
    int isActive();
    void showNumber(int num);
    void showSubMenu(int menu);

    int main();
};

extern DisplayMenu menu;

#endif
#endif
