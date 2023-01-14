# Vindor Firmware Makefile
#

# don't show complete command lines
Q=@

# run 4 compiles simultaneously; this may cause problems in a clean build
# so just run make multiple times
# MAKEFLAGS += --jobs=4

# update with each public release
FIRMWARE_VERSION = 7

# Platform or major revision
PLATFORM_VERSION = 1

# Motherboard revision
# ES2 released Nov 2020
PCBREV ?= 60
# ES1
# PCBREV ?= 56

# RELEASE=1
RELEASE ?= 0

# compile test programs
# TEST=1

# run random simulation
# SIMULATE=1

# compile with test debugging output
# TESTDEBUG=1

# compile for debugger
# DEBUG=1

# compile with profiling
# GPROF=1

# CPU version to use
# 30 is 128KB chip; 32 is K20DX256 64 pin; 33 is 80 pin versino
PLATFORM_R42=32
PLATFORM_R44=32
PLATFORM_R45=32
PLATFORM_R47=32
PLATFORM_R49=33
PLATFORM_R50=33
PLATFORM_R51=33
PLATFORM_R52=33
PLATFORM_R53=32ma
PLATFORM_R56=32
PLATFORM_R57=32
PLATFORM_R58=32
PLATFORM_R60=32
PLATFORM_R60c=32

PLATFORM = $(PLATFORM_R$(PCBREV))

#
# Select proper compile parameters for the platform
#
ifeq ($(PLATFORM),30)
MCU    = mk20dx128
MCUDEF = -D__MK20DX128__ -DBUILDPLATFORM=$(PLATFORM)
else ifeq ($(PLATFORM),32)
MCU    = mk20dx256
MCUDEF = -D__MK20DX256__ -DBUILDPLATFORM=$(PLATFORM)
else ifeq ($(PLATFORM),33)
MCU    = mk20dx256
MCUDEF = -D__MK20DX256__ -D__MK20DX256VLL10__ -DBUILDPLATFORM=$(PLATFORM)
else
$(error Invalid platform)
endif

BOOT = dfu

ZIPNAME = vindor-firmware

ifeq ($(wildcard /mnt/c),/mnt/c)
WINDOWS=1
else
WINDOWS=0
endif

# if we are running Windows Ubuntu shell, need to add .EXE to commands
ifeq (1,$(WINDOWS))
EXE=.exe
else
EXE=
endif


# The name of your project (used to name the compiled .hex file)
TARGET     = $(notdir $(CURDIR))
# EXE = .exe

CPUSPEED   = 96000000

ifeq ($(TEST),1)
# USB=-DVINDOR_USB_AUDIO_MIDI_SEREMU -DTEST
USB = -DVINDOR_USB_AUDIO_MIDI_SERIAL -DTEST -DUSB_ALTERNATE_PRODUCT_ID
else ifeq ($(TESTDEBUG),1)
# USB=-DUSB_MIDI_AUDIO_SERIAL -DTESTDEBUG -DUSB_ALTERNATE_PRODUCT_ID
USB = -DVINDOR_USB_AUDIO_MIDI_SERIAL -DTESTDEBUG -DUSB_ALTERNATE_PRODUCT_ID
else
# USB=-DVINDOR_USB_AUDIO_MIDI
USB = -DVINDOR_USB_AUDIO_MIDI -DUSB_ALTERNATE_PRODUCT_ID
endif

# configurable options
OPTIONS  = -DF_CPU=$(CPUSPEED) $(USB) -DLAYOUT_US_ENGLISH

# options needed by many Arduino libraries to configure for Teensy 3.x
OPTIONS += $(MCUDEF) -DARDUINO=10805 -DTEENSYDUINO=144 

# Vindor-specific codes for Teensy core
OPTIONS += -DI2S_ALT_CHANNEL -DCORE_XTRA_PINS

# use faster read-only SD card interface
# OPTIONS += -DUSE_TEENSY3_OPTIMIZED_CODE

# for buggy boards where VBAT is not connected to power
OPTIONS += -DSKIP_RTC_OSC
# no need for startup delay, so skip it
OPTIONS += -DSKIP_STARTUP_DELAY

ifdef PCBREV
OPTIONS += -DPCBREV=$(PCBREV) -DPCB_R1_$(PCBREV)
endif

