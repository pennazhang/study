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

if [ ! -f /tmp/.jetstream_p4_done ]; then
    wget -qO - https://package.perforce.com/perforce.pubkey | sudo apt-key add -
    echo 'deb http://package.perforce.com/apt/ubuntu xenial release' | sudo tee --append /etc/apt/sources.list.d/additional-repositories.list > /dev/null
    
    #we need to update apt source since we add an new source.
    sudo apt update
    
    #install P4V
    sudo apt install -y helix-cli helix-cli-base

    export P4USER=PZhang
    export P4PORT=10.90.66.25:3501

    p4 -u PZhang -p 10.90.66.25:3500 login
    p4 -u PZhang -p 10.90.66.25:3501 login
    touch /tmp/.jetstream_p4_done
fi

if [ ! -f /tmp/.jetstream_jira_done ]; then
    sudo dpkg-reconfigure dash 
    sudo apt install -y gawk wget git-core diffstat unzip texinfo gcc-multilib build-essential chrpath socat libsdl1.2-dev xterm rpm2cpio lzop liblz4-tool libssl-dev g++-multilib lib32z1-dev pv 
    sudo apt install -y python python3-pip
    sudo pip3 install jira
    sudo apt-get install -y texlive-latex-base
    sudo apt-get install -y libterm-readkey-perl intltool xalan
    touch /tmp/.jetstream_jira_done
fi
  
if [ ! -f /tmp/.jetstream_java_done ]; then    
    sudo apt-get install -y software-properties-common
    sudo add-apt-repository -y ppa:openjdk-r/ppa 
    sudo apt-get update
    sudo apt-get install -y openjdk-8-jdk
    sudo apt-get install -y libtool libterm-readkey-perl
    touch /tmp/.jetstream_java_done
fi

#mkdir -p ${HOME}/workspace/gclient || true
#cd ${HOME}/workspace/gclient
#if [ ! -d depot-tools ];then
#    git clone ssh://git@stash1.harman.com:7999/int_oss/depot-tools.git
#fi

#if [ ! -f /tmp/.jetstream_setpath_done ]; then 
#    export PATH=${HOME}/workspace/gclient/depot-tools:"$PATH"
#    export DEPOT_TOOLS_UPDATE=0
#    touch /tmp/.jetstream_setpath_done
#fi

#cd ${HOME}/workspace/gclient
#if [ ! -d va-gclient ];then
#    git clone ssh://git@bitbucket.harman.com:7999/vid/va-gclient.git
#fi

#cd va-gclient/
#./scripts/imx6/build_6ul.sh -t va-basic-tcfg

#cd $MYDIR
#sudo bash -e "$MYDIR/setupEnv_root.sh" "$1"

echo "setupEnv.sh is called successfully!"
