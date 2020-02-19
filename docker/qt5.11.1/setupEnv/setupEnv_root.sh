#!/bin/bash

# Run this script with sudo (called from install_dev)

set -e
MYDIR=`dirname "$0"`; MYDIR=`realpath "$MYDIR"`; export MYDIR

cd "$MYDIR"

# install dependencies
if [ ! -f ~/.qt5_apt_done ]; then
    # development
	apt-get -y install iproute2 iputils-ping build-essential qt5-default x11-apps
    touch ~/.qt5_apt_done
fi