OPTIONS += -DFIRMWARE_VERSION=$(FIRMWARE_VERSION) -DPLATFORM_VERSION=$(PLATFORM_VERSION)

ifeq ($(SIMULATE),1)
OPTIONS += -DSIMULATE
endif

BUILDDIR = build
# SOURCE = $(shell pwd)
SOURCE = .

# for Windows under Cygwin; obsolete. Recommend use Windows Ubuntu shell
ifdef COMSPEC
cygpath-win    = $(shell cygpath -w "$1")
else
cygpath-win    = $1
endif

EXTERNAL_ROOT = $(realpath ../external)

# path location for Teensy 3 core
CORE_PARENT = $(EXTERNAL_ROOT)/modules/cores
CORE_PATH   = $(CORE_PARENT)/teensy3

# path location for Arduino libraries
USER_LIBRARY_PATH   = $(EXTERNAL_ROOT)/modules
CUSTOM_LIBRARY_PATH = $(EXTERNAL_ROOT)/modules

# SDLIB = SD SD/utility
# SDLIB = SdFs/src SdFs/src/ExFatLib SdFs/src/FatLib SdFs/src/SdCard SdFs/src/SpiDriver SdFs/src/common SdFs/src/iostream
SDLIB = SdFat/src SdFat/src/ExFatLib SdFat/src/FatLib SdFat/src/SdCard SdFat/src/SpiDriver SdFat/src/common SdFat/src/iostream SdFat/src/FsLib

ifeq ($(PLATFORM),30)
USER_LIBS   = SPI EEPROM i2c_t3
CUSTOM_LIBS = Wire Audio Audio/utility
OPTIONS    += -DSMALL_FOOTPRINT
else
USER_LIBS   = SPI EEPROM SerialFlash Adafruit-GFX-Library OneWire arduino-DS28E17
CUSTOM_LIBS = Wire $(SDLIB) Audio Audio/utility MCP4551 Adafruit_SSD1306
OPTIONS    += -DUSE_SDCARD
endif

# Location of GCC. Download latest from: https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads
COMPILER_PATH ?= /Applications/ARM/bin
# COMPILER_PATH ?= ~/gcc-arm-none-eabi-10.3-2021.10/bin
# COMPILER_PATH ?= /usr/bin

COMPILER_PREFIX = arm-none-eabi

# path location for the arm-none-eabi compiler
ifeq ($(wildcard $(COMPILER_PATH)/$(COMPILER_PREFIX)-gcc),)
$(error COMPILER_PATH is not set correctly. Please update Makefile or set environment variable.)
endif

ifneq ($(wildcard $(EXTERNAL_ROOT)/dfu-util/src/dfu-util),)
DFU_DIR=$(EXTERNAL_ROOT)/dfu-util/src/
DFU_SUFFIX = $(DFU_DIR)/dfu-suffix$(EXE)
DFU_UTIL   = sudo $(DFU_DIR)/dfu-util$(EXE)
else
DFU_SUFFIX = dfu-suffix$(EXE)
DFU_UTIL   = sudo dfu-util$(EXE)
endif

MIDI_UTIL  = ../cmdline/midicmd$(EXE)

#************************************************************************
# Settings below this point usually do not need to be edited
#************************************************************************

ifeq ($(DEBUG),1)
DFLAGS = -g -O3 -DVINDOR_DEBUG
else
DFLAGS = -g -O3
endif

ifeq ($(RELEASE), 1)
DFLAGS = -O3 -fomit-frame-pointer -DVINDOR_RELEASE
endif

SOURCE_DIRS = . sound devices ui comm sys

# CPPFLAGS = compiler options for C and C++
CPPFLAGS := -Wall -MMD -mcpu=cortex-m4 -mthumb -fsingle-precision-constant -ffast-math -nostdlib -ffunction-sections -fdata-sections

# for assembly files
SFLAGS    = $(CPPFLAGS) -O0

CPPFLAGS += $(DFLAGS) $(OPTIONS) -I$(SOURCE) -I$(CORE_PATH)
CPPFLAGS += $(foreach dir,$(SOURCE_DIRS),-I$(SOURCE)/$(dir)) 

# compiler options for C++ only
CXXFLAGS := -std=gnu++14 -felide-constructors -fno-exceptions -fno-rtti -fno-threadsafe-statics

# compiler options for C only
CFLAGS   :=

