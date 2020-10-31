#!/bin/bash

set -e

DOCKER_IMAGE_NAME=jetstream
LINUX_VERSION=ubuntu:18.04

#If you don't want to run setupEnv.sh automatically, you need to set DEBUG_FLAG=1
DEBUG_FLAG=1
#HOST_NAME is host name in the building container,  only available when DEBUG_FLAG=1
HOST_NAME=" -h jetstream_debug "

#For some software that need to be mapped into docker container, we can use "-v HOST_CACHE_DIR:/cacheData"
#HOST_CACHE_DIR=`cd ~/workspace/cacheData; pwd`
if [ ! -z $HOST_CACHE_DIR ];then 
    if [ ! -d $HOST_CACHE_DIR ];then
    	#    mkdir -p $HOST_CACHE_DIR
	echo "Can not find HOST_CACHE_DIR: $HOST_CACHE_DIR, please create it first!"
	exit 0
    else 
        FOLDER_MAP=" -v $HOST_CACHE_DIR:/cacheData "
    fi
fi

CURRENT_DIR=`dirname "$0"`; CURRENT_DIR=`realpath "$CURRENT_DIR"`
PROJECT_DIR=$CURRENT_DIR
PROJECT_NAME=${PROJECT_DIR##*/}
HOST_GIT_DIR=`cd $PROJECT_DIR/..; pwd`; 

echo "PROJECT_NAME = $PROJECT_NAME"

#Auto run script in Docker to build environment
AUTO_START_DIR=/git/$PROJECT_NAME/setupEnv
AUTO_START_SCRIPT=setupEnv.sh
echo "auto run in Docker: cd $AUTO_START_DIR && ./$AUTO_START_SCRIPT"

if [ -z $AUTO_START_DIR ] || [ -z $AUTO_START_SCRIPT ];then
    echo "Error : Please define AUTO_START_DIR and AUTO_START_SCRIPT to setup the building environment"
    exit -1
fi

# install docker
DOCKER_VERSION="$(docker --version | grep $Docker version)"
if [ -z "$DOCKER_VERSION" ];then
    echo please install the package: docker first
    exit -1
fi

# Get the source code from git repo
#if [ ! -d $HOST_GIT_DIR/$PROJECT_NAME ];then
#    cd $HOST_GIT_DIR
#    git clone --recursive $GIT_REPO
#    cd $CURRENT_DIR
#fi

#if [ ! -d $HOST_GIT_DIR/$PROJECT_NAME ];then
#    echo "No souce code was found in $HOST_GIT_DIR/$PROJECT_NAME"
#    echo "Please download the code from: $GIT_REPO!"
#    exit -1
#fi

#-----------------------------------------------------------------------------
# install package on host
#-----------------------------------------------------------------------------
sudo apt update || true
sudo apt install -y x11-utils

#-----------------------------------------------------------------------------
# create docker_image:0.1
#-----------------------------------------------------------------------------
# We will create a user in the docker with the following param:
USER_NAME=`whoami`
USER_ID=`id -u`
GROUP_ID=`id -g`
PASSWORD="12345678"
HOME_DIR=/home/$USER_NAME

createDockerFile (){
    rm -rf $2
    cp -f $1 $2
    sed -i "s/LINUX:VERSION/$LINUX_VERSION/g" $2
    sed -i "s/USER_NAME/$USER_NAME/g" $2
    sed -i "s/USER_ID/$USER_ID/g" $2
    sed -i "s/GROUP_ID/$GROUP_ID/g" $2
    sed -i "s/PASSWORD/$PASSWORD/g" $2
}

#Check if the image is already exist.
imageExistFlag="$(docker images $DOCKER_IMAGE_NAME:0.1 | grep $DOCKER_IMAGE_NAME)" || true
if [ -z "$imageExistFlag" ];then
    echo ------------------------------------------------------------------------------
    echo --  building $DOCKER_IMAGE_NAME:0.1
    echo ------------------------------------------------------------------------------
    #Build the docker image.
    
    createDockerFile Dockerfile Dockerfile.tmp  $USER_NAME $USER_ID  $GROUP_ID $PASSWORD
    docker build -f Dockerfile.tmp -t $DOCKER_IMAGE_NAME:0.1 .
    echo build $DOCKER_IMAGE_NAME:0.1 OK!
    rm Dockerfile.tmp
fi

#-----------------------------------------------------------------------------
# create docker_image:0.2
#-----------------------------------------------------------------------------
FOLDER_MAP+=" -v $HOME_DIR:$HOME_DIR -v $HOST_GIT_DIR:/git "
TEMP_CONTAINER_NAME="$(date +%s)"
echo "FOLDER_MAP = $FOLDER_MAP"

#Check if the image is already exist.
imageExistFlag="$(docker images $DOCKER_IMAGE_NAME:0.2 | grep $DOCKER_IMAGE_NAME)" || true
if [ -z "$imageExistFlag" ]; then
    echo ------------------------------------------------------------------------------
    echo --  building $DOCKER_IMAGE_NAME:0.2                                   --
    echo ------------------------------------------------------------------------------
    echo "auto run in Docker: cd $AUTO_START_DIR && ./$AUTO_START_SCRIPT"
    echo ""

    if [ $DEBUG_FLAG == 1 ]; then
         docker run -it ${FOLDER_MAP} $HOST_NAME --name "${TEMP_CONTAINER_NAME}" ${DOCKER_IMAGE_NAME}:0.1 bash
	 exit 0
    else
        docker run ${FOLDER_MAP} --name "${TEMP_CONTAINER_NAME}" ${DOCKER_IMAGE_NAME}:0.1 sh -c "cd $AUTO_START_DIR && ./$AUTO_START_SCRIPT"
    fi
    
    #Save the container to docker image and delete the container.
    docker commit -m "build ${DOCKER_IMAGE_NAME}:0.2" ${TEMP_CONTAINER_NAME} $DOCKER_IMAGE_NAME:0.2
    docker rm ${TEMP_CONTAINER_NAME}
    echo build $DOCKER_IMAGE_NAME:0.2 OK!
fi

