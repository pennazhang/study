#export PATH=$PATH:/tmp/bin

export PROJ_DIR=${HOME}/workspace/dvi
cd $PROJ_DIR/software

export LD_LIBRARY_PATH=$PROJ_DIR/software/cross_compiler/host/usr/lib
export PATH=$PATH:$PROJ_DIR/software/cross_compiler/host/usr/bin
export CROSS_COMPILE=$PROJ_DIR/software/cross_compiler/host/usr/bin/arm-linux-
export ARCH=arm

echo PROJ_DIR=$PROJ_DIR

echo "CROSS_COMPILE=$CROSS_COMPILE"
