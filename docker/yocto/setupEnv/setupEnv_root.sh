#!/bin/bash

# Run this script with sudo (called from install_dev)

set -e
MYDIR=`dirname "$0"`; MYDIR=`realpath "$MYDIR"`; export MYDIR

cd "$MYDIR"

# install dependencies
if [ ! -f /tmp/.yocto_apt_done ]; then
    # development
    apt update || true

    #install gcc for X86
    apt install -y build-essential libsdl2-dev cmake autogen autoconf libtool m4 pkg-config bc device-tree-compiler gnupg2 libc6-i386 lib32stdc++6 lib32z1 

    #install other tools
    apt install -y python  repo
#    apt install -y gawk  git-core diffstat texinfo gcc-multilib 
#    apt install -y repo
    touch /tmp/.yocto_apt_done
fi
