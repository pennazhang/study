#!/bin/bash

# Run this script with sudo (called from install_dev)

set -e
MYDIR=`dirname "$0"`; MYDIR=`realpath "$MYDIR"`; export MYDIR

cd "$MYDIR"

# install dependencies
if [ ! -f ~/.debian_apt_done ]; then
    # development
    apt update || true
    apt-get install -y wget cpio unzip rsync gnupg2 dos2unix iputils-ping net-tools iproute2 diffstat unzip texinfo chrpath nano
    apt-get install -y git git-core  build-essential cmake autogen autoconf libtool m4 pkg-config bc device-tree-compiler libc6-i386 lib32stdc++6 lv2-c++-tools 
    apt-get install -y python socat gawk doxygen
    apt-get install -y locales && localedef -i en_US -c -f UTF-8 -A /usr/share/locale/locale.alias en_US.UTF-8
    cd /infinity_kernel
    dpkg -i  linux-headers-4.9.40-3-rt30_1.2.studer.infinity_amd64.deb
    dpkg -i  linux-image-4.9.40-3-rt30_1.2.studer.infinity_amd64.deb
    touch ~/.debian_apt_done
fi
