#######################################################
# Environment setup

ARDUINO ?= /Applications/Arduino.app/Contents/Java
TOOLS_PATH = $(ARDUINO)/hardware/tools
COMPILER_PATH = $(TOOLS_PATH)/arm/bin
CC = $(COMPILER_PATH)/arm-none-eabi-gcc
CXX = $(COMPILER_PATH)/arm-none-eabi-g++
OBJCOPY = $(COMPILER_PATH)/arm-none-eabi-objcopy
OBJDUMP = $(COMPILER_PATH)/arm-none-eabi-objdump
SIZE = $(COMPILER_PATH)/arm-none-eabi-size

DFU_DIR=../dfu-util-dfu-util/src/
DFU_SUFFIX = $(DFU_DIR)/dfu-suffix
DFU_UTIL = $(DFU_DIR)/dfu-util

# toolchain
#CC = arm-none-eabi-gcc
#CXX = arm-none-eabi-g++
#OBJCOPY = arm-none-eabi-objcopy
#OBJDUMP = arm-none-eabi-objdump
#SIZE = arm-none-eabi-size

#######################################################

# Sources
C_FILES = \
	mk20dx128.c \
	usb_desc.c \
	usb_dev.c \
	bootloader.c \
	../encrypt/chacha.c \
	dfu.c

CPP_FILES =

# Headers
INCLUDES = -I. -I../encrypt

# DCPU=-D__MK20DX128__
# MCPU=mk20dx128
# NCPU=64

DCPU=-D__MK20DX256__ -D__MK20DX256VLL10__
MCPU=mk20dx256
NCPU=100

# DCPU=-D__MK20DX256__
# MCPU=mk20dx256
# NCPU=64

# The name of your project (used to name the compiled .hex file)
TARGET = boot-$(MCPU)-$(NCPU)

OPTIONS=$(DCPU) -DARDUINO=10600 -DTEENSYDUINO=125 -DF_CPU=96000000

# OPTIONS=$(DCPU) -DARDUINO=10600 -DTEENSYDUINO=125 -DF_CPU=96000000 -DSECURE_FLASH

# CPPFLAGS = compiler options for C and C++.
# More aggressive size optimizations here than in the normal firmware!
CPPFLAGS =-g -Og -Wall -Wno-sign-compare -Wno-strict-aliasing \
	-ffunction-sections -fdata-sections -nostdlib \
	-mcpu=cortex-m4 -mthumb -nostdlib -MMD $(OPTIONS) $(INCLUDES)

# compiler options for C++ only
CXXFLAGS = -std=gnu++0x -felide-constructors -fno-exceptions -fno-rtti

# compiler options for C only
CFLAGS =

# linker script
#LDSCRIPT = mk20dx128.ld
LDSCRIPT = $(MCPU).ld

# linker options
LDFLAGS = -Os -Wl,--gc-sections -mcpu=cortex-m4 -mthumb \
	-ffunction-sections -fdata-sections -nostdlib -T$(LDSCRIPT)

# additional libraries to link
LIBS = -lm

OBJS := $(C_FILES:.c=.o) $(CPP_FILES:.cpp=.o)

all: $(TARGET).hex $(TARGET).bin size

$(TARGET).elf: $(OBJS) $(LDSCRIPT)
	$(CC) $(LDFLAGS) -o $@ -Wl,-Map,$(TARGET).map $(OBJS) $(LIBS)

%.hex: %.elf
	$(OBJCOPY) -O ihex $< $@

%.bin: %.elf
	$(OBJCOPY) -O binary $< $@

%.dfu: %.elf
	$(SIZE) "$<"
	$(OBJCOPY) -O binary "$<" "$@"
	$(DFU_SUFFIX) -a $@

# compiler generated dependency info
-include $(OBJS:.o=.d)

clean:
	rm -f *.d *.o $(TARGET).elf $(TARGET).hex $(TARGET).bin

# Install with OpenOCD. (No code protection!)
install: $(TARGET).hex
	openocd -f openocd.cfg -c "program $(TARGET).hex verify reset"

upload: $(TARGET).dfu
	$(DFU_UTIL) -d 1d50:6082 -D "$<"

upload-teensy: $(TARGET).hex
	../../teensy_loader_cli/teensy_loader_cli --mcu $(MCPU) -w -v $(TARGET).hex

objdump: $(TARGET).elf
	$(OBJDUMP) -d -S $< > $@.txt

size: $(TARGET).elf
	$(SIZE) $<

.PHONY: all clean install objdump size
