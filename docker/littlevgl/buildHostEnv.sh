#!/bin/bash

HOST_GIT_DIR=/mnt/disk2/git
CURRENT_DIR=$(cd `dirname $0`; pwd)

#read  -s -p "Enter Your sudo password: "  SUDO_PASSWORD
#echo sudo password = $SUDO_PASSWORD

# Get the source code for gitHub
if [ ! -d $HOST_GIT_DIR/pc_simulator_sdl_eclipse ];then
    cd $HOST_GIT_DIR
    git clone --recursive https://github.com/littlevgl/pc_simulator_sdl_eclipse.git
    cd $CURRENT_DIR
fi

# Install needed package to show the GUI in the docker.
sudo -S apt install -y x11-utils

# If you install libsdl2, then you can run the demo to show the GUI in the host directly.
#sudo -S apt install -y libsdl2-2.0

