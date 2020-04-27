#!/bin/bash

# Run this script with sudo (called from install_dev)

set -e
MYDIR=`dirname "$0"`; MYDIR=`realpath "$MYDIR"`; export MYDIR

cd "$MYDIR"

# install dependencies
if [ ! -f ~/.qt4.8.7_apt_done ]; then
    # development
    apt-get update
	sudo apt install -y git iproute2 iputils-ping x11-utils dos2unix zip unzip	apt-get install -y build-essential qt4-dev-tools 
    touch ~/.qt4.8.7_apt_done
fi