ifeq ($(GPROF),1)
CXXFLAGS += -pg -DGPROF_SKIP_SD -DGPROF -DHISTFRACTION=32 -DHASHFRACTION=64 -DARCDENSITY=5 -DARCDENSITYDIVSOR=1000
CUSTOM_LIBS += TeensyGProf/src
endif

LDSCRIPT = sys/$(MCU)$(BOOT).ld

# linker options
LDFLAGS = -dead_strip -Wl,--gc-sections,--defsym=__rtc_localtime=0 --specs=nano.specs -mcpu=cortex-m4 -mthumb -T$(LDSCRIPT) -Xlinker -Map=firmware.map

# additional libraries to link
LIBS    = -lm -L$(EXTERNAL_ROOT)/modules/CMSIS -larm_cortexM4l_math
# LIBS = -lm

# names for the compiler programs
CC      = $(COMPILER_PATH)/$(COMPILER_PREFIX)-gcc$(EXE)
CXX     = $(COMPILER_PATH)/$(COMPILER_PREFIX)-g++$(EXE)
OBJCOPY = $(COMPILER_PATH)/$(COMPILER_PREFIX)-objcopy$(EXE)
SIZE    = $(COMPILER_PATH)/$(COMPILER_PREFIX)-size$(EXE)
OBJDUMP = $(COMPILER_PATH)/$(COMPILER_PREFIX)-objdump$(EXE)
NM      = $(COMPILER_PATH)/$(COMPILER_PREFIX)-nm$(EXE)

# GPROFX=$(COMPILER_PATH)/arm-none-eabi-gprof$(EXE)
GPROFX =arm-none-eabi-gprof$(EXE)

# L_INC := -I$(SOURCE) -I$(CORE_PATH)
L_INC := $(foreach lib,$(USER_LIBS), -I$(USER_LIBRARY_PATH)/$(lib))
L_INC += $(foreach lib,$(CUSTOM_LIBS), -I$(CUSTOM_LIBRARY_PATH)/$(lib))

