#
# Emerson ECC P2020 build - 01/28/2010 Percy
#
#debug=0

SHELL           ?= /bin/sh

TOPDIR		?= $(shell pwd)
BASEDIR         ?= $(TOPDIR)
ARCH            ?= powerpc
HOSTCC  	?= 'ccache /usr/bin/gcc -B/usr/bin/'
HOSTSTRIP	?= /usr/bin/strip
OPTFLAGS	?= -Os
CROSS_COMPILE 	?= powerpc-none-linux-gnuspe-
PPC_TOOL_PATH 	?= /opt/freescale/usr/local/gcc-4.3.74-eglibc-2.8.74-dp-2/powerpc-none-linux-gnuspe/bin:/opt/freescale/ltib/usr/bin
KERNEL_SRC      ?= $(BASEDIR)/linux/kernel/current
DATE 		?= $(shell date)
USER 		?= $(shell whoami)
HOST		?= $(shell hostname)
OUTPUTDIR   ?= $(TOPDIR)

ifeq ($(MAKELEVEL),0)
  PATH := $(PPC_TOOL_PATH):$(PATH)
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
	@echo " Emerson BlackAdder - P2020 Build "
	@echo " "
	@echo " If you are creating a new release, the appropriate"
	@echo " version numbers must be updated in the following files:"
	@echo "    versions"
	@echo " "
	@echo "++++++++++++++++++++++++++++++++++++++++++++++++++++++"
	@echo " Normal Usage Targets:"
	@echo "  all..................... typical target for SD"
	@echo "  clean................... clean the u-boot, kernel, device tree and rootfs"
	@echo "++++++++++++++++++++++++++++++++++++++++++++++++++++++"
	@echo " Targets: u-boot"
	@echo "  uboot-nor .............. config and build u-boot-nor.bin for NOR flash"
	@echo "  uboot-nand ............. config and build u-boot-nand.bin for NAND flash"
	@echo "  uboot-spi .............. config and build u-boot-spi.bin for SPI flash"
	@echo "  uboot-sd ............... config and build u-boot-sd.bin for SD Card"
	@echo "  uboot-clean............. clean the u-boot"
	@echo "++++++++++++++++++++++++++++++++++++++++++++++++++++++"
	@echo " Targets: kernel"
	@echo "  kernel-config........... config the kernel"
	@echo "  kernel.................. compile kernel with current configuration"
	@echo "  kernel-default.......... compile kernel with default configuration"
	@echo "  kernel-clean............ clean the kernel"
	@echo "++++++++++++++++++++++++++++++++++++++++++++++++++++++"
	@echo " Targets: device tree"
	@echo "  dtb..................... compile device tree binary"
	@echo "  dtb-clean............... clean the device tree binary"
	@echo "++++++++++++++++++++++++++++++++++++++++++++++++++++++"
	@echo " Targets: rootfs"
	@echo "  rootfs-min-nor.......... build min rootfs for NOR flash"
	@echo "  rootfs-min-nand......... build min rootfs for NAND flash"
	@echo "  rootfs-min-spi.......... build min rootfs for SPI flash"
	@echo "  rootfs-min-ext2......... build min rootfs for ext2"
	@echo "  rootfs-usr-nor.......... build usr rootfs for NOR flash"
	@echo "  rootfs-usr-nand......... build usr rootfs for NAND flash"
	@echo "  rootfs-usr-spi.......... build usr rootfs for SPI flash"
	@echo "  rootfs-usr-ext2......... build usr rootfs for ext2"
	@echo "  rootfs-dev-nor.......... build dev rootfs for NOR flash"
	@echo "  rootfs-dev-nand......... build dev rootfs for NAND flash"
	@echo "  rootfs-dev-spi.......... build dev rootfs for SPI flash"
	@echo "  rootfs-dev-ext2......... build dev rootfs for ext2"
	@echo "  rootfs-clean............ clean the rootfs"
	@echo "++++++++++++++++++++++++++++++++++++++++++++++++++++++"
	@echo "  current output dir = $(OUTPUTDIR)/."
	@echo "  you can set the output dir by modify the variable: OUTPUTDIR "
	@echo "      For example: export OUTPUTDIR=/tftpboot "
	@echo " "

