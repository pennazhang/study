#!/bin/bash

set -e

DOCKER_IMAGE_NAME=littlevgl
DOCKER_IMAGE_VERSION=0.2

HOST_GIT_DIR=/git

if [ ! -d $HOST_GIT_DIR ]; then
    echo "$HOST_GIT_DIR not Exist."
    exit -1
fi

# The following PARMA is used to start docker images.
RUN_ENV="-e XDG_RUNTIME_DIR"
HOST_NAME="-h little_vgl"
DIR_MAP="-v $HOST_GIT_DIR:/git -v $XDG_RUNTIME_DIR:$XDG_RUNTIME_DIR"
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

# disable X-Windows server access control, clients can connect from any host
xhost +
export XSOCK=/tmp/.X11-unix
export XAUTH=/tmp/.docker.xauth
touch /tmp/.docker.xauth
xauth nlist $DISPLAY | sed -e 's/^..../ffff/' | xauth -f $XAUTH nmerge -

if [[ $DISPLAY =~ "localhost" ]]; then 
    echo "Please startDocker from Host terminal, not SSH shell."
    exit -1
else
    X11_OPTION="-v $XSOCK:$XSOCK -v $XAUTH:$XAUTH -e XAUTHORITY=$XAUTH -e DISPLAY"
fi	


#X11_OPTION="-v $XSOCK -v $XAUTH -e XAUTHORITY=$XAUTH -e DISPLAY --net=host"
#echo "X11_OPTION=$X11_OPTION"
#echo "DIR_MAP=$DIR_MAP"

#echo "Folder map : $DIR_MAP" 
#echo "---------------------------------------------------------------------------"
#echo "To build littlevgl, run the following commands:"
#echo "    cd /git"
#echo "    git clone --recursive https://github.com/littlevgl/pc_simulator_sdl_eclipse.git"
#echo "    cd pc_simulator_sdl_eclipse"
#echo "    make clean && make"
#echo "    ./demo"

if [ $# -eq 0 ]; then
	echo docker run -it --rm $X11_OPTION $DIR_MAP $USER_LOGIN $RUN_ENV $HOST_NAME $DNS_Map $DOCKER_IMAGE_NAME:$DOCKER_IMAGE_VERSION bash
	docker run -it --rm $X11_OPTION $DIR_MAP $USER_LOGIN $RUN_ENV $HOST_NAME $DNS_Map $DOCKER_IMAGE_NAME:$DOCKER_IMAGE_VERSION bash
else
	echo docker run -it --rm $X11_OPTION $DIR_MAP $USER_LOGIN $RUN_ENV $HOST_NAME $DNS_Map $DOCKER_IMAGE_NAME:$DOCKER_IMAGE_VERSION sh -c \'\'"$1"\'\'
	docker run -it --rm $X11_OPTION $DIR_MAP $USER_LOGIN $RUN_ENV $HOST_NAME $DNS_Map $DOCKER_IMAGE_NAME:$DOCKER_IMAGE_VERSION sh -c \'\'"$1"\'\'
fi