GET_LIB = $(foreach lib,$(2),$(filter %.c %.cpp %.S %.ino, $(wildcard $(1)/$(lib)/*)))

SOURCE_FILES := $(foreach dir,$(SOURCE_DIRS),$(wildcard $(SOURCE)/$(dir)/*))
LC_FILES := $(filter %.c %.cpp %.S, $(SOURCE_FILES))
LC_FILES += $(call GET_LIB, $(CORE_PARENT), teensy3)
LC_FILES += $(call GET_LIB, $(USER_LIBRARY_PATH), $(USER_LIBS))
LC_FILES += $(call GET_LIB, $(CUSTOM_LIBRARY_PATH), $(CUSTOM_LIBS))

SKIP_FILES = memcpy-armv7m.S
SKIP_FILTER := $(foreach f,$(SKIP_FILES),%/$(f))
LC_FILES := $(filter-out $(SKIP_FILTER),$(LC_FILES))

OBJS_FILES := \
$(patsubst %.ino, %.o, $(filter %.ino, $(LC_FILES))) \
$(patsubst %.c, %.o, $(filter %.c, $(LC_FILES))) \
$(patsubst %.cpp, %.o, $(filter %.cpp, $(LC_FILES))) \
$(patsubst %.S, %.o, $(filter %.S, $(LC_FILES)))

OBJS := $(foreach obj, $(OBJS_FILES), $(BUILDDIR)/$(obj))

COMPILETIME=$(shell date '+%Y%m%d')

.PHONY: all multi
multi:
	$(MAKE) -j4 all

all: dfu

show:
	$(foreach v, $(.VARIABLES), $(info $(v) = $($(v))))

hex: $(TARGET).hex

dfu: $(TARGET).dfu

dump: $(TARGET).dump

space: $(TARGET).space

doc:
	doxygen doxygen.cfg

upload: $(TARGET).dfu
	$(MIDI_UTIL) -r
	sleep 2
	$(DFU_UTIL) -d 15a2:0500 -D "$<"

flash-boot:
	../ocd/upload.sh ../dfu/bootloader/boot-$(MCU)-64.bin 0x0000

flash: $(TARGET).bin
	../ocd/upload.sh $(TARGET).bin 8192

gmon: gmon.out
	$(Q)$(GPROFX) $(TARGET).elf > $(TARGET).gprof

showifdef:
	awk -F'ifdef ' '/#ifdef/{print$$2}' *.cpp | sort -u

sf2:
	zsh -c "conda activate vindor; python tools/sf2vindor.py"

ui/ReadFingering.cpp: ui/FingerTables.auto.hpp

ui/FingerTables.auto.hpp: kb/create/*.py
	kb/create/analyze.py --cpp --out ui/FingerTables.auto.hpp

pressuremap.h: tools/pressuremap.py
	python tools/pressuremap.py > pressuremap.h

$(BUILDDIR)/%.o: %.c
	@echo "[CC]\t$<"
	$(Q)mkdir -p "$(dir $@)"
	$(Q)$(CC) $(CPPFLAGS) $(CFLAGS) $(L_INC) -o "$@" -c "$<"

$(BUILDDIR)/%.o: %.ino
	@echo "[CXX]\t$<"
	$(Q)mkdir -p "$(dir $@)"
	$(Q)$(CC) $(CPPFLAGS) $(CXXFLAGS) $(L_INC) -o "$@" -c -x c++ -include Arduino.h "$<"

$(BUILDDIR)/%.o: %.cpp
	@echo "[CXX]\t$<"
	@mkdir -p "$(dir $@)"
	$(Q)$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(L_INC) -o "$@" -c "$<"

$(BUILDDIR)/%.o: %.S
	@echo "[ASM]\t$<"
	$(Q)mkdir -p "$(dir $@)"
	$(Q)$(CC) $(SFLAGS) $(L_INC) -o "$@" -c "$<"

$(TARGET).elf: $(OBJS) $(LDSCRIPT)
	@echo "[LD]\t$@"
	$(Q)$(CC) $(LDFLAGS) -o "$@" $(OBJS) $(LIBS)

%.nm: %.elf
	$(Q)$(NM) -C -l -t d --print-size --size-sort "$<" | awk '$$1>536854528{print}' >"$@"

%.dump: %.elf
	$(Q)$(OBJDUMP) -x -d -t -S "$<" >"$@"

%.space: %.map
	python3 tools/space3.py > "$@"

%.hex: %.elf
	@echo "[HEX]\t$@"
	$(Q)$(SIZE) "$<"
	$(Q)$(OBJCOPY) -O ihex -R .eeprom "$<" "$@"

%.bin: %.elf
	@echo "[BIN]\t$@"
	$(Q)$(SIZE) "$<"
	$(Q)$(OBJCOPY) -O binary -R .eeprom "$<" "$@"

%.dfu: %.elf
	@echo "[DFU]\t$@"
	$(Q)$(SIZE) "$<"
	$(Q)$(OBJCOPY) -O binary "$<" "$@"
	$(DFU_SUFFIX) -a $@

%.zip: %.hex
	zip $@ $<

.PHONY: web clean cleanall

web: $(TARGET).zip
	cp $(TARGET).zip ../../Web

rev_dfu: $(TARGET).dfu
	cp $(TARGET).dfu $(TARGET)-R$(PCBREV).dfu

# old revisions 47 49 51 52 53
zdf:
	-rm $(TARGET)-R*.dfu
	for rev in 56 60; do \
		make cleaner ; \
		make PCBREV=$$rev RELEASE=1 rev_dfu ; \
	done
	echo "date="`date` > zdf.txt
	echo "version=$(FIRMWARE_VERSION)" >> zdf.txt
	echo "platform=$(PLATFORM_VERSION)" >> zdf.txt

	zip $(ZIPNAME)-$(COMPILETIME).zdf zdf.txt $(TARGET)-R*.dfu
	-mkdir -p archive/$(COMPILETIME)
	mv zdf.txt $(TARGET)-R*.dfu archive/$(COMPILETIME)
	cp $(ZIPNAME)-$(COMPILETIME).zdf archive/$(ZIPNAME)-latest.zdf
	mv $(ZIPNAME)-$(COMPILETIME).zdf archive


# compiler generated dependency info
-include $(OBJS:.o=.d)

cleaner: clean
	rm -rf "$(BUILDDIR)" *.auto.hpp

clean:
	@echo Cleaning...
	-rm -rf "$(BUILDDIR)"/*
	-rm -f "$(TARGET).elf" "$(TARGET).hex" "$(TARGET).nm" "$(TARGET).map" "$(TARGET).dump" "$(TARGET).dfu" "$(TARGET).bin"
