#!/bin/bash

set -e

CURRENT_DIR=`dirname "$0"`; CURRENT_DIR=`realpath "$CURRENT_DIR"`

#for junebox Atmel ARM9G25.
export SOFTWARE_BASE=/git/junebox/software
export LD_LIBRARY_PATH=$SOFTWARE_BASE/cross_compiler/host/usr/lib/
export PATH=$PATH:$SOFTWARE_BASE/cross_compiler/host/usr/bin/
export CROSS_COMPILE=$SOFTWARE_BASE/cross_compiler/host/usr/bin/arm-linux-

#for gcc-arm-8.3-2019.03-x86_64-arm-linux-gnueabihf
#export SOFTWARE_BASE=/git/gcc-arm-8.3-2019.03-x86_64-arm-linux-gnueabihf
#export LD_LIBRARY_PATH=$SOFTWARE_BASE/arm-linux-gnueabihf/lib
#export PATH=$PATH:$SOFTWARE_BASE/bin/
#export CROSS_COMPILE=arm-linux-gnueabihf-

#for /git/gcc-linaro-5.5.0-2017.10-x86_64_arm-linux-gnueabihf
#export SOFTWARE_BASE=/git/gcc-linaro-5.5.0-2017.10-x86_64_arm-linux-gnueabihf
#export LD_LIBRARY_PATH=$SOFTWARE_BASE/arm-linux-gnueabihf/lib
#export PATH=$PATH:$SOFTWARE_BASE/bin/
#export CROSS_COMPILE=arm-linux-gnueabihf-

make all
