#!/bin/bash

# Run this script with sudo (called from install_dev)

set -e
MYDIR=`dirname "$0"`; MYDIR=`realpath "$MYDIR"`; export MYDIR

cd "$MYDIR"

# install dependencies
if [ ! -f ~/.yocto_apt_done ]; then
    # development
    apt update || true
    apt install -y git iproute2 iputils-ping x11-utils dos2unix zip unzip  x11-apps wget cpio dos2unix iputils-ping net-tools iproute2 nano rsync
    apt install -y build-essential cmake autogen autoconf libtool m4 pkg-config bc device-tree-compiler gnupg2 libc6-i386 lib32stdc++6 lib32z1 
    apt install -y python  
    apt install -y gawk  git-core diffstat texinfo gcc-multilib build-essential chrpath socat libsdl1.2-dev
    apt install -y repo
    touch ~/.yocto_apt_done
fi
