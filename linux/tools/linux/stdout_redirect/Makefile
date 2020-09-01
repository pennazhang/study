#
# Emerson ECC P2020 build - 01/28/2010 Percy
#
#debug=0

SHELL           ?= /bin/sh

TOPDIR		?= $(shell pwd)
CURRENTDIR	=  $(shell pwd)
#ARCH            ?= powerpc
HOSTCC  	?= 'ccache /usr/bin/gcc -B/usr/bin/'
HOSTSTRIP	?= /usr/bin/strip
OPTFLAGS	?= -Os
DATE 		?= $(shell date)
USER 		?= $(shell whoami)
HOST		?= $(shell hostname)
OUTPUTDIR       ?= $(TOPDIR)

#for Blackadder compile at Blackadder
#PPC_TOOL_PATH 	?= /opt/freescale/usr/local/gcc-4.3.74-eglibc-2.8.74-dp-2/powerpc-none-linux-gnuspe/bin:/opt/freescale/ltib/usr/bin
CROSS_COMPILE   ?=
ARCH            ?= 

#for RHEL5.3 - 32bit
#CROSS_COMPILE 	?= 
#ARCH            ?= i686


#for RHEL4.8 - 32bit
#CROSS_COMPILE 	?= 
#ARCH            ?= i386
G++				:= $(CROSS_COMPILE)gcc
GCC				 := $(CROSS_COMPILE)gcc
CC               := $(CROSS_COMPILE)gcc
LD               := $(CROSS_COMPILE)ld
CFLAG            += -g -Wall -lpthread 
CFLAGS           += -g -Wall 

ifeq ($(MAKELEVEL),0)
  PATH := $(PATH):$(PPC_TOOL_PATH)
endif

#
# make sure other makes get our env
#
export 

.PHONY: message printver endmessage dirs all u-boot linux nexthop endmessage
#
# initial target
#
message:
	@echo "++++++++++++++++++++++++++++++++++++++++++++++++++++++"
	@echo " "
	@echo " Emerson Sysdev Module Build "
	@echo " "
	@echo " If you are creating a new release, the appropriate"
	@echo " version numbers must be updated in the following files:"
	@echo "    versions"
	@echo " "
	@echo "++++++++++++++++++++++++++++++++++++++++++++++++++++++"
	@echo " Normal Usage Targets:"
	@echo "  all..................... build module, share object and stdout_redirect"
	@echo "  stdout_redirect......... build stdout_redirect"
	@echo "  clean................... clean the result"
	@echo " "

help: message


debug:
	@echo "++++++++++++++++++++++++++++++++++++++++++++++++++++++"
	@echo "    $(DATE)"
	@echo "    $(USER)@$(HOST)"
	@echo "    version:"
	@echo "       `cat version.txt`"
	@echo "    CROSS_COMPILE: $(CROSS_COMPILE)"
	@echo "    PATH         : $(PATH)"
	@echo "    PPC_TOOL_PATH: $(PPC_TOOL_PATH)"
	@echo "    KERNEL_SRC   : $(KERNEL_SRC)"
	@echo "    CC           : $(CC)"
	@echo "++++++++++++++++++++++++++++++++++++++++++++++++++++++"

all: clean stdout_redirect

DEFINE_VERSION:
#	echo "#ifndef CONFIG_BLACKADDER" > $(CURRENTDIR)/define.h 
#	echo "#define CONFIG_BLACKADDER" >> $(CURRENTDIR)/define.h	
#	echo "#endif" >> $(CURRENTDIR)/define.h

stdout_redirect_OBJ = main.o
stdout_redirect:   DEFINE_VERSION $(stdout_redirect_OBJ)
	$(CC) $(CFLAG) -o $@ $(stdout_redirect_OBJ) -L.

main.o: main.cpp
	$(CC) $(CFLAGS) -c main.cpp


clean:
	rm -f *.o *.exe *.bak stdout_redirect
	

