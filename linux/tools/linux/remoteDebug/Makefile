# Makefile - makefile for GPU Application
#
# Copyright 
#
# modification history
# --------------------
#
# DESCRIPTION
#

#CROSS_COMPILE   ?= arm-linux-gnueabihf-

LD              = $(CROSS_COMPILE)g++
CC              := $(CROSS_COMPILE)g++
CFLAGS          += -g -Wall -fpermissive

# Sometimes we need to use -lsupc++; sometimes we need to use -lcurses; sometimes we don't need any of them, it depends on the compiler.
LIBS 			= -lpthread -lcurses -lsupc++	

RemoteDebug_OBJ = main.o common.o debugFrame.o tcpServer.o funcInterface.o funcLib.o

all: clean remoteDebug

remoteDebug:   $(RemoteDebug_OBJ)
	$(CC) -o $@ $(RemoteDebug_OBJ)	$(LIBS)

clean:
	rm -f *.o remoteDebug

%.o: %.cpp
	$(CC) -c $(CFLAGS) $< -o $@	
