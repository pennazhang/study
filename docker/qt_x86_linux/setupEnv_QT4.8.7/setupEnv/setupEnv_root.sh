#!/bin/bash

# Run this script with sudo (called from install_dev)

set -e
MYDIR=`dirname "$0"`; MYDIR=`realpath "$MYDIR"`; export MYDIR

cd "$MYDIR"

# install dependencies
if [ ! -f /tmp/.qt4.8.7_apt_done ]; then
    apt update || true

    #install gcc for X86
    apt install -y build-essential libsdl2-dev

    #install qt4
    apt install -y qt4-dev-tools 
    
    # gnome-calculator is only available for Ubuntu 18.04 or above, and it just used to check the GUI in docker.
    apt install -y gnome-calculator
    
    touch /tmp/.qt4.8.7_apt_done
fi
