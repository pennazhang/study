#
# Emerson ECC P2020 build - 01/28/2010 Percy
#
#debug=0

SHELL           ?= /bin/sh

TOPDIR      ?= $(shell pwd)
CURRENTDIR  =  $(shell pwd)
HOSTCC      ?= 'ccache /usr/bin/gcc -B/usr/bin/'
HOSTSTRIP   ?= /usr/bin/strip
OPTFLAGS    ?= -Os
DATE        ?= $(shell date)
USER        ?= $(shell whoami)
HOST        ?= $(shell hostname)
OUTPUTDIR       ?= $(TOPDIR)
#config_p2020rdb = 1

KERNEL_VERSION  :=  $(shell uname -r)
KERNEL_SRC      ?=  /lib/modules/$(KERNEL_VERSION)/build
ARCH            ?= i386

CC              ?= $(CROSS_COMPILE)gcc
LD              ?= $(CROSS_COMPILE)ld

obj-m           := inputdrv.o
inputdrv-objs   := moduleFrame.o 

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
	@echo " Emerson inputdrv Module Build "
	@echo " "
	@echo " If you are creating a new release, the appropriate"
	@echo " version numbers must be updated in the following files:"
	@echo "    versions"
	@echo " "
	@echo "++++++++++++++++++++++++++++++++++++++++++++++++++++++"
	@echo " Normal Usage Targets:"
	@echo "  all..................... rebuild inputdrv.ko again"
	@echo "  inputdrv.ko............. build inputdrv.ko"
	@echo "  clean................... clean the result"
	@echo "++++++++++++++++++++++++++++++++++++++++++++++++++++++"
	@echo "  current output dir = $(OUTPUTDIR)/."
	@echo "  you can set the output dir by modify the variable: OUTPUTDIR "
	@echo "      For example: export OUTPUTDIR=/tftpboot "
	@echo " "

help: message

debug:
	@echo "++++++++++++++++++++++++++++++++++++++++++++++++++++++"
	@echo "    $(DATE)"
	@echo "    $(USER)@$(HOST)"
	@echo "    version:"
	@echo "       `cat version.txt`"
	@echo "    MAKE         : $(MAKE)"
	@echo "    CC           : $(CC)"
	@echo "    LD           : $(LD)"
	@echo "    PATH         : $(PATH)"
	@echo "    PPC_TOOL_PATH: $(PPC_TOOL_PATH)"
	@echo "    KERNEL_SRC   : $(KERNEL_SRC)"
	@echo "    TOPDIR       : $(TOPDIR)"
	@echo "    CURRENTDIR   : $(CURRENTDIR)"
	@echo "++++++++++++++++++++++++++++++++++++++++++++++++++++++"

all: clean inputdrv.ko

inputdrv.ko:
	echo "#define $(ARCH)" > $(CURRENTDIR)/define.h
	$(MAKE) -C $(KERNEL_SRC) SUBDIRS=$(CURRENTDIR) modules  ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE)

clean: 
	$(RM) -rf .*cmd .tmp* *.o *.mod.c *.bak *.ko *.cmd $(TOPDIR)/Module.markers $(TOPDIR)/Module.symvers inputdrv.mod.c define.h
