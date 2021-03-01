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
LIBS 			= -lssl -lcrypto

SSL_SERVER_OBJ = SSL-Server.o
SSL_CLIENT_OBJ = SSL-Client.o

all: clean sslServer sslClient

sslServer:   $(SSL_SERVER_OBJ)
	$(CC) -o $@ $(SSL_SERVER_OBJ)	$(LIBS)
	
sslClient:   $(SSL_CLIENT_OBJ)
	$(CC) -o $@ $(SSL_CLIENT_OBJ)	$(LIBS)
	

clean:
	rm -f *.o sslServer sslClient

%.o: %.cpp
	$(CC) -c $(CFLAGS) $< -o $@	
