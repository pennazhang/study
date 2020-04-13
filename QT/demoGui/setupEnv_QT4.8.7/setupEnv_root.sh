#!/bin/bash

# Run this script with sudo (called from install_dev)

set -e
MYDIR=`dirname "$0"`; MYDIR=`realpath "$MYDIR"`; export MYDIR

cd "$MYDIR"

# install dependencies
if [ ! -f ~/.qt4.8.7_apt_done ]; then
    # development
	apt-get -y install iproute2 iputils-ping build-essential qt4-dev-tools x11-apps
    touch ~/.qt4.8.7_apt_done
fi
