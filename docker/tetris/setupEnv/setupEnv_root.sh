#!/bin/bash

# Run this script with sudo (called from install_dev)

set -e
MYDIR=`dirname "$0"`; MYDIR=`realpath "$MYDIR"`; export MYDIR

cd "$MYDIR"

# install dependencies
if [ ! -f ~/.tetris_apt_done ]; then
    # development
    apt update || true
    apt install -y git iproute2 iputils-ping x11-utils dos2unix zip unzip  x11-apps wget cpio dos2unix iputils-ping net-tools iproute2 nano rsync
    apt-get install -y locales && localedef -i en_US -c -f UTF-8 -A /usr/share/locale/locale.alias en_US.UTF-8
    touch ~/.tetris_apt_done
fi
