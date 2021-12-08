
PROJECT = ledcontrol

PROJECT_DIRS += src
PROJECT_DIRS += src/httpd/cgi src/httpd/rest-api

PROJECT_SOURCEFILES += main.c
PROJECT_SOURCEFILES += ledctl.c
PROJECT_SOURCEFILES += network.c

# Rest API
PROJECT_SOURCEFILES += cgicalls.c
PROJECT_SOURCEFILES += json_output.c
PROJECT_SOURCEFILES += rest_api.c
PROJECT_SOURCEFILES += rest_api_sys.c
PROJECT_SOURCEFILES += rest_api_led.c


all: $(PROJECT) makebin

# Compiler options
CFLAGS += -O2
CFLAGS += -DCFG_DEBUG=1

MODULES += core/dev/video/ledstrip

### HTTPD
include ../../core/net/httpd/Makefile.inc

# Common sources
include ../../Makefile.inc

