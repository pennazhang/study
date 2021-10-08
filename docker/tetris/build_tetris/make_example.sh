#!/bin/bash

set -e
mkdir -p ~/tetris/script ~/tetris/moderox ~/tetris/roombook
cd ~/tetris/script
git clone ssh://uss:29418/AmxProjects/Scripts/Modero-X -b  B_Tetris
sudo sysctl -w net.ipv4.tcp_window_scaling=0
cd Modero-X
ant -f moderox.xml  -DBASE=/home/pzhang/tetris/moderox -DJOBS=-j20 -DREPO_JOBS=-j20 -DmajorVersion=1 -DminorVersion=8 -DbuildNumber=3 -DMAVEN3_PATH=/opt/apache-maven-3.1.1 -DJAVA_7_HOME=/usr/lib/jvm/java-7-openjdk-amd64 -DANDROID_SDK_PATH=/opt/android-sdk_LB_ModeroE-G5_Tetris_1_8_1_linux-x86 all-eng sign-eng
ant -f roombook.xml -DBASE=/home/pzhang/tetris/roombook -DJOBS=-j20 -DREPO_JOBS=-j20 -DmajorVersion=1 -DminorVersion=8 -DbuildNumber=3 -DMAVEN3_PATH=/opt/apache-maven-3.1.1 -DJAVA_7_HOME=/usr/lib/jvm/java-7-openjdk-amd64 -DANDROID_SDK_PATH=/opt/android-sdk_LB_ModeroE-G5_Tetris_1_8_1_linux-x86 all-eng sign-eng
