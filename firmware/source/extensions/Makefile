# Vindor Firmware Makefile
#

# Q=@

TARGET = sample
OBJS   = extensions.o

COMPILER_PATH = /Applications/ARM/bin
# names for the compiler programs
CC      = $(COMPILER_PATH)/arm-none-eabi-gcc
CXX     = $(COMPILER_PATH)/arm-none-eabi-g++
OBJCOPY = $(COMPILER_PATH)/arm-none-eabi-objcopy
SIZE    = $(COMPILER_PATH)/arm-none-eabi-size
OBJDUMP = $(COMPILER_PATH)/arm-none-eabi-objdump

# CPPFLAGS = compiler options for C and C++
CPPFLAGS = -Wall -MMD -mcpu=cortex-m4 -mthumb -fsingle-precision-constant -ffast-math -nostdlib -ffunction-sections -fdata-sections -O2

# compiler options for C++ only
CXXFLAGS = -std=gnu++0x -felide-constructors -fno-exceptions -fno-rtti

# compiler options for C only
CFLAGS = 

# linker options
LDSCRIPT = mk20dx256dfu.ld
LDFLAGS = -lm -dead_strip -Wl,--gc-sections,--defsym=__rtc_localtime=0 --specs=nano.specs -mcpu=cortex-m4 -mthumb -mcpu=cortex-m4 -mthumb -T$(LDSCRIPT)

all: $(TARGET).bin
        
%.o: %.c
	@echo "[CC]\t$<"
	$(Q)mkdir -p "$(dir $@)"
	$(Q)$(CC) $(CPPFLAGS) $(CFLAGS) -o "$@" -c "$<"

%.o: %.ino
	@echo "[CXX]\t$<"
	$(Q)mkdir -p "$(dir $@)"
	$(Q)$(CC) $(CPPFLAGS) $(CXXFLAGS)  -o "$@" -c -x c++ -include Arduino.h "$<"

%.o: %.cpp
	@echo "[CXX]\t$<"
	@mkdir -p "$(dir $@)"
	$(Q)$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o "$@" -c "$<"

%.o: %.S
	@echo "[CC]\t$<"
	$(Q)mkdir -p "$(dir $@)"
	$(Q)$(CC) $(CPPFLAGS) $(CFLAGS) -o "$@" -c "$<"
	
$(TARGET).elf: $(OBJS) $(LDSCRIPT)
	@echo "[LD]\t$@"
	$(Q)$(CC) $(LDFLAGS) -o "$@" -Wl,-Map,$(TARGET).map $(OBJS)

%.bin: %.elf
	@echo "[BIN]\t$@"
	$(Q)$(SIZE) "$<"
	$(Q)$(OBJCOPY) -O binary -R .eeprom "$<" "$@"

%.dump: %.elf
	$(Q)$(OBJDUMP) -x -d -t -S "$<" >"$@"

clean:
	@echo Cleaning...
	$(Q)rm -rf *.o
	$(Q)rm -f "$(TARGET).elf"* "$(TARGET).map" "$(TARGET).dump" "$(TARGET).bin"
