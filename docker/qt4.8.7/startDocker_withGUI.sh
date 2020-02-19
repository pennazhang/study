#!/bin/bash

set -e

DOCKER_IMAGE_NAME=qt4.8.7
DOCKER_IMAGE_VERSION=0.2

HOST_GIT_DIR=/home/share/pzhang/git
DOCKER_GIT_DIR=/git

if [ ! -d $HOST_GIT_DIR ]; then
    echo "$HOST_GIT_DIR not Exist."
    exit -1
fi

# The following PARMA is used to start docker images.
#RUN_ENV="-e QT_SELECT=junebox"
#HOST_NAME="-h juneBox_docker"
DIR_MAP="-v $HOST_GIT_DIR:/$DOCKER_GIT_DIR"
USER_ID=`id -u`
GROUP_ID=`id -g`
USER_LOGIN="--user $USER_ID:$GROUP_ID"

imageExistFlag="$(docker images $DOCKER_IMAGE_NAME:$DOCKER_IMAGE_VERSION | grep $DOCKER_IMAGE_NAME)" || true
if [ -z "$imageExistFlag" ];then
    echo dokcer image: $DOCKER_IMAGE_NAME:$DOCKER_IMAGE_VERSION not exist, please build it first!
    exit -1
fi

xhost
export XSOCK=/tmp/.X11-unix
export XAUTH=/tmp/.docker.xauth
sudo rm -rf /tmp/.docker.xauth
xauth nlist $DISPLAY | sed -e 's/^..../ffff/' | xauth -f $XAUTH nmerge -
X11_OPTION="-v $XSOCK:$XSOCK -v $XAUTH:$XAUTH -e XAUTHORITY=$XAUTH --env DISPLAY=${DISPLAY#localhost} --env QT_X11_NO_MITSHM=1 "
echo "X11_OPTION=$X11_OPTION"
echo "DIR_MAP=$DIR_MAP"

echo "Folder map : $DIR_MAP" 
echo "---------------------------------------------------------------------------"
echo "To build demoGui, run the following commands:"
echo "    cd /git/docker/qt4.8.7/setupEnv/demoGui"
echo "    qmake && make"
docker run -it --rm $X11_OPTION $DIR_MAP $USER_LOGIN $RUN_ENV $HOST_NAME $DNS_Map $DOCKER_IMAGE_NAME:$DOCKER_IMAGE_VERSION bash
#docker run    --rm $X11_OPTION $DIR_MAP $USER_LOGIN $RUN_ENV $HOST_NAME $DNS_Map $DOCKER_IMAGE_NAME:$DOCKER_IMAGE_VERSION sh -c "cd /git/docker/qt4.8.7/setupEnv/demoGui && qmake && make && ./demoGui"

