
PROJECT = watmeter

PROJECT_DIRS += src 
PROJECT_DIRS += src/cfg 

PROJECT_SOURCEFILES += main.c
PROJECT_SOURCEFILES += emontask.c

ifeq ($(LINKMODE),boot)
all: $(PROJECT) makebin makebin_boot
else
all: $(PROJECT) makebin
endif

### Compiler options
CFLAGS += -Os
CFLAGS += -DCFG_DEBUG=1

### RTOS 
include ../../core/sys/openos/Makefile.inc

MODULES += core/lib/emon

### Common sources
include ../../Makefile.inc
	
