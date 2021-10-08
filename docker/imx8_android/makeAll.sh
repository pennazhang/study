#!/bin/bash

set -e

#export WORKING_DIR=$HOME/workspace/imx8_android
#export CACHE_DIR=$HOME/workspace/cacheData

echo WORKING_DIR = $WORKING_DIR
echo CACHE_DIR = $CACHE_DIR

rm -rf $WORKING_DIR

mkdir -p $WORKING_DIR/bin
cd $WORKING_DIR
cp $CACHE_DIR/imx-android-11.0.0_2.2.0.tar.gz .
tar zxvf imx-android-11.0.0_2.2.0.tar.gz
curl https://storage.googleapis.com/git-repo-downloads/repo > ./bin/repo
chmod a+x ./bin/repo 
export PATH=${PATH}:$WORKING_DIR/bin
echo PATH=$PATH
export MY_ANDROID=$WORKING_DIR
source ./imx-android-11.0.0_2.2.0/imx_android_setup.sh  
source build/envsetup.sh
lunch evk_8mm-userdebug
./imx-make.sh -j16 2>&1 | tee build-log.txt
