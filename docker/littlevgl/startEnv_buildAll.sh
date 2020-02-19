#!/bin/bash

HOST_GIT_DIR=/mnt/disk2/git
DOCKER_IMAGE_NAME=littlevgl
DOCKER_IMAGE_VERSION=0.1
CURRENT_DIR=$(cd `dirname $0`; pwd)


imageExistFlag="$(docker images $DOCKER_IMAGE_NAME:$DOCKER_IMAGE_VERSION | grep $DOCKER_IMAGE_NAME)"
if [ -z "$imageExistFlag" ];then
	echo dokcer image: $DOCKER_IMAGE_NAME:$DOCKER_IMAGE_VERSION not exist, please build it first!
	exit 1
fi

#read  -s -p "Enter Your sudo password: "  SUDO_PASSWORD
#echo sudo password = $SUDO_PASSWORD

CHECK_WORD=$(sudo echo "checking sudo password")
if [ ! "${CHECK_WORD}" == "checking sudo password" ]; then
     echo "Invalid sudo password!"
     exit 0
fi

xhost
export XSOCK=/tmp/.X11-unix
export XAUTH=/tmp/.docker.xauth
sudo -S rm -rf /tmp/.docker.xauth
xauth nlist :0 | sed -e 's/^..../ffff/' | xauth -f $XAUTH nmerge -
#docker run -it --rm -v $XSOCK:$XSOCK -v $XAUTH:$XAUTH -e XAUTHORITY=$XAUTH --net=host --env DISPLAY=${DISPLAY#localhost} -v $HOST_GIT_DIR:/git $DOCKER_IMAGE_NAME:$DOCKER_IMAGE_VERSION 
docker run --rm -v $XSOCK:$XSOCK -v $XAUTH:$XAUTH -e XAUTHORITY=$XAUTH --net=host --env DISPLAY=${DISPLAY#localhost} -v $HOST_GIT_DIR:/git $DOCKER_IMAGE_NAME:$DOCKER_IMAGE_VERSION sh -c "cd /git/pc_simulator_sdl_eclipse && make clean && make && ./demo"
