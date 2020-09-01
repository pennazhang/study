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
CFLAGS           += -g -Wall

#-I/usr/local/include/directfb


LOADCFLAGS += -Map test.map

TEST_OBJ = test1.o


test:   $(TEST_OBJ)
	$(CC) -o $@ $(TEST_OBJ)  -lpthread


test1.o: test1.c
	$(CC) $(CFLAGS) -c test1.c

clean:
	rm -f *.o test
	


