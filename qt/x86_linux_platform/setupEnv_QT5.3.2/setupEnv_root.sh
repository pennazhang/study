#!/bin/bash

# Run this script with sudo (called from install_dev)

set -e
MYDIR=`dirname "$0"`; MYDIR=`realpath "$MYDIR"`; export MYDIR

cd "$MYDIR"

# install dependencies
if [ ! -f ~/.qt5_apt_done ]; then
    # development
    apt-get update
    apt-get -y install iproute2 iputils-ping git 
    apt-get install -y build-essential cmake autogen autoconf libtool m4 pkg-config bc device-tree-compiler python wget cpio unzip rsync gnupg2 dos2unix qtchooser
    apt install -y libx11-dev libdbus-1-dev
    apt-get install -y locales && rm -rf /var/lib/apt/lists/* && localedef -i en_US -c -f UTF-8 -A /usr/share/locale/locale.alias en_US.UTF-8
    touch ~/.qt5_apt_done
fi
