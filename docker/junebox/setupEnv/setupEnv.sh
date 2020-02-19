#!/bin/bash

set -e

MYDIR=`dirname "$0"`; MYDIR=`realpath "$MYDIR"`; export MYDIR
PROJ_DIR=`cd $MYDIR/..; pwd`; export PROJ_DIR
export CACHE_DATA_DIR=$HOME/cacheData

if [ ! -d $CACHE_DATA_DIR ]; then
	mkdir -p $CACHE_DATA_DIR
fi

export LD_LIBRARY_PATH=$PROJ_DIR/software/cross_compiler/host/usr/lib
export PATH=$PATH:$PROJ_DIR/software/cross_compiler/host/usr/bin
export CROSS_COMPILE=$PROJ_DIR/software/cross_compiler/host/usr/bin/arm-linux-

echo MYDIR=$MYDIR
echo PROJ_DIR=$PROJ_DIR
echo CACHE_DATA_DIR=$CACHE_DATA_DIR
echo "CROSS_COMPILE=$CROSS_COMPILE"

cd $MYDIR
sudo bash -e "$MYDIR/setupEnv_root.sh" "$1"

#build and install tslib
if [ ! -f ~/.junebox_tslib_done ]; then
	# Get the source code of tslib
	cd $CACHE_DATA_DIR
	rm -rf $CACHE_DATA_DIR/tslib
	git clone https://github.com/kergoth/tslib.git
	cd $CACHE_DATA_DIR/tslib
	./autogen.sh
	./configure --host=arm-linux ac_cv_func_malloc_0_nonnull=yes --cache-file=arm-linux.cache CC=$PROJ_DIR/software/cross_compiler/host/usr/bin/arm-linux-gcc --prefix=/opt/arm/tslib
	make -j4
	sudo make install
    touch ~/.junebox_tslib_done
fi

#build and install QT4.8.6
if [ ! -f ~/.junebox_qt4.8.6_done ]; then
	cd $CACHE_DATA_DIR
	# Get the source code of qt-4.8.6
	if [ ! -f $CACHE_DATA_DIR/qt-everywhere-opensource-src-4.8.6.tar.gz ];then
		wget http://download.qt.io/archive/qt/4.8/4.8.6/qt-everywhere-opensource-src-4.8.6.tar.gz
	fi
	
	rm -rf $CACHE_DATA_DIR/qt-everywhere-opensource-src-4.8.6
	tar zxvf qt-everywhere-opensource-src-4.8.6.tar.gz
	cp -rf $MYDIR/qmake.conf $CACHE_DATA_DIR/qt-everywhere-opensource-src-4.8.6/mkspecs/qws/linux-arm-g++/.

	cd $CACHE_DATA_DIR/qt-everywhere-opensource-src-4.8.6
	./configure \
        -opensource \
        -confirm-license \
        -release -shared \
        -embedded arm \
        -xplatform qws/linux-arm-g++ \
        -depths 16,18,24,32 \
        -fast \
        -optimized-qmake \
        -pch \
        -qt-sql-sqlite \
        -qt-libjpeg \
        -qt-zlib \
        -qt-libpng \
        -qt-freetype \
        -little-endian -host-little-endian \
        -no-qt3support \
        -no-libtiff -no-libmng \
        -no-opengl \
        -no-mmx -no-sse -no-sse2 \
        -no-3dnow \
        -no-openssl \
        -no-webkit \
        -no-qvfb \
        -no-phonon \
        -no-nis \
        -no-opengl \
        -no-cups \
        -no-glib \
        -no-xcursor -no-xfixes -no-xrandr -no-xrender \
        -no-separate-debug-info \
        -nomake examples -nomake tools -nomake docs \
        -qt-mouse-tslib \
        -I/opt/arm/tslib/include \
        -L/opt/arm/tslib/lib \
        --prefix=/opt/arm/qt4.8.6

	make -j4
	sudo make install
	qtchooser -install junebox /opt/arm/qt4.8.6/bin/qmake
	export QT_SELECT=junebox
	
	touch ~/.junebox_qt4.8.6_done
fi
