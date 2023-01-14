#!/bin/sh

mkdir reference
cd reference

git clone git://git.code.sf.net/p/dfu-util/dfu-util
git clone http://github.com/libusb/libusb.git
git clone https://github.com/uChip/MCP4551.git
git clone https://github.com/lathoub/Arduino-AppleMIDI-Library.git
git clone https://github.com/adafruit/Adafruit_SSD1306.git
git clone https://github.com/adafruit/Adafruit-GFX-Library.git
git clone https://github.com/PaulStoffregen/cores.git
git clone https://github.com/PaulStoffregen/Audio.git
git clone https://github.com/PaulStoffregen/EEPROM.git
git clone https://github.com/PaulStoffregen/SPI.git
git clone https://github.com/PaulStoffregen/Wire.git
git clone https://github.com/duff2013/Snooze.git
git clone https://github.com/PaulStoffregen/SerialFlash.git
git clone https://github.com/adafruit/Adafruit_LIS3DH.git
git clone https://github.com/greiman/SdFs.git
git clone https://github.com/greiman/SdFat.git

cd ..
