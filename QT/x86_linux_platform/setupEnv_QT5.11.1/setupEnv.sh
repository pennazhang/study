#!/bin/bash

set -e

MYDIR=`dirname "$0"`; MYDIR=`realpath "$MYDIR"`; export MYDIR
PROJ_DIR=`cd $MYDIR/..; pwd`; export PROJ_DIR
export CACHE_DATA_DIR=$HOME/cacheData

if [ ! -d $CACHE_DATA_DIR ]; then
	mkdir -p $CACHE_DATA_DIR
fi

echo MYDIR=$MYDIR
echo PROJ_DIR=$PROJ_DIR
echo CACHE_DATA_DIR=$CACHE_DATA_DIR
echo "CROSS_COMPILE=$CROSS_COMPILE"

cd $MYDIR
sudo bash -e "$MYDIR/setupEnv_root.sh" "$1"

