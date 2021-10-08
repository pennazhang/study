
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

all: clean syslog

SYSDEBUG_OBJ = syslog.o

%.o:%.cpp
	$(CXX) -c $(CFLAGS) $(CPPFLAGS) $< -o $@ 
	
syslog:  $(SYSDEBUG_OBJ)
	$(CC) -o $@ $(SYSDEBUG_OBJ) -L. $(LDFLAGS) 

clean:
	$(RM) *.o *.exe *.bak *.pre syslog
		
