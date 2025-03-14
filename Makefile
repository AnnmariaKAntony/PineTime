# Makefile for PineTime bare metal OS

CC = arm-none-eabi-gcc
CFLAGS = -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard -O2 -Wall -std=c11
LDFLAGS = -T gcc_nrf52.ld -nostartfiles -Wl,--gc-sections

SRCS = src/main.c src/time.c src/display.c
OBJS = $(SRCS:.c=.o)
TARGET_DIR = build/src
TARGET = $(TARGET_DIR)/pinetime-app-1.15.0

all: $(TARGET_DIR) $(TARGET).elf

$(TARGET_DIR):
	mkdir -p $(TARGET_DIR)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET).elf: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET).elf $(LDFLAGS)

$(TARGET).bin: $(TARGET).elf
	arm-none-eabi-objcopy -O binary $(TARGET).elf $(TARGET).bin

flash: $(TARGET).elf
	# Flash the binary to 0x00000000 (adjust if necessary)
	st-flash write $(TARGET).bin 0x00000000

clean:
	rm -f $(OBJS) $(TARGET_DIR)/pinetime-app-1.15.0.elf $(TARGET_DIR)/pinetime-app-1.15.0.bin
