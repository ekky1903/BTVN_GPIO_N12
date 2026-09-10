CC = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy
CFLAGS = -mcpu=cortex-m3 -mthumb -O2 -Wall -g
ASFLAGS = -mcpu=cortex-m3 -mthumb
LDFLAGS = -T linker.ld -nostartfiles -Wl,--gc-sections

TARGET = blink

all: $(TARGET).bin $(TARGET).hex

main.o: main.c
	$(CC) $(CFLAGS) -c main.c -o main.o

startup.o: startup.s
	$(CC) $(ASFLAGS) -c startup.s -o startup.o

$(TARGET).elf: main.o startup.o
	$(CC) $(CFLAGS) $(LDFLAGS) main.o startup.o -o $@

$(TARGET).bin: $(TARGET).elf
	$(OBJCOPY) -O binary $< $@

$(TARGET).hex: $(TARGET).elf
	$(OBJCOPY) -O ihex $< $@

flash-stlink: $(TARGET).bin
	st-flash write $(TARGET).bin 0x08000000

flash-openocd: $(TARGET).elf
	openocd -f interface/stlink.cfg -f target/stm32f1x.cfg -c "program $(TARGET).elf verify reset exit"

clean:
	rm -f *.o $(TARGET).elf $(TARGET).bin $(TARGET).hex
