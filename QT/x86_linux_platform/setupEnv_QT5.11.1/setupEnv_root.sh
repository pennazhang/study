#!/bin/bash

# Run this script with sudo (called from install_dev)

set -e
MYDIR=`dirname "$0"`; MYDIR=`realpath "$MYDIR"`; export MYDIR

cd "$MYDIR"

# install dependencies
if [ ! -f ~/.qt5_apt_done ]; then
    # development
    apt-get update
    apt-get install -y iproute2 iputils-ping git 
    apt-get install -y build-essential qt5-default x11-apps
    touch ~/.qt5_apt_done
fi
