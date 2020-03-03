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

#build and install QT5.3.2 for x86 in Linux
if [ ! -f ~/.x86_qt5.3.2_done ]; then
	cd $CACHE_DATA_DIR
	# Get the source code of qt-5.3.2
	if [ ! -f $CACHE_DATA_DIR/qt-everywhere-opensource-src-5.3.2.tar.gz ];then
		wget https://download.qt.io/archive/qt/5.3/5.3.2/single/qt-everywhere-opensource-src-5.3.2.tar.gz
	fi
	
	rm -rf $CACHE_DATA_DIR/qt-everywhere-opensource-src-5.3.2
	tar zxvf qt-everywhere-opensource-src-5.3.2.tar.gz
	
	echo "QMAKE_CXXFLAGS += -std=gnu++98" >> qt-everywhere-opensource-src-5.3.2/qtbase/mkspecs/common/g++-unix.conf

	cd $CACHE_DATA_DIR/qt-everywhere-opensource-src-5.3.2
	./configure \
        -opensource \
        -confirm-license \
        -release -shared \
        -directfb -linuxfb \
        -no-kms -no-opengl -no-pch \
        -no-xinput2 -no-xcb-xlib -no-glib -no-pulseaudio \
        -no-alsa -no-gtkstyle -nomake examples -no-compile-examples -no-nis \
        -no-cups -no-iconv -no-icu -no-fontconfig -no-dbus -no-xcb -no-eglfs \
        -no-opengl \
        -no-openssl \
        -no-nis \
        -no-opengl \
        -no-cups \
        -no-glib \
        -no-xcursor -no-xfixes -no-xrandr -no-xrender \
        -no-separate-debug-info \
        --prefix=/opt/qt5.3.2

	make -j4
	sudo make install
	qtchooser -install x86_qt5.3.2 /opt/qt5.3.2/bin/qmake
	export QT_SELECT=x86_qt5.3.2
	
	touch ~/.x86_qt5.3.2_done
fi
