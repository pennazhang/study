#!/bin/bash

# Run this script with sudo (called from install_dev)

set -e
MYDIR=`dirname "$0"`; MYDIR=`realpath "$MYDIR"`; export MYDIR

cd "$MYDIR"

# install dependencies
if [ ! -f ~/.qt5.6.1_apt_done ]; then
    # development
    apt update
    apt install -y git iproute2 iputils-ping x11-utils dos2unix zip unzip  x11-apps
    apt install -y build-essential cmake autogen autoconf libtool m4 pkg-config bc device-tree-compiler python wget cpio unzip rsync gnupg2 dos2unix qtchooser
    apt install -y libx11-dev libdbus-1-dev
    apt install -y locales && rm -rf /var/lib/apt/lists/* && localedef -i en_US -c -f UTF-8 -A /usr/share/locale/locale.alias en_US.UTF-8
    ln -s /git/study/bin/runCommand.sh /bin/runCommand.sh
    
    touch ~/.qt5.6.1_apt_done
fi
