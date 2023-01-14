/**
 * @file Accelerometer.cpp
 * @author Fernando Trias
 * @brief Accelerometer class and utilities
 * @version 0.1
 * @date 2020-06-02
 * 
 * @copyright Copyright (c) 2020 by Vindor Music, Inc.
 * 
 */
#include "Accelerometer.h"

#include "OutputMode.h"

#include "Event.h"
#include "Settings.h"
#include "Display.h"

extern int playingNote;

// uint16_t Accelerometer::id;

bool Accelerometer::flag_active;
bool Accelerometer::flag_gesture;
int Accelerometer::button_state;
unsigned int Accelerometer::button_state_ms;
float Accelerometer::roll;
float Accelerometer::pitch;
float Accelerometer::base_pitch;
// float Accelerometer::factor;

int Accelerometer::raw_x, Accelerometer::raw_y, Accelerometer::raw_z, Accelerometer::temp;
// int Accelerometer::prev_z = 0;
// float Accelerometer::cum_z = 0;

float memory_midiCC_accel_ccw_up = 0;
float memory_midiCC_accel_ccw_down = 0;
float memory_midiCC_accel_tilt_up = 0;
float memory_midiCC_accel_tilt_down = 0;
float memory_midiCC_accel_ccw = 0;
float memory_midiCC_accel_cw = 0;

void Accelerometer::onChange()
{
    float p = floatPitch(); // * factor;
    if (p > 0) {
        Event::eventEffect(settings.midiCC_accel_tilt_up, p, &memory_midiCC_accel_tilt_up);
    }
    else {
        Event::eventEffect(settings.midiCC_accel_tilt_down, -p, &memory_midiCC_accel_tilt_down);
    }
    float r = floatRoll();
    if (r > 0) {
        Event::eventEffect(settings.midiCC_accel_cw, r, &memory_midiCC_accel_cw);
    }
    else {
        Event::eventEffect(settings.midiCC_accel_ccw, -r, &memory_midiCC_accel_ccw);
    }
}

void Accelerometer::onButtonOff() {
    Event::eventEffect(settings.midiCC_accel_ccw_up, 0.0F, &memory_midiCC_accel_ccw_up);
    Event::eventEffect(settings.midiCC_accel_ccw_down, 0.0F, &memory_midiCC_accel_ccw_down);
    Event::eventEffect(settings.midiCC_accel_tilt_up, 0.0F, &memory_midiCC_accel_tilt_up);
    Event::eventEffect(settings.midiCC_accel_tilt_down, 0.0F, &memory_midiCC_accel_tilt_down);
    Event::eventEffect(settings.midiCC_accel_ccw, 0.0F, &memory_midiCC_accel_ccw);
    Event::eventEffect(settings.midiCC_accel_cw, 0.0F, &memory_midiCC_accel_cw);
#ifdef DEBUGBEND
    Serial.println("Accelerometer button off");
#endif
}

void Accelerometer::onButtonOn() {
    base_pitch = pitch;
#ifdef DEBUGBEND
    Serial.println("Accelerometer button on");
#endif
}

void Accelerometer::newNote() {
    if (settings.useGestures==3) {
        if (hasNewData()) measureDataRaw();
        pitch = atan2f(raw_x, sqrt(raw_y*raw_y + raw_z*raw_z)) * 180.0F/M_PI; // up is positive;
        // factor = 1.0F;
        onButtonOff();
        onButtonOn();
    }
}

void Accelerometer::measure() {
    // something happened to the button, so we debounce it
    if (button_state && (millis() > button_state_ms)) {
        if (button_state == 1) {   // is it pressed
            button_state = 0;

#ifdef USE_DISPLAY
            // menu active, so use this button as well as regular button
            // for activation
            if (menu.isActive() && isFaceUp()) {
                menu.action(99);
                return;
            }
#endif

            flag_active = true;
            if (hasNewData()) measureDataRaw();
            pitch = atan2f(raw_x, sqrt(raw_y*raw_y + raw_z*raw_z)) * 180.0F/M_PI; // up is positive;
            // factor = 1.0F;
            onButtonOn();
            processMovement();
            return;
        }
        else {  // it is released
            button_state = 0;
            flag_active = false;
            onButtonOff();
            return;
        }
    }

    if (hasNewData()) {
        measureDataRaw();
        if (playingNote > 0) {
            processMovement();
        }
    }
}

