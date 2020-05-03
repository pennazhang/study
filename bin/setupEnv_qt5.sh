#!/bin/bash
set -e
MY_DIR=`dirname "$0"`; MY_DIR=`realpath "$MY_DIR"`

#we need to build checkCmd first
cd $MY_DIR

if [ -f checkCmd ]; then
	exit 0
fi

#to build checkCmd, we need docker qt5.11.1:0.2 first
cd /git/study/qt/x86_linux_platform/setupEnv_QT5.11.1/docker
./buildDocker.sh
./startDocker.sh "cd /git/study/bin && ./build_checkCmd_qt5.sh"
