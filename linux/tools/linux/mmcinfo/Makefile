# Makefile - makefile for GPU Application
#
# Copyright 
#
# modification history
# --------------------
#
# DESCRIPTION
#

CROSS_COMPILE   ?= arm-linux-gnueabihf-

LD              = $(CROSS_COMPILE)gcc
CC              := $(CROSS_COMPILE)gcc
CFLAGS          += -g -Wall


MMCINFO_OBJ = mmcinfo.o

all: clean mmcinfo

mmcinfo:   $(MMCINFO_OBJ)
	$(CC) -o $@ $(MMCINFO_OBJ)

mmcinfo.o: mmcinfo.cpp
	$(CC) $(CFLAGS) -c mmcinfo.cpp

clean:
	rm -f *.o mmcinfo
	