void Accelerometer::updateAngles() {
    pitch = atan2f(raw_x, sqrt(raw_y*raw_y + raw_z*raw_z)) * 180.0F/M_PI; // up is positive;
    roll = atan2f(raw_y, sqrt(raw_x*raw_x + raw_z*raw_z)) * 180.0F/M_PI; // clockwise is positive;
}

void Accelerometer::processMovement() {
    pitch = atan2f(raw_x, sqrt(raw_y*raw_y + raw_z*raw_z)) * 180.0F/M_PI; // up is positive;
    // cum_z = float(raw_z) / cosf(pitch);
    if (flag_active) {
        onChange();
    }
    else if (settings.useGestures==3) { // always on
        onChange();
    }
    else if (settings.useGestures) {
        roll = atan2f(raw_y, sqrt(raw_x*raw_x + raw_z*raw_z)) * 180.0F/M_PI; // clockwise is positive;
        onGesture();
    }
}

float Accelerometer::floatPitch() {
    const int degrees = 3;
    auto p = pitch - base_pitch;
    if (p > degrees) p -= degrees;
    else if (p < -degrees) p += degrees;
    else p = 0;
    return p / 45.0F;
}

float Accelerometer::floatRoll() {
    const int degrees = 20;
    auto p = roll;
    if (p > degrees) p -= degrees;
    else if (p < -degrees) p += degrees;
    else p = 0;
    return p / 30.0F;
}

void Accelerometer::onGesture() {
    if (! flag_gesture) {
        if (roll > 15 || roll < -15) {
            flag_gesture = true;
            // factor = 1.0F;
            onButtonOn();
        }
        else {
            return;
        }
    }
    else {
        if (roll < 10 && roll > -10) {
            flag_gesture = false;
            onButtonOff();
            return;
        }
    }

    if (flag_gesture) {
        float p = floatPitch();
        if (settings.useGestures == 2) { // complex gestures
            if (roll < 0) {
                if (roll < -20) {
                    Event::eventEffect(settings.midiCC_accel_ccw, -floatRoll(), &memory_midiCC_accel_ccw);
                }
                if (p > 0) {
                    Event::eventEffect(settings.midiCC_accel_ccw_up, p, &memory_midiCC_accel_ccw_up);
                }
                else {
                    Event::eventEffect(settings.midiCC_accel_ccw_down, -p, &memory_midiCC_accel_ccw_down);
                }
            }
            else {
                if (roll > 20) {
                    Event::eventEffect(settings.midiCC_accel_cw, floatRoll(), &memory_midiCC_accel_cw);
                }
                if (p > 0) {
                    Event::eventEffect(settings.midiCC_accel_tilt_up, p, &memory_midiCC_accel_tilt_up);
                }
                else {
                    Event::eventEffect(settings.midiCC_accel_tilt_down, -p, &memory_midiCC_accel_tilt_down);
                }
            }
        }
        else { // simple gestures
            if (p > 0) {
                Event::eventEffect(settings.midiCC_accel_tilt_up, p, &memory_midiCC_accel_tilt_up);
            }
            else {
                Event::eventEffect(settings.midiCC_accel_tilt_down, -p, &memory_midiCC_accel_tilt_down);
            }
        }
    }
}

void Accelerometer::interrupt_isr() {
#ifdef USE_ACCELEROMETER
    if (digitalRead(USE_ACCELEROMETER) == LOW) {
        button_state_ms = millis()+10;
        button_state = 1;
    }
    else {
        button_state_ms = millis()+10;
        button_state = 2;
    }
#endif
}

bool Accelerometer::begin() {    
#if USE_ACCELEROMETER_CS==38
    // by default, PTE4 is disabled, so we enable it (pin 38)
    CORE_PIN38_CONFIG = PORT_PCR_MUX(1);     
#endif

    bool result = initializeDevice();
    
    flag_active = false;
    flag_gesture = false;
    button_state = 0;
    // factor = 1.0F;

#ifdef USE_ACCELEROMETER
    pinMode(USE_ACCELEROMETER, INPUT_PULLUP);     
    attachInterrupt(USE_ACCELEROMETER, interrupt_isr, CHANGE);
#endif
    return result;
}

