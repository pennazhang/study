
CROSS_COMPILE 	?= 

CC             	?= $(CROSS_COMPILE)gcc 
CXX 			?= $(CROSS_COMPILE)g++
LD 				?= $(CROSS_COMPILE)ld
AR 				?= $(CROSS_COMPILE)


CPPFLAGS			+= -DRUN_SYSTEM_WHEN_POSSIBLE 
LDFLAGS            += -g -Wall  -lstdc++ -ldbus-c++-1 -lpthread

ifeq ($(MAKELEVEL),0)
  PATH := $(PATH):$(PPC_TOOL_PATH)
endif

#
# make sure other makes get our env
#
export 

all: clean header echo-client echo-client-mt echo-server

ADAPTOR_OBJ = echo-server.o 
PROXY_OBJ = echo-client.o 
PROXY_MT_OBJ = echo-client-mt.o 

%.o:%.cpp
	$(CXX) -c $(CFLAGS) $(CPPFLAGS) $< -o $@ 
	
echo-server:  $(ADAPTOR_OBJ)
	$(CC) -o $@ $(ADAPTOR_OBJ) -L. $(LDFLAGS) 

echo-client:  $(PROXY_OBJ)
	$(CC) -o $@ $(PROXY_OBJ) -L. $(LDFLAGS) 

echo-client-mt:  $(PROXY_MT_OBJ)
	$(CC) -o $@ $(PROXY_MT_OBJ) -L. $(LDFLAGS) 

clean:
	$(RM) *.o *.exe *.bak *.pre echo-client echo-server echo-client-mt echo-client-glue.h ef-echo-server-glue.h

header:
	dbusxx-xml2cpp echo-introspect.xml --proxy=echo-client-glue.h --adaptor=echo-server-glue.h
	dbusxx-xml2cpp echo-introspect.xml --proxy=ef-echo-client-glue.h --adaptor=ef-echo-server-glue.h
