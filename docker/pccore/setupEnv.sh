#!/bin/bash

set -e

MYDIR=`dirname "$0"`; MYDIR=`realpath "$MYDIR"`; export MYDIR
PROJ_DIR=`cd $MYDIR/..; pwd`; export PROJ_DIR
echo MYDIR=$MYDIR
echo PROJ_DIR=$PROJ_DIR

# We need to get DOCKER_CACHE_DIR from docker.conf.
source docker.conf
echo DOCKER_CACHE_DIR=$DOCKER_CACHE_DIR

# DOCKER_CACHE_DIR must not be NULL in this project.
if [ ! -d $DOCKER_CACHE_DIR ];then 
    echo "can't find the DOCKER_CACHE_DIR:$DOCKER_CACHE_DIR "
    exit -1
fi

cd $MYDIR
sudo apt update
#sudo bash -e "$MYDIR/setupEnv_root.sh" "$1"

if [ ! -f ~/.debian_apt_done ]; then
    # common install
    sudo apt-get install -y wget cpio unzip rsync gnupg2 dos2unix iputils-ping net-tools iproute2 diffstat unzip texinfo chrpath nano
    sudo apt-get install -y git git-core  build-essential cmake autogen autoconf libtool m4 pkg-config bc device-tree-compiler libc6-i386 lib32stdc++6 lv2-c++-tools 
    sudo apt-get install -y python socat gawk doxygen
    sudo apt-get install -y locales && localedef -i en_US -c -f UTF-8 -A /usr/share/locale/locale.alias en_US.UTF-8
    
    cd $DOCKER_CACHE_DIR
    dpkg -i  linux-headers-4.9.40-3-rt30_1.2.studer.infinity_amd64.deb
    dpkg -i  linux-image-4.9.40-3-rt30_1.2.studer.infinity_amd64.deb
    touch ~/.debian_apt_done
fi