#!/bin/bash
set -e
MY_DIR=`dirname "$0"`; MY_DIR=`realpath "$MY_DIR"`

#we need to build checkCmd first
cd $MY_DIR

#Let's build checkCmd in qt5.11.1:0.2
cd /git/study/qt/project/checkCmd/project
rm -rf Makefile out || true
qmake
make
rm -rf /git/bin/checmCmd libQt5Xml.so.5* libQt5Core.so.5* libdouble-conversion.so.1*  || true
cp out/bin/checkCmd $MY_DIR/.
cp -a /usr/lib/x86_64-linux-gnu/libQt5Xml.so.5*  $MY_DIR/.
cp -a /usr/lib/x86_64-linux-gnu/libQt5Core.so.5* $MY_DIR/.
cp -a /usr/lib/x86_64-linux-gnu/libdouble-conversion.so.1* $MY_DIR/.

cp -a /usr/lib/x86_64-linux-gnu/libicui18n.so.60* $MY_DIR/.
cp -a /usr/lib/x86_64-linux-gnu/libicuuc.so.60* $MY_DIR/.
cp -a /usr/lib/x86_64-linux-gnu/libicudata.so.60* $MY_DIR/.
