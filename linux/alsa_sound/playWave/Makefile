
CROSS_COMPILE 	?= 

CC             	?= $(CROSS_COMPILE)gcc 
CXX 			?= $(CROSS_COMPILE)g++
LD 				?= $(CROSS_COMPILE)ld
AR 				?= $(CROSS_COMPILE)


CPPFLAGS			+= -DRUN_SYSTEM_WHEN_POSSIBLE 
LDFLAGS            += -g -Wall  -lstdc++

ifeq ($(MAKELEVEL),0)
  PATH := $(PATH):$(PPC_TOOL_PATH)
endif

#
# make sure other makes get our env
#
export 

all: clean playWave

SYSDEBUG_OBJ = playWave.o

%.o:%.cpp
	$(CXX) -c $(CFLAGS) $(CPPFLAGS) $< -o $@ 
	
playWave:  $(SYSDEBUG_OBJ)
	$(CC) -o $@ $(SYSDEBUG_OBJ) -L. $(LDFLAGS) -lasound

clean:
	$(RM) *.o *.exe *.bak *.pre playWave
		
