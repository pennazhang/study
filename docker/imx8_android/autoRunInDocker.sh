export WORKING_DIR=$HOME/workspace/imx8_android_test
export CACHE_DIR=/cacheData

mkdir -p $WORKING_DIR/bin
cd $WORKING_DIR

export PATH=${PATH}:$WORKING_DIR/bin
#source ./imx-android-11.0.0_2.2.0/imx_android_setup.sh  
export MY_ANDROID=$WORKING_DIR/android_build
echo $PATH
export AARCH64_GCC_CROSS_COMPILE=/opt/gcc-arm-8.3-2019.03-x86_64-aarch64-elf/bin/aarch64-elf-
export CLANG_PATH=/opt/prebuilt-android-clang

echo "IMX8MMEVK Android environment has been set."
