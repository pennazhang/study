#!/bin/bash

set -e

HOST_GIT_DIR=/home/share/pzhang/git
DOCKER_IMAGE_NAME=junebox
DOCKER_IMAGE_VERSION=0.2

# The following PARMA is used to start docker images.
DIR_MAP="-v $HOST_GIT_DIR:/git"
USER_ID=`id -u`
GROUP_ID=`id -g`
USER_LOGIN="--user $USER_ID:$GROUP_ID"
HOST_NAME="-h juneBox_builder"

#default dns is: 8.8.8.8. To find the dns, run the command in host: nmcli dev show | grep 'IP4.DNS'
#dnsMap="--dns 192.168.0.1"
DNS_Map=

imageExistFlag="$(docker images $DOCKER_IMAGE_NAME:$DOCKER_IMAGE_VERSION | grep $DOCKER_IMAGE_NAME)"
if [ -z "$imageExistFlag" ];then
	echo dokcer image: $DOCKER_IMAGE_NAME:$DOCKER_IMAGE_VERSION not exist, please build it first!
	exit 1
fi

#read  -s -p "Enter Your sudo password: "  SUDO_PASSWORD
#echo sudo password = $SUDO_PASSWORD

#CHECK_WORD=$(sudo echo "checking sudo password")
#if [ ! "${CHECK_WORD}" == "checking sudo password" ]; then
#     echo "Invalid sudo password!"
#     exit 0
#fi

#echo "To build junebox, run the following commands:"
#echo "    cd /git/junebox/software"
#echo "    ./makeAll.sh"
#docker run -it --rm $DIR_MAP $USER_LOGIN $HOST_NAME $DNS_Map $DOCKER_IMAGE_NAME:$DOCKER_IMAGE_VERSION bash
docker run --rm $DIR_MAP $USER_ID $HOST_NAME $DNS_Map $DOCKER_IMAGE_NAME:$DOCKER_IMAGE_VERSION sh -c "cd /git/junebox/software && ./makeAll.sh"


