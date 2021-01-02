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

LD              ?= $(CROSS_COMPILE)gcc
CC              ?= $(CROSS_COMPILE)gcc
CFLAGS          += -g -Wall -fpermissive -Wint-to-pointer-cast
LIBS 			= -lpthread -lcurses

TCP_SERVER_OBJ = tcpServer.o
TCP_CLIENT_OBJ = tcpClient.o

all: clean tcpServer tcpClient

tcpServer:   $(TCP_SERVER_OBJ)
	$(CC) -o $@ $(TCP_SERVER_OBJ)	$(LIBS)
	
tcpClient:   $(TCP_CLIENT_OBJ)
	$(CC) -o $@ $(TCP_CLIENT_OBJ)	$(LIBS)
	

clean:
	rm -f *.o tcpServer tcpClient

%.o: %.cpp
	$(CC) -c $(CFLAGS) $< -o $@	
