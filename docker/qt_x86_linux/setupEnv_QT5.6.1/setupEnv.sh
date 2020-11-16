#!/bin/bash

set -e

MYDIR=`dirname "$0"`; MYDIR=`realpath "$MYDIR"`; export MYDIR
PROJ_DIR=`cd $MYDIR/..; pwd`; export PROJ_DIR
echo MYDIR=$MYDIR
echo PROJ_DIR=$PROJ_DIR

# We need to get DOCKER_CACHE_DIR from docker.conf.
source docker.conf
echo DOCKER_CACHE_DIR=$DOCKER_CACHE_DIR

# DOCKER_CACHE_DIR must not be NULL in this project.
if [ ! -d $DOCKER_CACHE_DIR ];then 
    echo "can't find the DOCKER_CACHE_DIR:$DOCKER_CACHE_DIR "
    exit -1
fi

cd $MYDIR
sudo apt update
#sudo bash -e "$MYDIR/setupEnv_root.sh" "$1"
cp .qt-license ~/.

#build and install QT5.6.1 for x86 in Linux
if [ ! -f /tmp/.x86_qt5.6.1_done ]; then
    cd $DOCKER_CACHE_DIR
    # Get the source code of qt-5.6.1
    if [ ! -f $DOCKER_CACHE_DIR/qt-everywhere-opensource-src-5.6.1.tar.gz ];then
        wget https://download.qt.io/archive/qt/5.6/5.6.1/single/qt-everywhere-opensource-src-5.6.1.tar.gz
    fi
	
    rm -rf qt-everywhere-opensource-src-5.6.1
    tar zxvf qt-everywhere-opensource-src-5.6.1.tar.gz
	
    echo "QMAKE_CXXFLAGS += -std=gnu++98" >> qt-everywhere-opensource-src-5.6.1/qtbase/mkspecs/common/g++-unix.conf

    export QMAKESPEC=linux-g++
    cd $DOCKER_CACHE_DIR/qt-everywhere-opensource-src-5.6.1
    ./configure \
        -commercial -confirm-license \
	-platform linux-g++ \
	-c++std c++11 \
        -release -shared \
	-nomake tests -nomake examples \
	-qt-xcb -qt-sql-sqlite -qt-zlib \
	-skip qtquickcontrols -skip qtquickcontrols2 -skip qtsensors -skip qtdoc \
        --prefix=/opt/qt5.6.1

    make -j8
    sudo make install
	
    touch /tmp/.x86_qt5.6.1_done
fi




echo "setupEnv.sh is called successfully!"
