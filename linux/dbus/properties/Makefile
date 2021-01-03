
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

all: clean header adaptor proxy

ADAPTOR_OBJ = propsgs-server.o
PROXY_OBJ = propsgs-client.o

%.o:%.cpp
	$(CXX) -c $(CFLAGS) $(CPPFLAGS) $< -o $@ 
	
adaptor:  $(ADAPTOR_OBJ)
	$(CC) -o $@ $(ADAPTOR_OBJ) -L. $(LDFLAGS) 

proxy:  $(PROXY_OBJ)
	$(CC) -o $@ $(PROXY_OBJ) -L. $(LDFLAGS) 

clean:
	$(RM) *.o *.exe *.bak *.pre adaptor proxy propsgs-glue-proxy.h propsgs-glue-adaptor.h

header:
	dbusxx-xml2cpp propsgs-introspect.xml --proxy=propsgs-glue-proxy.h --adaptor=propsgs-glue-adaptor.h
