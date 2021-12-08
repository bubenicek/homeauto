
PROJECT = test-ebus
PROJECT_DIRS += src

PROJECT_SOURCEFILES += main.c

all: $(PROJECT) makebin

### Compiler options
CFLAGS += -DCFG_DEBUG=1

MODULES += core/lib/ebus

### Common sources
include ../../Makefile.inc
