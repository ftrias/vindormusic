/*

DS28E17 1-Wire to i2C Bridge
============================

Arduino Library for Maxim Integrated DS28E17 1-Wire to i2C Bridge
Author: podija https://github.com/podija

Product: https://www.maximintegrated.com/en/products/interface/controllers-expanders/DS28E17.html

MIT License

*/
#ifndef DS28E17_h
#define DS28E17_h

#include "Arduino.h"
#include <OneWire.h>

#define ONEWIRE_TIMEOUT 50

#define DS28E17_ENABLE_SLEEP 0x1E
#define DS28E17_WRITE 0x4B
#define DS28E17_READ 0x87
#define DS28E17_MEMMORY_READ 0x2D

class DS28E17
{
  public:
    /**
     * @brief       Constructor of SoilSensor class.
     */
    DS28E17();

    /**
      * @brief       Constructor of SoilSensor class.
      */
    DS28E17(OneWire *oneWire);
    
    /**
     * @brief       Set address of DS28E17.
     * @param[in]   sensorAddress   address of DS28E17
     */
    void setAddress(uint8_t *sensorAddress); 
    
    /**
     * @brief       Wake up asleep DS28E17.
     */
    void wakeUp();
    
    /**
     * @brief       Put DS28E17 into sleep mode.
     */    
    void enableSleepMode();
    
    /**
     * @brief       Write data to I2C device connected to DS28E17.
     * @param       i2cAddress    address of required I2C device
     * @param[in]   data          data to be written
     * @param       dataLength    length of written data
     * @return      True if the write was successful, otherwise false.
     */
    bool write(uint8_t i2cAddress, uint8_t *data, uint8_t dataLength);
    
    /**
     * @brief       Write data to specified register in I2C device connected to DS28E17.
     * @param       i2cAddress    address of required I2C device
     * @param       i2cRegister   addres of required register in I2C device (may be 8 or 16 bit)
     * @param[in]   data          data to be written
     * @param       dataLength    length of written data
     * @return      True if the write was successful, otherwise false.
     */
    bool memoryWrite(uint8_t i2cAddress, uint8_t i2cRegister, uint8_t *data, uint8_t dataLength);
    
    /**
     * @brief       Read data from I2C device connected to DS28E17.
     * @param       i2cAddress    address of required I2C device
     * @param[out]  buffer        buffer for readed data
     * @param       bufferLength  required data length
     * @return      True if the read was successful, otherwise false.
     */
    bool read(uint8_t i2cAddress, uint8_t *buffer, uint8_t bufferLength);
    
    /**
     * @brief       Read data from specified register from I2C device connected to DS28E17.
     * @param       i2cAddress    address of required I2C device
     * @param       i2cRegister   addres of required register in I2C device (may be 8 or 16 bit)
     * @param[out]  buffer        buffer for readed data
     * @param       bufferLength  required data length
     * @return      True if the read was successful, otherwise false.
     */
    bool memoryRead(uint8_t i2cAddress, uint16_t i2cRegister, uint8_t *buffer, uint8_t bufferLength); 
    
  private:
    /**
     * @brief       Pointer to OneWire object.
     */
    OneWire *oneWire;
    
    /**
     * @brief       DS28E17 address.
     */
    uint8_t *address;
    
    /**
     * @brief       Common part for data write - compute CRC, send and read status.
     * @param[in]   header        header to be write
     * @param       headerLength  header length
     * @param[in]   data          data to be write
     * @param       dataLength    data length
     * @return      True if the write was successful, otherwise false.
     */
    bool _writeTo(uint8_t *header, uint8_t headerLength, uint8_t *data, uint8_t dataLength);
    
    /**
     * @brief       Common part for data write - compute CRC, send and read status.
     * @param[in]   header        header to be write
     * @param       headerLength  header length
     * @param[out]  buffer        buffer for readed data
     * @param       bufferLength  required data length
     * @return      True if the read was successful, otherwise false.
     */
    bool _readFrom(uint8_t *header, uint8_t headerLength, uint8_t *buffer, uint8_t bufferLength);       
};

#endif