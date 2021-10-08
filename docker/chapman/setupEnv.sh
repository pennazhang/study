#!/bin/bash

set -e

MYDIR=`dirname "$0"`; MYDIR=`realpath "$MYDIR"`; export MYDIR
PROJ_DIR=`cd $MYDIR/..; pwd`; export PROJ_DIR
echo MYDIR=$MYDIR
echo PROJ_DIR=$PROJ_DIR

# We need to get DOCKER_CACHE_DIR from docker.conf.
# since we don't need DOCKER_CACHE_DIR, we didn't need to source docker.conf
#source docker.conf
#echo DOCKER_CACHE_DIR=$DOCKER_CACHE_DIR

# DOCKER_CACHE_DIR must not be NULL in this project.
#if [ ! -d $DOCKER_CACHE_DIR ];then 
#    echo "can't find the DOCKER_CACHE_DIR:$DOCKER_CACHE_DIR "
#    exit -1
#fi

cd $MYDIR
sudo apt update
#sudo bash -e "$MYDIR/setupEnv_root.sh" "$1"

# install dependencies
if [ ! -f /tmp/.done_chapman_apt ]; then
    # development
    sudo apt-get install -y git build-essential cmake autogen autoconf libtool m4 pkg-config bc device-tree-compiler python wget cpio unzip rsync gnupg2 libc6-i386 lib32stdc++6 lib32z1
    sudo apt-get install -y software-properties-common
	
	#install jdk-11
##   sudo apt-get install -y default-jdk
#	sudo add-apt-repository ppa:openjdk-r/ppa
#	sudo apt-get update
	sudo apt install -y openjdk-11-jdk

	#install android studio
    sudo add-apt-repository -y ppa:maarten-fonville/android-studio
    sudo apt-get update
    sudo apt-get install -y android-studio

    sudo apt-get install -y qemu-kvm curl libncurses5
	curl -s "https://get.sdkman.io" | bash
	source $HOME/.sdkman/bin/sdkman-init.sh
	sdk install gradle 7.1.1
	sdk use gradle 7.1.1
##    sudo adduser pzhang kvm
    touch /tmp/.done_chapman_apt
fi

echo "setupEnv.sh is called successfully!"
