#!/bin/bash
# script to create all Linux and boot binaries
set -e

#set env
SOFTWARE_BASE=`pwd`

###############################################################################
# build - Model
###############################################################################
rm -rf ./out
export QT_SELECT=junebox
cd project
qmake Panel.pro
make
cd $SOFTWARE_BASE
ls -al ./out/bin/Panel

