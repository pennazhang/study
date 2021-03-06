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
if [ ! -f /tmp/.done_dvi_apt ]; then
    # development
    sudo apt-get install -y git build-essential cmake autogen autoconf libtool m4 pkg-config bc device-tree-compiler python wget cpio unzip rsync gnupg2 libc6-i386 lib32stdc++6 lib32z1
    touch /tmp/.done_dvi_apt
fi

echo "setupEnv.sh is called successfully!"