void Accelerometer::simulate(){
    measure();
    Serial.print("X=");
    Serial.print(raw_x);	 
    Serial.print("\tY=");
    Serial.print(raw_y);	 
    Serial.print("\tZ=");
    Serial.print(raw_z);
    Serial.println();
}


void Accelerometer::debug(){
    // read all three axis in burst to ensure all measurements correspond to same sample time
    measureDataRaw();
#ifdef USE_ACCELEROMETER
    Serial.print("B=");                
    if (digitalRead(USE_ACCELEROMETER) == HIGH) {
        Serial.print("0\t");
    }
    else {
        Serial.print("1\t");                
    }

    Serial.print("XVALUE=");
    Serial.print(raw_x);	 
    Serial.print("\tYVALUE=");
    Serial.print(raw_y);	 
    Serial.print("\tZVALUE=");
    Serial.print(raw_z);	 
    Serial.print("\tTEMPERATURE=");
    Serial.println(temp);	
#endif 
}

/***************************/

#ifdef ACC_LSM6DS3IMU

#include <MyWire.h>

#include "SparkFunLSM6DS3.h"

class LSM6DS3sub: public LSM6DS3 {
    public:
    LSM6DS3sub() : LSM6DS3( I2C_MODE, 0x6A ) { }
    int readRegister16(int reg) {
        int16_t output;
        readRegisterInt16( &output, reg );
        return output;
    }
};

LSM6DS3sub lis;

// int Accelerometer::readRaw(int reg) {
//     int8_t data[2]; // = {0,10};
//     lis.readRegister( (uint8_t*) data, reg+1 );
//     lis.readRegister( (uint8_t*) data+1, reg );
//     int16_t *p = (int16_t*)data;
//     return *p;
// }

int Accelerometer::hasNewData() {
    uint8_t status;
    lis.readRegister(&status, LSM6DS3_ACC_GYRO_STATUS_REG);
    return (status & 0b001);
}

void Accelerometer::measureDataRaw() {
    // prev_z = raw_z;
    raw_x = lis.readRegister16(LSM6DS3_ACC_GYRO_OUTY_L_XL);
    raw_y = lis.readRegister16(LSM6DS3_ACC_GYRO_OUTX_L_XL);
    raw_z = lis.readRegister16(LSM6DS3_ACC_GYRO_OUTZ_L_XL);
}

bool Accelerometer::initializeDevice() {
    // base_x = base_y = base_z = 0;    
    // divisorX = 16;
    // divisorY = 16 * 200;
    // divisorZ = 16;
    lis.begin();
    uint8_t id;
    lis.readRegister(&id, LSM6DS3_ACC_GYRO_WHO_AM_I_REG);
    if (id == 0x69) return true;
	else if (id == 0x6A) return true;
    else return false;
}

int Accelerometer::getChipID() {
    uint8_t id;
    lis.readRegister(&id, LSM6DS3_ACC_GYRO_WHO_AM_I_REG);
    return id;
}

#endif

/***************************/

#ifdef ACC_LIS2HH12

#include <SPI.h>

#define LIS2HH_REG_WHO_AM_I      0x0F
#define LIS2HH_REG_CTRL1         0x20
#define LIS2HH_REG_CTRL2         0x21
#define LIS2HH_REG_CTRL3         0x22
#define LIS2HH_REG_CTRL4         0x23
#define LIS2HH_REG_CTRL5         0x24
#define LIS2HH_REG_STATUS        0x27
#define LIS2HH_REG_OUT_X_L       0x28
#define LIS2HH_REG_OUT_X_H       0x29
#define LIS2HH_REG_OUT_Y_L       0x2A
#define LIS2HH_REG_OUT_Y_H       0x2B
#define LIS2HH_REG_OUT_Z_L       0x2C
#define LIS2HH_REG_OUT_Z_H       0x2D
#define LIS2HH_REG_FIFO_CTRL     0x2E
#define LIS2HH_REG_FIFO_SRC      0x2F

