#!/bin/bash

DOCKER_IMAGE_NAME=littlevgl
DOCKER_IMAGE_VERSION=0.1
CURRENT_DIR=$(cd `dirname $0`; pwd)

#read  -s -p "Enter Your sudo password: "  SUDO_PASSWORD
#echo sudo password = $SUDO_PASSWORD

CHECK_WORD=$(sudo echo "checking sudo password")
if [ ! "${CHECK_WORD}" == "checking sudo password" ]; then
     echo "Invalid sudo password!"
     exit 0
fi

#Check if the image is already exist.
imageExistFlag="$(docker images $DOCKER_IMAGE_NAME:$DOCKER_IMAGE_VERSION | grep $DOCKER_IMAGE_NAME)"
if [ ! -z "$imageExistFlag" ];then
	echo dokcer image: $DOCKER_IMAGE_NAME:$DOCKER_IMAGE_VERSION exist!
	exit 1
fi

# Setup build environment on Host.
./buildHostEnv.sh

#Build the docker image.
docker build -t $DOCKER_IMAGE_NAME:$DOCKER_IMAGE_VERSION buildEnv

echo Build image:$DOCKER_IMAGE_NAME:$DOCKER_IMAGE_VERSION OK!
exit 1


