#!/bin/bash

set -e

CURRENT_DIR=`dirname "$0"`; CURRENT_DIR=`realpath "$CURRENT_DIR"`

SOFTWARE_BASE=/git/junebox/software

export LD_LIBRARY_PATH=$SOFTWARE_BASE/cross_compiler/host/usr/lib/
export PATH=$PATH:$SOFTWARE_BASE/cross_compiler/host/usr/bin/
export CROSS_COMPILE=$SOFTWARE_BASE/cross_compiler/host/usr/bin/arm-linux-

make all
