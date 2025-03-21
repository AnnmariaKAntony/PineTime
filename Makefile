# C Compiler
CC = arm-none-eabi-gcc
CXX = arm-none-eabi-g++

# Common Compiler Flags
COMMONFLAGS = -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard -O2 -Wall

# Device Definitions for nRF52832
DEVICE = -DNRF52832_XXAA -DNRF52

# C and C++ Specific Flags
CFLAGS = $(COMMONFLAGS) -std=c11 $(DEVICE)
CXXFLAGS = $(COMMONFLAGS) -std=c++11 $(DEVICE)

# Include paths (update these paths to match your SDK installation)
INCLUDES = -I$(HOME)/nrf52_sdk/modules/nrfx/mdk \
           -I$(HOME)/nrf52_sdk/modules/nrfx \
		   -I$(HOME)/nrf52_sdk/modules/nrfx/hal \
           -I$(HOME)/nrf52_sdk/integration/nrfx \
           -I$(HOME)/nrf52_sdk/components/toolchain/cmsis \
           -I$(HOME)/nrf52_sdk/components/toolchain/cmsis/include \
           -I$(HOME)/nrf52_sdk/components/drivers_nrf/rtc \
		   -I$(HOME)/nrf52_sdk/components/libraries/util \
		   -I$(HOME)/nrf52_sdk/config/nrf52832/config \
		   -I$(HOME)/nrf52_sdk/components/softdevice/s132/headers \
		   -I$(HOME)/nrf52_sdk/components/libraries/delay \
		   -I$(HOME)/nrf52_sdk/modules/nrfx/drivers/include \
		   -I$(HOME)/nrf52_sdk/components/libraries/log \
		   -I$(HOME)/nrf52_sdk/components/libraries/experimental_section_vars \
		   -I$(HOME)/nrf52_sdk/components/libraries/log/src \
		   -I$(HOME)/nrf52_sdk/modules/nrfx/drivers/include \
		   -I$(HOME)/nrf52_sdk/external/thedotfactory_fonts/ \
		   -I$(HOME)/nrf52_sdk/components/libraries/svc/ \
		   -I$(HOME)/nrf52_sdk/components/boards/

# Append include paths to compiler flags
CFLAGS += $(INCLUDES)
CFLAGS += -DBOARD_PINETIME_DEVKIT1

CXXFLAGS += $(INCLUDES)

# Assembler Flags
ASFLAGS = -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard

# Linker Flags
LDFLAGS = -T gcc_nrf52.ld -nostartfiles -Wl,--gc-sections

# C and C++ source files
# CSRCS = src/display.c 
# CXXSRCS = src/time.cpp src/main.cpp
# SRC_FILES += src/St7789.cpp
# SRC_FILES += src/SpiMaster.cpp
# SRC_FILES += src/Spi.cpp

CXXSRCS = src/time.cpp src/main.cpp \
          src/St7789.cpp src/SpiMaster.cpp src/components/Gfx.cpp src/DisplayApp/DisplayApp.cpp\
		  src/DisplayApp/Screens/Clock.cpp src/DisplayApp/Screens/Screen.cpp

CSRCS = $(HOME)/nrf52_sdk/modules/nrfx/drivers/src/nrfx_gpiote.c \
        $(HOME)/nrf52_sdk/components/libraries/util/app_error.c \
		src/DisplayApp/Fonts/lcdfont70.c src/DisplayApp/Fonts/lcdfont14.c

# Startup assembly file
ASRC = startup_nrf52.s

# Object files
COBJS = $(CSRCS:.c=.o)
CXXOBJS = $(CXXSRCS:.cpp=.o)
ASOBJ = $(ASRC:.s=.o)
OBJS = $(COBJS) $(CXXOBJS) $(ASOBJ)

TARGET_DIR = build/src
TARGET = $(TARGET_DIR)/pinetime-app-1.15.0

all: $(TARGET_DIR) $(TARGET).elf

$(TARGET_DIR):
	mkdir -p $(TARGET_DIR)

# Rule for compiling C files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Rule for compiling C++ files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Rule for compiling assembly (.s) files
%.o: %.s
	$(CC) $(ASFLAGS) -c $< -o $@

$(TARGET).elf: $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(TARGET).elf $(LDFLAGS)

$(TARGET).bin: $(TARGET).elf
	arm-none-eabi-objcopy -O binary $(TARGET).elf $(TARGET).bin

flash: $(TARGET).elf
	# Flash the binary to 0x00000000 (adjust if necessary)
	st-flash write $(TARGET).elf 0x00000000

clean:
	rm -f $(OBJS) $(TARGET_DIR)/pinetime-app-1.15.0.elf $(TARGET_DIR)/pinetime-app-1.15.0.bin

