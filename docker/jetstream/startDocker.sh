#!/bin/bash

# we can use the option "set -x" to do some debug.
#set -x
set -e

DOCKER_IMAGE_NAME=qt5.11.1
DOCKER_IMAGE_VERSION=0.2
HOST_NAME=" -h qt5_11_1 "

CURRENT_DIR=`dirname "$0"`; CURRENT_DIR=`realpath "$CURRENT_DIR"`
SETUP_ENV_DIR=`cd $CURRENT_DIR/..; pwd`
PROJECT_DIR=`cd $CURRENT_DIR/../..; pwd`
PROJECT_NAME=${PROJECT_DIR##*/}
USER_NAME=`whoami`
HOME_DIR=/home/$USER_NAME

#You can add you dir map here with the format : -v $MAP_DIR_IN_HOST:$MAP_DIR_IN_DOCKER
DIR_MAP=" -v $HOME_DIR:$HOME_DIR "

#Add DIR_MAP for display.
DIR_MAP+=" -v $XDG_RUNTIME_DIR:$XDG_RUNTIME_DIR "

# Add environment parameter for Display.
RUN_ENV=" -e XDG_RUNTIME_DIR "



USER_ID=`id -u`
GROUP_ID=`id -g`
USER_LOGIN="--user $USER_ID:$GROUP_ID"

#default dns is: 8.8.8.8. To find the dns, run the command in host: nmcli dev show | grep 'IP4.DNS'
#dnsMap="--dns 192.168.0.1"
DNS_Map=

#Check if the image is already exist.
imageExistFlag="$(docker images $DOCKER_IMAGE_NAME:$DOCKER_IMAGE_VERSION | grep $DOCKER_IMAGE_NAME)" || true
if [ -z "$imageExistFlag" ];then
    echo ------------------------------------------------------------------------------
    echo warning --  dokcer image: $DOCKER_IMAGE_NAME:$DOCKER_IMAGE_VERSION not exist, please build it first!
    echo ------------------------------------------------------------------------------
    exit -1
fi

# disable X-Windows server access control, clients can connect from any host
xhost + || true
export XSOCK=/tmp/.X11-unix
export XAUTH=/tmp/.docker.xauth
touch /tmp/.docker.xauth
xauth nlist $DISPLAY | sed -e 's/^..../ffff/' | xauth -f $XAUTH nmerge -

if [[ $DISPLAY =~ "localhost" ]]; then 
    echo DISPLAY=$DISPLAY
    echo "Please startDocker from Host terminal, not SSH shell."
fi	


X11_OPTION=" -v $XSOCK -v $XAUTH -e XAUTHORITY=$XAUTH -e DISPLAY --net=host "
echo "DIR_MAP=$DIR_MAP"

if [ $# -eq 0 ]; then
	echo docker run -it --rm $X11_OPTION $DIR_MAP $USER_LOGIN $RUN_ENV $HOST_NAME $DNS_Map $DOCKER_IMAGE_NAME:$DOCKER_IMAGE_VERSION bash
	docker run -it --rm $X11_OPTION $DIR_MAP $USER_LOGIN $RUN_ENV $HOST_NAME $DNS_Map $DOCKER_IMAGE_NAME:$DOCKER_IMAGE_VERSION bash
else
	echo docker run -it --rm $X11_OPTION $DIR_MAP $USER_LOGIN $RUN_ENV $HOST_NAME $DNS_Map $DOCKER_IMAGE_NAME:$DOCKER_IMAGE_VERSION sh -c \'\'"$1"\'\'
	docker run -it --rm $X11_OPTION $DIR_MAP $USER_LOGIN $RUN_ENV $HOST_NAME $DNS_Map $DOCKER_IMAGE_NAME:$DOCKER_IMAGE_VERSION sh -c \'\'"$1"\'\'
fi

