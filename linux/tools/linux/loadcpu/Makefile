# Makefile - makefile for GPU Application
#
# Copyright 
#
# modification history
# --------------------
#
# DESCRIPTION
#

CROSS_COMPILE   ?= /mnt/disk2/git/dvi/software/Atmel/buildroot-2017.02.5/output/host/usr/bin/arm-linux-

LD              = gcc
CC              := $(CROSS_COMPILE)gcc
CFLAGS          += -g -Wall

#-I/usr/local/include/directfb


LOADCFLAGS += -Map hello.map

HELLO_OBJ = loadcpu.o


hello:   $(HELLO_OBJ)
	$(CC) -o $@ $(HELLO_OBJ)  -lpthread


loadcpu.o: loadcpu.c
	$(CC) $(CFLAGS) -c loadcpu.c


clean:
	rm -f *.o hello
	


