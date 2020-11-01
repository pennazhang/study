#!/bin/bash

set -e

MYDIR=`dirname "$0"`; MYDIR=`realpath "$MYDIR"`; export MYDIR
PROJ_DIR=`cd $MYDIR/..; pwd`; export PROJ_DIR

# We need to get DOCKER_CACHE_DIR from docker.conf.
source docker.conf

echo MYDIR=$MYDIR
echo PROJ_DIR=$PROJ_DIR
echo DOCKER_CACHE_DIR=$DOCKER_CACHE_DIR

# DOCKER_CACHE_DIR must not be NULL in this project.
#if [ ! -d $DOCKER_CACHE_DIR ];then 
#	echo "can't find the DOCKER_CACHE_DIR:$DOCKER_CACHE_DIR "
#	exit -1
#fi

cd $MYDIR
sudo apt update
#sudo bash -e "$MYDIR/setupEnv_root.sh" "$1"

if [ ! -f /tmp/.qt4.8.7_apt_done ]; then
    #install gcc for X86
    sudo apt install -y build-essential libsdl2-dev

    #install qt4
    sudo apt install -y qt4-dev-tools 
    
    # gnome-calculator can be used to check the GUI in docker.
    sudo apt install -y gnome-calculator
    
    touch /tmp/.qt4.8.7_apt_done
fi

echo "setupEnv.sh is called successfully!"