#include "SparkFunLIS3DH.h"

class LIS2HH: public LIS3DHCore
{
public:
    LIS2HH( uint8_t busType, uint8_t inputArg ) : LIS3DHCore( busType, inputArg ) {
    }
    
    status_t begin( void )
    {
        //Begin the inherited core.  This gets the physical wires connected
        status_t returnError = beginCore();       

        // 50Hz
        // BDU - burst mode, don't overwrite until read complete
        // Enable XYZ
        writeRegister(LIS2HH_REG_CTRL1,     0b00101111);

        // writeRegister(LIS2HH_REG_FIFO_CTRL, 0b01000000);
        // writeRegister(LIS2HH_REG_CTRL3,     0b10000000);

        // IF_ADD_INC - increment address after read 
        writeRegister(LIS2HH_REG_CTRL4,     0b00000100);
        
        return returnError;
    }

    int16_t readRegister16(int offset) {
        uint8_t data[2];
        digitalWrite(chipSelectPin, LOW);
        SPI.transfer(offset | 0x80);  // "auto increment" bit enabled earlier
        data[0] = SPI.transfer(0x00); // receive a byte as character
        data[1] = SPI.transfer(0x00); // receive a byte as character
        digitalWrite(chipSelectPin, HIGH);
        int16_t *p = (int16_t*)data;
        return *p;
    }

    #if 0
    int8_t readRegister8(int offset) {
        uint8_t data;
        digitalWrite(chipSelectPin, LOW);
        SPI.transfer(offset | 0x80);  //Ored with "read request" bit and "auto increment" bit
        data = SPI.transfer(0x00); // receive a byte as character
        digitalWrite(chipSelectPin, HIGH);
        return (int8_t)data; 
    }
    #endif
};

#ifdef USE_ACCELEROMETER_CS
LIS2HH lis( SPI_MODE, USE_ACCELEROMETER_CS );
#endif

int Accelerometer::readRaw(int reg) {
    int8_t data[2]; // = {0,10};
    lis.readRegister( (uint8_t*) data, reg+1 );
    lis.readRegister( (uint8_t*) data+1, reg );
    int16_t *p = (int16_t*)data;
    return *p;
}

int Accelerometer::hasNewData() {
    uint8_t status;
    lis.readRegister(&status, LIS2HH_REG_STATUS);
    return (status & 0b10000000);
}

void Accelerometer::measureDataRaw() {
    raw_x = -lis.readRegister16(LIS2HH_REG_OUT_X_L);
    raw_y = -lis.readRegister16(LIS2HH_REG_OUT_Y_L);
    raw_z = lis.readRegister16(LIS2HH_REG_OUT_Z_L);
}

bool Accelerometer::initializeDevice() {
    // base_x = base_y = base_z = 0;    
    // divisorX = 64;
    // divisorY = 64 * 200;
    // divisorZ = 64;
    status_t ret = lis.begin();
    uint16_t id = lis.readRegister16(LIS2HH_REG_WHO_AM_I);
    if (id == 65) return true;
    else return false;

    return ret == IMU_SUCCESS;
}

int Accelerometer::getChipID() {
    uint8_t id;
    lis.readRegister(&id, LIS2HH_REG_WHO_AM_I);
    return id;
}
                  
#endif

#ifdef ACC_ADXL362

#include <SPI.h>
#include <ADXL362.h>

ADXL362 xl;

void Accelerometer::measureDataRaw() {
    noInterrupts();
    xl.readXYZTData(raw_x, raw_y, raw_z, temp);
    interrupts();
}

void Accelerometer::base() {
    noInterrupts();
    xl.readXYZTData(base_x, base_y, base_z, temp);
    interrupts();
}

void Accelerometer::initializeDevice() {    
    base_x = base_y = base_z = 0;
    divisor = 4;
    noInterrupts();        
    xl.begin(USE_ACCELEROMETER_CS);           // Setup SPI protocol, issue device soft reset
    xl.beginMeasure();     // Switch ADXL362 to measure mode  
    interrupts();        
} 

#endif