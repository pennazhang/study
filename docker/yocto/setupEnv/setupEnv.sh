#!/bin/bash

set -e

MYDIR=`dirname "$0"`; MYDIR=`realpath "$MYDIR"`; export MYDIR
PROJ_DIR=`cd $MYDIR/..; pwd`; export PROJ_DIR

# If we define the HOST_CACHE_DIR,  then it will be mapped to CACHE_DATA_DIR
export CACHE_DATA_DIR=/cacheData

echo MYDIR=$MYDIR
echo PROJ_DIR=$PROJ_DIR
echo CACHE_DATA_DIR=$CACHE_DATA_DIR
echo "CROSS_COMPILE=$CROSS_COMPILE"

cd $MYDIR
sudo bash -e "$MYDIR/setupEnv_root.sh" "$1"

