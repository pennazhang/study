CC?=$(CROSS_COMPILE)gcc
CFLAGS=-I.
DEPS = 
OBJ = gpiotest.o 
%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

gpiotest: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)
	
debug:
	echo "CC = $(CC)"
	
clean:
	-rm -r *.o  gpiotest
