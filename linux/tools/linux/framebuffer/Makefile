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

FBTEST_OBJ = test.o


fbtest:   $(FBTEST_OBJ)
	$(CC) -o $@ $(FBTEST_OBJ)  -lpthread


test.o: test.c
	$(CC) $(CFLAGS) -c test.c


clean:
	rm -f *.o fbtest
	


