#!/bin/bash

# Run this script with sudo (called from install_dev)

set -e
MYDIR=`dirname "$0"`; MYDIR=`realpath "$MYDIR"`; export MYDIR

cd "$MYDIR"

# install dependencies
if [ ! -f ~/.qt4.8.7_apt_done ]; then
    # development
	sudo apt update
	echo "-----------------install git -------------------------------"
	apt-get -y install git 
	echo "-----------------install iproute2 iputils-ping -------------------------------"
	apt-get -y install iproute2 iputils-ping 
	echo "-----------------install build-essential  -------------------------------"
	apt-get -y install build-essential 
	echo "-----------------install qt4-dev-tools -------------------------------"
	apt-get -y install qt4-dev-tools 
	echo "-----------------install x11-apps -------------------------------"
	apt-get -y install x11-apps
    touch ~/.qt4.8.7_apt_done
fi
