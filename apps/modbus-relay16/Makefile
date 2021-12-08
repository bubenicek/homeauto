
PROJECT = modbus-relay

PROJECT_DIRS += src 
PROJECT_DIRS += src/cfg 

PROJECT_SOURCEFILES += main.c

all: $(PROJECT) makebin

### Compiler options
CFLAGS += -O2
CFLAGS += -DCFG_DEBUG=0

### RTOS 
include ../../core/sys/openos/Makefile.inc


MODULES += core/lib/modbus


### Common sources
include ../../Makefile.inc

makebin:
	$(Q)$(OBJCOPY) -O binary -R .eeprom -R .eesafe $(PROJECT).$(TARGET) $(PROJECT).$(TARGET).bin
	$(Q)$(OBJCOPY) -O ihex -R .eeprom -R .eesafe $(PROJECT).$(TARGET) $(PROJECT).$(TARGET).hex
#	$(OBJDUMP) -D -S $(PROJECT).$(TARGET) > $(PROJECT).$(TARGET).lst
	$(Q)$(SIZE) -A $(PROJECT).$(TARGET)
	$(Q)$(SIZE) --mcu=atmega328p --format=avr $(PROJECT).$(TARGET)
