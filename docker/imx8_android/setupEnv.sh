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

# install dependencies
if [ ! -f /tmp/.done_imx8_android_apt ]; then
    sudo apt-get install -y git build-essential cmake autogen autoconf libtool m4 pkg-config bc device-tree-compiler python wget cpio unzip rsync gnupg2 libc6-i386 lib32stdc++6 lib32z1
    sudo apt install -y uuid uuid-dev zlib1g-dev liblz-dev liblzo2-2 liblzo2-dev \
 	lzop  git curl  u-boot-tools mtd-utils android-sdk-libsparse-utils android-sdk-ext4-utils \
 	device-tree-compiler gdisk m4 zlib1g-dev bison flex make libssl-dev gcc-multilib
    sudo apt install -y openjdk-8-jdk


    cd $DOCKER_CACHE_DIR
    sudo tar -xvJf gcc-arm-8.3-2019.03-x86_64-aarch64-elf.tar.xz -C /opt

    sudo git clone https://android.googlesource.com/platform/prebuilts/clang/host/linux-x86 /opt/prebuilt-android-clang -b master
    cd /opt/prebuilt-android-clang
    sudo git checkout 007c96f100c5322acc37b84669c032c0121e68d0
    
    touch /tmp/.done_imx8_android_apt
fi

echo "setupEnv.sh is called successfully!"
