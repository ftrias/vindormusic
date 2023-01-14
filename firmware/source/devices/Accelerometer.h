#ifndef ACCELEROMETER_H
#define ACCELEROMETER_H

class Accelerometer {
public:
    static float roll, pitch;
    
    /**
     * @brief Initialize accelerometer. Called at boot.
     * 
     * @return true Success
     * @return false Failed due to hardware error
     */
    static bool begin();            // initialize reading
    static int getChipID();

    /**
     * @brief Collect current accelerometer state. Called at a regular interval
     *        or as required.
     * 
     */
    static void measure();          // measure orientation (call this every 10 ms)
                                    // this function triggers all the action

    /**
     * @brief Return if Accel is in use due to pitch bend, etc.
     * 
     * @return int 1 if enabled; 0 if disabled
     */
    static int getState() { return flag_active || flag_gesture; }  // is enabled?

    /**
     * @brief Return if device screen is facing user (used to turn screen on/off)
     * 
     * @return int 1 if facing user; 0 if not
     */
    static int isFaceUp() { return (raw_z >= 0); }

    static void newNote(); // manually turn on/off
    static void updateAngles();

    static int raw_x, raw_y, raw_z, temp; // raw counts from sensor
    // static float cum_z;
    // static int prev_z;

protected:
    static bool flag_active;
    static bool flag_gesture;
    static int button_state;
    static unsigned int button_state_ms;
    static float base_pitch;
    // static float factor;

    static float floatPitch();      // calculate pitch in +/-180 degrees
    static float floatRoll();       // calculate roll in degrees
    static void processMovement();  // detect roll to trigger gesture
    static void onGesture();        // called when rolling or other gesture
    static void onButtonOn();       // executed when button is pressed
    static void onButtonOff();      // when button is released
    static void onChange();         // when orientation changes with button use

private:
    static void interrupt_isr();
    
    // these funcs are specific to the chipset
    static void measureDataRaw();
    static bool initializeDevice();    
    static int readRaw(int reg);
    static int hasNewData();
         
public:
    static void simulate();
    static void debug();
};

#endif
