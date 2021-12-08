
PROJECT = fancontrol

PROJECT_DIRS += src src/test
PROJECT_DIRS += src/cfg 

PROJECT_SOURCEFILES += main.c
PROJECT_SOURCEFILES += fanpwm.c
PROJECT_SOURCEFILES += console_cmd.c
PROJECT_SOURCEFILES += test_usb.c


ifeq ($(LINKMODE),boot)
all: $(PROJECT) makebin makebin_boot
else
all: $(PROJECT) makebin
endif

### Compiler options
CFLAGS += -O2
CFLAGS += -DCFG_DEBUG=1

## CPU options
CPU_USB_DEVICE=CDC

### RTOS 
include $(EMBEDX_ROOT)/core/sys/openos/Makefile.inc

## Serial console
MODULES += core/lib/console

## Common sources
include $(EMBEDX_ROOT)/Makefile.inc
	