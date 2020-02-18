#!/bin/bash

set -e

DOCKER_IMAGE_NAME=qt4.8.7
DOCKER_IMAGE_VERSION=0.2

CURRENT_DIR=`dirname "$0"`; CURRENT_DIR=`realpath "$CURRENT_DIR"`
SETUP_ENV_DIR=`cd $CURRENT_DIR/..; pwd`
PROJECT_DIR=`cd $CURRENT_DIR/../..; pwd`
PROJECT_NAME=${PROJECT_DIR##*/}
HOST_GIT_DIR=`cd $CURRENT_DIR/../../..; pwd`; 

echo "CURRENT_DIR = $CURRENT_DIR"
echo "PROJECT_NAME = $PROJECT_NAME"
echo "HOST_GIT_DIR = $HOST_GIT_DIR"

# The following PARMA is used to start docker images.
#RUN_ENV="-e QT_SELECT=junebox"
#HOST_NAME="-h juneBox_docker"
DIR_MAP="-v $HOST_GIT_DIR:/git"
USER_ID=`id -u`
GROUP_ID=`id -g`
USER_LOGIN="--user $USER_ID:$GROUP_ID"

#default dns is: 8.8.8.8. To find the dns, run the command in host: nmcli dev show | grep 'IP4.DNS'
#dnsMap="--dns 192.168.0.1"
DNS_Map=

imageExistFlag="$(docker images $DOCKER_IMAGE_NAME:$DOCKER_IMAGE_VERSION | grep $DOCKER_IMAGE_NAME)"
if [ -z "$imageExistFlag" ];then
    echo dokcer image: $DOCKER_IMAGE_NAME:$DOCKER_IMAGE_VERSION not exist, please build it first!
    exit -1
fi

sudo apt install -y x11-utils
xhost
export XSOCK=/tmp/.X11-unix
export XAUTH=/tmp/.docker.xauth
sudo rm -rf /tmp/.docker.xauth
xauth nlist $DISPLAY | sed -e 's/^..../ffff/' | xauth -f $XAUTH nmerge -
X11_OPTION="-v $XSOCK:$XSOCK -v $XAUTH:$XAUTH -e XAUTHORITY=$XAUTH --env DISPLAY=${DISPLAY#localhost} --env QT_X11_NO_MITSHM=1 "
echo "X11_OPTION=$X11_OPTION"

echo "To build demoGui, run the following commands:"
echo "    cd /git/demoGui/src"
echo "    qmake && make"
docker run -it --rm $X11_OPTION $DIR_MAP $USER_LOGIN $RUN_ENV $HOST_NAME $DNS_Map $DOCKER_IMAGE_NAME:$DOCKER_IMAGE_VERSION bash
#docker run --rm $X11_OPTION $DIR_MAP $USER_LOGIN $RUN_ENV $HOST_NAME $DNS_Map $DOCKER_IMAGE_NAME:$DOCKER_IMAGE_VERSION sh -c "cd /git/junebox/software && ./makeAll.sh"


