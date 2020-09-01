#
# Emerson ECC P2020 build - 01/28/2010 Percy
#
#debug=0

SHELL           ?= /bin/sh

TOPDIR		?= $(shell pwd)
CURRENTDIR	=  $(shell pwd)
HOSTCC  	?= 'ccache /usr/bin/gcc -B/usr/bin/'
HOSTSTRIP	?= /usr/bin/strip
OPTFLAGS	?= -Os
DATE 		?= $(shell date)
USER 		?= $(shell whoami)
HOST		?= $(shell hostname)
OUTPUTDIR       ?= $(TOPDIR)

#PPC_TOOL_PATH 	?= /opt/freescale/usr/local/gcc-4.3.74-eglibc-2.8.74-dp-2/powerpc-none-linux-gnuspe/bin:/opt/freescale/ltib/usr/bin
#CROSS_COMPILE   ?= powerpc-none-linux-gnuspe-
CC               := $(CROSS_COMPILE)gcc

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
	@echo "  all..................... build module and sysdebug"
	@echo "  clean................... clean all"
	@echo "++++++++++++++++++++++++++++++++++++++++++++++++++++++"
	@echo " Other Usage Targets:"
	@echo "  sysdev.ko................ build the modlule: sysdev.ko"
	@echo "  sysdebug................build shelldebug"
	@echo "++++++++++++++++++++++++++++++++++++++++++++++++++++++"
	@echo "  current output dir = $(OUTPUTDIR)/."
	@echo "  you can set the output dir by modify the variable: OUTPUTDIR "
	@echo "      For example: export OUTPUTDIR=/tftpboot "
	@echo " "

help: message


debugx:
	$(MAKE) -C driver -f Makefile-rhel4.8-32bit debug
	$(MAKE) -C app -f Makefile-rhel4.8-32bit debug
	
all: clean sysdev.ko sysdebug

sysdev.ko:
	$(MAKE) -C driver -f Makefile-x86 sysdev.ko 
	-cp ./driver/sysdev.ko $(OUTPUTDIR)/.
	
sysdebug:
	$(MAKE) -C app -f Makefile-rhel4.8-32bit sysdebug 
	-cp ./app/sysdebug $(OUTPUTDIR)/.

clean:
	$(MAKE) -C app -f Makefile-rhel4.8-32bit clean
	$(MAKE) -C driver -f Makefile-x86 clean
	$(RM) $(OUTPUTDIR)/sysdev.ko $(OUTPUTDIR)/libsysdev.so $(OUTPUTDIR)/sysdebug 

