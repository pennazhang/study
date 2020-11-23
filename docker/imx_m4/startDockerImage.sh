#!/bin/bash

set -e

CURRENT_DIR=`dirname "$0"`; CURRENT_DIR=`realpath "$CURRENT_DIR"`
USER_NAME=`whoami`
HOME_DIR=/home/$USER_NAME

# if there is only one parameter, that must be the script file name.
if [ $# -eq 1 ]; then
    if [ ! -f ${CURRENT_DIR}/$1 ]; then
    	echo "File: ${CURRENT_DIR}/$1 doesn't exist!"
	exit -1
    fi
fi

# It must be wrong when the parameter number is more than one.
if [ $# -gt 1 ]; then
    echo "Invalid parameter number."
    echo "    Usage: ./startDockerImage autoRunScript.sh"
    exit -2
fi

FOLDER_MAP="-v ${CURRENT_DIR}:/bin/docker "
source docker.conf
FOLDER_MAP+=$START_DOCKER_FOLDER_MAP
HOST_NAME=" -h ${DOCKER_RUN_NAME} "

SETUP_ENV_DIR=`cd $CURRENT_DIR/..; pwd`
PROJECT_DIR=`cd $CURRENT_DIR/../..; pwd`
PROJECT_NAME=${PROJECT_DIR##*/}

#You can add you dir map here with the format : -v $MAP_DIR_IN_HOST:$MAP_DIR_IN_DOCKER
FOLDER_MAP+=" -v $HOME_DIR:$HOME_DIR "

if [ ! $USE_GUI_IN_DOCKER == 0 ]; then
    #We need to use GUI in docker.

    #Add FOLDER_MAP for display.
    FOLDER_MAP+=" -v $XDG_RUNTIME_DIR:$XDG_RUNTIME_DIR "
    
    # Add environment parameter for Display.
    RUN_ENV=" -e XDG_RUNTIME_DIR "

    #Add Fond map
    FOLDER_MAP+=" -v /usr/share/fonts:/usr/share/fonts"
    
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

    X11_OPTION="-v $XSOCK:$XSOCK -v $XAUTH:$XAUTH -e XAUTHORITY=$XAUTH -e DISPLAY --net=host --env QT_X11_NO_MITSHM=1 "
fi

USER_ID=`id -u`
GROUP_ID=`id -g`
USER_LOGIN="--user $USER_ID:$GROUP_ID"



#Check if the image is already exist.
imageExistFlag="$(docker images $DOCKER_IMAGE_NAME:0.2 | grep $DOCKER_IMAGE_NAME)" || true
if [ -z "$imageExistFlag" ];then
    echo ------------------------------------------------------------------------------
    echo warning --  dokcer image: $DOCKER_IMAGE_NAME:0.2 not exist, please build it first!
    echo ------------------------------------------------------------------------------
    exit -1
fi

#Add some additional environment for different product.
RUN_ENV+=$ADDITIONAL_ENV

if [ $# -eq 0 ]; then
    echo docker run -it --rm $X11_OPTION $FOLDER_MAP $USER_LOGIN $RUN_ENV $HOST_NAME $DNS_Map $PORT_MAP $DOCKER_IMAGE_NAME:0.2 bash -c 'source /bin/docker/autoRunInDocker.sh; bash'
    docker run -it --rm $X11_OPTION $FOLDER_MAP $USER_LOGIN $RUN_ENV $HOST_NAME $DNS_Map $PORT_MAP $DOCKER_IMAGE_NAME:0.2 bash -c 'source /bin/docker/autoRunInDocker.sh; bash'
else
    echo docker run -it --rm $X11_OPTION $FOLDER_MAP $USER_LOGIN $RUN_ENV $HOST_NAME $DNS_Map $PORT_MAP $DOCKER_IMAGE_NAME:0.2 bash -c "source /bin/docker/autoRunInDocker.sh; source /bin/docker/$1; bash"
    docker run -it --rm $X11_OPTION $FOLDER_MAP $USER_LOGIN $RUN_ENV $HOST_NAME $DNS_Map $PORT_MAP $DOCKER_IMAGE_NAME:0.2 bash -c "source /bin/docker/autoRunInDocker.sh && source /bin/docker/$1; bash"
fi

