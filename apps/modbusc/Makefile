
PROJECT = modbusc

PROJECT_DIRS += src
PROJECT_DIRS += src/cfg

PROJECT_SOURCEFILES += main.c

all: $(PROJECT)

### Compiler options
CFLAGS += -O2
CFLAGS += -DCFG_DEBUG=1

### RTOS
#include ../../core/sys/openos/Makefile.inc


MODULES += core/lib/modbus


### Common sources
include ../../Makefile.inc