help: message


printver: version
	@echo "++++++++++++++++++++++++++++++++++++++++++++++++++++++"
	@echo " FlexCore Build"
	@echo "    $(DATE)"
	@echo "    $(USER)@$(HOST)"
	@echo "    version:"
	@echo "       `cat version.txt`"
	@echo "    CROSS_COMPILE: $(CROSS_COMPILE)"
	@echo "    PATH         : $(PATH)"
	@echo "    PPC_TOOL_PATH: $(PPC_TOOL_PATH)"
	@echo "    KERNEL_SRC   : $(KERNEL_SRC)"
	@echo "++++++++++++++++++++++++++++++++++++++++++++++++++++++"

endmessage:
	@echo "++++++++++++++++++++++++++++++++++++++++++++++++++++++"
	@echo "  FlexCore Build - BUILD FINISHED "
	@echo "  (Until gnu make 3.80+ is used you will see mkdir"
	@echo "   errors.  These are ignored and are OK.)"
	@echo " "
	@echo " Packages are located in: $(PROMOTEDIR)"
	@echo " "
#	@echo " NOTE: nexthop items MUST be gzipped, copied, and"
#	@echo " checked in to linux/files/flash/common/app"
#	@echo " before building an image or package!"
#	@echo " "
	@echo "++++++++++++++++++++++++++++++++++++++++++++++++++++++"

all: clean uboot-sd kernel-default dtb rootfs-min-ext2
	
clean: uboot-clean kernel-clean dtb-clean rootfs-clean

uboot-nor:
	$(MAKE) -C u-boot -f Makefile uboot-nor

uboot-nand:
	$(MAKE) -C u-boot -f Makefile uboot-nand

uboot-spi:
	$(MAKE) -C u-boot -f Makefile uboot-spi

uboot-sd:
	$(MAKE) -C u-boot -f Makefile uboot-sd

uboot-clean:
	$(MAKE) -C u-boot -f Makefile clean

kernel:
	$(MAKE) -C linux/kernel -f Makefile kernel

kernel-config:
	$(MAKE) -C linux/kernel -f Makefile kernel-config

kernel-default:
	$(MAKE) -C linux/kernel -f Makefile kernel-default
	
kernel-clean:
	$(MAKE) -C linux/kernel -f Makefile kernel-clean

dtb:
	$(MAKE) -C linux/kernel -f Makefile dtb

dtb-clean:
	$(MAKE) -C linux/kernel -f Makefile dtb-clean

rootfs-min-nor:
	$(MAKE) -C linux/fs -f Makefile rootfs-min-nor

rootfs-min-nand:
	$(MAKE) -C linux/fs -f Makefile rootfs-min-nand

rootfs-min-spi:
	$(MAKE) -C linux/fs -f Makefile rootfs-min-spi

rootfs-min-ext2:
	$(MAKE) -C linux/fs -f Makefile rootfs-min-ext2

rootfs-usr-nor:
	$(MAKE) -C linux/fs -f Makefile rootfs-usr-nor

rootfs-usr-nand:
	$(MAKE) -C linux/fs -f Makefile rootfs-usr-nand

rootfs-usr-spi:
	$(MAKE) -C linux/fs -f Makefile rootfs-usr-spi

rootfs-usr-ext2:
	$(MAKE) -C linux/fs -f Makefile rootfs-usr-ext2

rootfs-dev-nor:
	$(MAKE) -C linux/fs -f Makefile rootfs-dev-nor

rootfs-dev-nand:
	$(MAKE) -C linux/fs -f Makefile rootfs-dev-nand

rootfs-dev-spi:
	$(MAKE) -C linux/fs -f Makefile rootfs-dev-spi

rootfs-dev-ext2:
	$(MAKE) -C linux/fs -f Makefile rootfs-dev-ext2

rootfs-clean:
	$(MAKE) -C linux/fs -f Makefile rootfs-clean

debug:
	@echo "TOPDIR=$(TOPDIR)"
	$(MAKE) -C u-boot -f Makefile debug
	$(MAKE) -C linux/kernel -f Makefile debug
	