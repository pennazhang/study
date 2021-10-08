#!/bin/bash

set -e

MYDIR=`dirname "$0"`; MYDIR=`realpath "$MYDIR"`; export MYDIR
PROJ_DIR=`cd $MYDIR/..; pwd`; export PROJ_DIR
echo MYDIR=$MYDIR
echo PROJ_DIR=$PROJ_DIR

# We need to get DOCKER_CACHE_DIR from docker.conf.
# since we don't need DOCKER_CACHE_DIR, we didn't need to source docker.conf
#source docker.conf
#echo DOCKER_CACHE_DIR=$DOCKER_CACHE_DIR

# DOCKER_CACHE_DIR must not be NULL in this project.
#if [ ! -d $DOCKER_CACHE_DIR ];then 
#    echo "can't find the DOCKER_CACHE_DIR:$DOCKER_CACHE_DIR "
#    exit -1
#fi

cd $MYDIR
sudo dpkg --add-architecture i386
sudo apt update

# install dependencies
if [ ! -f /tmp/.done_tetris_apt ]; then
    # development
    sudo apt-get install -y git build-essential cmake autogen autoconf libtool m4 pkg-config bc device-tree-compiler python wget cpio unzip rsync gnupg2 lib32stdc++6 lib32z1 libzip2 python-apt libncurses5-dev
    sudo apt install -y android-tools-adb openjdk-7-jdk subversion curl flex bison xsltproc u-boot-tools gperf gcc-multilib libswitch-perl libxml2-utils
    
    #for i386
    sudo apt install -y zlib1g:i386 liblzo2-dev:i386 uuid-dev:i386 zlib1g-dev:i386 
    
    sudo apt install -y gcc g++
    
    cd /usr/lib/gcc/x86_64-linux-gnu/4.8/32
    sudo ln -s ../../../../../lib32/libstdc++.so.6 libstdc++.so
    
    cd /tmp
    wget http://mirror.cogentco.com/pub/apache/maven/maven-3/3.3.9/binaries/apache-maven-3.3.9-bin.tar.gz
    tar zxvf apache-maven-3.3.9-bin.tar.gz
    sudo mv apache-maven-3.3.9 /usr/share/maven
    
    wget https://archive.apache.org/dist/ant/binaries/apache-ant-1.9.6-bin.tar.gz
    tar -zxvf apache-ant-1.9.6-bin.tar.gz
    sudo mv apache-ant-1.9.6 /usr/share/ant
    
    sudo chmod 777 /opt

    # svn checkout http://uss.amx.internal/scm/trunk/tools/java/lib /opt/ant/lib
    # sometimes it need password, so we have to download the lib to ~/workspace/cacheData/uss.amx.internal.lib.tar.gz first.
    mkdir -p /opt/ant
    cd /opt/ant
    tar zxvf ~/workspace/cacheData/uss.amx.internal.lib.tar.gz
    cd /usr/share/ant/lib
    ls -1 /opt/ant/lib/*.jar | while read f; do 
       sudo ln -s $f .
    done 
    
    cd /tmp
    wget http://apache.claz.org/maven/maven-3/3.1.1/binaries/apache-maven-3.1.1-bin.tar.gz
    tar -zxvf apache-maven-3.1.1-bin.tar.gz
    mv apache-maven-3.1.1  /opt/.
    rm apache-maven-3.1.1-bin.tar.gz

    cd /opt
    tar zxvf ~/workspace/cacheData/android-sdk_LB_ModeroE-G5_Tetris_1_8_1_linux-x86-updated.tar.gz
    
    cd ~/workspace/cacheData
    ./jdk-6u45-linux-x64.bin
    sudo mv jdk1.6.0_45 /usr/lib/jvm/.
    cd /usr/lib/jvm
    sudo ln -s jdk1.6.0_45 java-6-sun
    sudo ln -s jdk1.6.0_45 java-6-oracle
    
    sudo update-alternatives --install /usr/bin/javac javac /usr/lib/jvm/java-6-oracle/bin/javac 1072
    sudo update-alternatives --install /usr/bin/java java /usr/lib/jvm/java-6-oracle/bin/java 1072
    sudo update-alternatives --set java /usr/lib/jvm/java-7-openjdk-amd64/jre/bin/java
    sudo update-alternatives --set javac /usr/lib/jvm/java-7-openjdk-amd64/bin/javac

    
    rm -rf ~/.repoconfig
    
    touch /tmp/.done_tetris_apt
fi

echo "setupEnv.sh is called successfully!"
