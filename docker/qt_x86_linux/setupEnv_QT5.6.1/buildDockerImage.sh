#!/bin/bash

set -e

# -------------------------------------------------------------------------------------------------------------
# common header
# -------------------------------------------------------------------------------------------------------------
CURRENT_DIR=`dirname "$0"`; CURRENT_DIR=`realpath "$CURRENT_DIR"`
USER_NAME=`whoami`
HOME_DIR=/home/$USER_NAME

source docker.conf

# -------------------------------------------------------------------------------------------------------------
# The following setting is only for building docker. (buildDockerImage.sh)
# -------------------------------------------------------------------------------------------------------------

#If you want to run setupEnv.sh automatically, you need to set BUILD_DEBUG_FLAG=0, and then the new docker image can be saved.
#If you don't want to run setupEnv.sh automatically, you need to set BUILD_DEBUG_FLAG=1, also the new docker image won't be saved.
export BUILD_DEBUG_FLAG=0

checkDir=()
#checkDir[0]="~/workspace/cacheData/imx7-gcc"
#checkDir[1]="~/workspace/cacheData/imx7-gcc1"

checkFile=()
checkFile[0]="$HOST_CACHE_DIR/qt-everywhere-opensource-src-5.6.1.tar.gz"
#checkFile[1]="~/workspace/cacheData/qt-everywhere-opensource-src-5.6.1.tar.gz"
#checkFile[2]="~/workspace/cacheData/qt-everywhere-src-5.10.1.tar.gz"
	

#This function will be called before building the docker image. Normally it should return 0.
#If you found some error and want to stop the building, the function should return any value other than 0.
function preCheck (){
    for dir in ${checkDir[@]}
    do
	if [ ! -d $dir ]; then
	    echo "dir: $dir is not exist!"
	    return 3
	fi
    done	
		
    for file in ${checkFile[@]}
    do
        if [ ! -f $file ]; then
	    echo "file: $file is not exist!"
	    return 4
    	fi
    done
	
    return 0
}

# -------------------------------------------------------------------------------------------------------------
# Everysetting is ready, we can build docker image now. 
# -------------------------------------------------------------------------------------------------------------
preCheck
cd $CURRENT_DIR/../..
./buildProjectImage $CURRENT_DIR

