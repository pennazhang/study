#!/bin/bash

set -e


# -------------------------------------------------------------------------------------------------------------
# common header
# -------------------------------------------------------------------------------------------------------------
CURRENT_DIR=`dirname "$0"`; CURRENT_DIR=`realpath "$CURRENT_DIR"`
USER_NAME=`whoami`
HOME_DIR=/home/$USER_NAME

# if there is only one parameter, that must be the script file name.
if [ $# -eq 1 ]; then
    if [ ! -f ${CURRENT_DIR}/$1 ]; then
    	echo "File: ${CURRENT_DIR}/$1 doesn't exist!"
	exit -1
    fi
    export EXTRA_SCRIPT=$1
fi

# It must be wrong when the parameter number is more than one.
if [ $# -gt 1 ]; then
    echo "Invalid parameter number."
    echo "    Usage: $0 autoRunScript.sh"
    exit -2
fi

# -------------------------------------------------------------------------------------------------------------
# The following setting is only for starting docker. (startDockerImage)
# -------------------------------------------------------------------------------------------------------------

source docker.conf
export DOCKER_RUN_NAME=yocto_5.4

# -------------------------------------------------------------------------------------------------------------
# Everysetting is ready, we can start docker image now. 
# -------------------------------------------------------------------------------------------------------------
cd $CURRENT_DIR/../..
./startProjectImage $CURRENT_DIR