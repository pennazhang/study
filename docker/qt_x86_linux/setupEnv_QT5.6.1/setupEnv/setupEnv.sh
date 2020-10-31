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
cp .qt-license ~/.
sudo bash -e "$MYDIR/setupEnv_root.sh" "$1"

#build and install QT5.6.1 for x86 in Linux
if [ ! -f /tmp/.x86_qt5.6.1_done ]; then
    if [ ! -z $CACHE_DATA_DIR ];then 
	sudo mkdir -p $CACHE_DATA_DIR
	sudo chmod 777 $CACHE_DATA_DIR
    fi
    
    cd $CACHE_DATA_DIR
    # Get the source code of qt-5.6.1
    if [ ! -f $CACHE_DATA_DIR/qt-everywhere-opensource-src-5.6.1.tar.gz ];then
        wget https://download.qt.io/archive/qt/5.6/5.6.1/single/qt-everywhere-opensource-src-5.6.1.tar.gz
    fi
	
    rm -rf qt-everywhere-opensource-src-5.6.1
    tar zxvf qt-everywhere-opensource-src-5.6.1.tar.gz
	
    echo "QMAKE_CXXFLAGS += -std=gnu++98" >> qt-everywhere-opensource-src-5.6.1/qtbase/mkspecs/common/g++-unix.conf

    export QMAKESPEC=linux-g++
    cd $CACHE_DATA_DIR/qt-everywhere-opensource-src-5.6.1
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
#    qtchooser -install x86_qt5.6.1 /opt/qt5.6.1/bin/qmake || true
#    export QT_SELECT=x86_qt5.6.1
	
    touch /tmp/.x86_qt5.6.1_done
fi
