# Makefile - makefile for GPU Application
#
# Copyright 
#
# modification history
# --------------------
#
# DESCRIPTION
#

LD               = gcc
CC               = gcc
CFLAGS           += -g -Wall \
-I/usr/local/include/directfb


LOADCFLAGS += -Map cygwin.map

HELLO_OBJ = line.o


hello:   $(HELLO_OBJ)
	$(CC) -o $@ $(HELLO_OBJ)  -ldirectfb


line.o: line.c
	$(CC) $(CFLAGS) -c line.c


clean:
	rm -f *.o hello
	


