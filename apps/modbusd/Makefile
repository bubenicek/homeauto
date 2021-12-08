#/* ******************************************************************************
# *                                                                            *
# *                        evolving systems consulting                         *
# *                          http://www.evolvsys.cz                            *
# *                                                                            *
# *                           ALL RIGHTS RESERVED                              *
# *                                                                            *
# ******************************************************************************/
#/*
# * $HeadURL: svn://213.165.82.37/solo/STIX/SW/SuSW/trunk/Makefile.SuSW_IT $
# * $Revision: 5968 $
# * $Date: 2015-05-28 11:54:22 +0200 (Čt, 28 kvě 2015) $
# * $Author: escsro138 $
# */

# enviroment
CC = gcc
CXX = g++
AR = g++
LD = g++
TESTD = test -d
MKDIR = mkdir -p
RM = rm
RMDIR = rmdir

TARGET = modbusd

BINDIR = bin
OBJDIR = obj
SRCDIR = .

CFLAGS =
# STIX
#CFLAGS += -O2
#CFLAGS += -DENABLE_TRACES

# BSW switch
#CFLAGS += -mno-fpu -msoft-float -DNORTEMS

# source: A. Jung, ESA
#CFLAGS += -W -g3 -g -Wall -Wextra
# -Wconversion
CFLAGS += -Winit-self -Wredundant-decls  -Wshadow -Wpointer-arith
CFLAGS += -Wcast-qual -Wcast-align -Wundef -pedantic
CFLAGS += -std=c99

# do not compile if warning
# CFLAGS += -Werror

# sparc-elf-gcc.exe: this is valid for C++/ObjC++ but not for C
# CFLAGS += -Wno-deprecated

# sparc-elf-gcc.exe: not supported
# CFLAGS += -Wsign-conversion -Wvla -Wlogical-op
# CFLAGS += -Wno-error=old-style-cast -Wmissing-include-dirs

SRCS =
SRCS += main.c
SRCS += modbus.c
SRCS += serial.c
SRCS += tcp_socket.c

SRCS := $(addprefix $(SRCDIR)/,$(SRCS))
SRCS := $(SRCS)

OBJS = $(SRCS:%.c=$(OBJDIR)/%.o)
OBJDIRS = $(dir $(OBJS))

INCS =
INCS +=
INCS := -I$(SRCDIR) $(addprefix -I$(SRCDIR)/,$(INCS))

INCS := $(INCS)

RESINC =
LIBDIR =
LIB =

LDFLAGS =
LDFLAGS += BSW
LDFLAGS := $(addprefix -I$(SRCDIR)/,$(LDFLAGS))

LIBS =
#LIBS += -lpthread -lm -lrt


.PHONY: all clean \
  target_sim \
  clean_obj create_obj \
  clean_bin create_bin 

all: clean_obj create_obj create_bin target_sim

create_bin:
	$(info create_bin)
	@$(TESTD) $(BINDIR) || $(MKDIR) $(BINDIR)

create_obj:
	$(info create_obj)

	@for dir in $(OBJDIRS); do \
		$(TESTD) $$dir || $(MKDIR) $$dir; \
	done

clean_bin:
	$(RM) -rf $(BINDIR)/
	@$(info clean_bin done)

clean_obj:
	$(RM) -rf $(OBJDIR)/
	@$(info clean_obj done)

clean: clean_obj clean_bin
#####################################
target_sim: $(addsuffix _sim,$(OBJS))
	$(LD) -o $(BINDIR)/$(TARGET) $(CFLAGS_SIM) $(LDFLAGS) $(addsuffix _sim,$(OBJS)) $(LIBS)

$(addsuffix _sim,$(OBJS)): %.o_sim : %.o_sim_d
	$(CC) $(CFLAGS_SIM) $(INCS) -c $(patsubst $(OBJDIR)/%.o_sim_d,%.c,$<) -o $@
	$(CC) $(CFLAGS_SIM) $(INCS) -c $(patsubst $(OBJDIR)/%.o_sim_d,%.c,$<) -MM -MT $@ > $@_d

$(addsuffix _sim_d,$(OBJS)):

-include $(OBJS:.o=.o_sim_d)

