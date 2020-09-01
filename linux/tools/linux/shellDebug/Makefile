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

#for UIMixer
#CROSS_COMPILE   ?= /usr/lib/ccache/arm-linux-gnueabihf-
#ARCH            ?= arm

# for DVI
#CROSS_COMPILE   ?= /git/dvi/software/Atmel/buildroot-at91/output/host/usr/bin/arm-linux-
#ARCH            ?= arm

#for RHEL5.3 - 64bit
CROSS_COMPILE 	?= 
ARCH            ?= i686


#for RHEL4.8 - 32bit
#CROSS_COMPILE 	?= 
#ARCH            ?= i386

CC             	:= $(CROSS_COMPILE)gcc
CXX 			:= $(CROSS_COMPILE)g++
LD 				:= $(CROSS_COMPILE)ld
AR 				:= $(CROSS_COMPILE)ar

CPPFLAGS			+= -DRUN_SYSTEM_WHEN_POSSIBLE
LDFLAGS            += -g -Wall -lpthread -lstdc++

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
	@echo " Emerson Sysdev Module Build - Percy "
	@echo " "
	@echo " If you are creating a new release, the appropriate"
	@echo " version numbers must be updated in the following files:"
	@echo "    versions"
	@echo " "
	@echo "++++++++++++++++++++++++++++++++++++++++++++++++++++++"
	@echo " Normal Usage Targets:"
	@echo "  all..................... clean & rebuild shellDebug"
	@echo "  shellDebug............ build shellDebug"
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

all: clean shellDebug

DEFINE_VERSION:
#	echo "#ifndef CONFIG_BLACKADDER" > $(CURRENTDIR)/define.h 
#	echo "#define CONFIG_BLACKADDER" >> $(CURRENTDIR)/define.h	
#	echo "#endif" >> $(CURRENTDIR)/define.h

SYSDEBUG_OBJ = main.o tcpServerFrame.o commonLib.o shellFrame.o

%.o:%.cpp
	$(CXX) -c $(CFLAGS) $(CPPFLAGS) $< -o $@ 
	
shellDebug:   DEFINE_VERSION $(SYSDEBUG_OBJ)
	$(CC) -o $@ $(SYSDEBUG_OBJ) -L. $(LDFLAGS) 

clean:
	rm -f *.o *.exe *.bak shellDebug *.pre
	
copy:
	cp shellDebug /mnt/disk2/rootfs-nfs/root/.
	
