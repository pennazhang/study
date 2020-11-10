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
#	echo "can't find the DOCKER_CACHE_DIR:$DOCKER_CACHE_DIR "
#	exit -1
#fi

cd $MYDIR
sudo apt update
#sudo bash -e "$MYDIR/setupEnv_root.sh" "$1"

if [ ! -f /tmp/.yocto_apt_done ]; then

    sudo apt install -y build-essential libsdl2-dev cmake autogen autoconf libtool m4 pkg-config bc device-tree-compiler gnupg2 libc6-i386 lib32stdc++6 lib32z1 
    sudo apt install -y  gawk wget diffstat unzip texinfo gcc-multilib 
    sudo apt install -y build-essential chrpath socat cpio python python3 python3-pip python3-pexpect chrpath gawk texinfo libsdl1.2-dev whiptail diffstat cpio libssl-dev
    sudo apt install -y xz-utils debianutils iputils-ping python3-git python3-jinja2 libegl1-mesa libsdl1.2-dev xterm
    sudo apt install -y python repo curl 
    sudo apt install -y libncurses5-dev libncursesw5-dev bison flex bc pv jq tmux
    touch /tmp/.yocto_apt_done
fi

mkdir /tmp/bin || true
curl https://storage.googleapis.com/git-repo-downloads/repo > /tmp/bin/repo
chmod a+x /tmp/bin/repo

echo "setupEnv.sh is called successfully!"
