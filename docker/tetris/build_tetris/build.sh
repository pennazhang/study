#!/bin/bash

set -e


# -------------------------------------------------------------------------------------------------------------
# Available PRODUCT = [ roombook | moderox ] room'r
# -------------------------------------------------------------------------------------------------------------
PRODUCT=roombook

BASE_DIR=${HOME}/tetris/${PRODUCT}

# -------------------------------------------------------------------------------------------------------------
# Available BRANCH = [B_Jenga_May|B_Tetris|B_Jenga|B_Jenga_Tetris|B_Jenga_New|""] 
# -------------------------------------------------------------------------------------------------------------
BRANCH="B_Jenga_May"

# -------------------------------------------------------------------------------------------------------------
# Available BUILD_TARGET = [ debug | all-eng | all-user | all | sign-user | sign-eng | all-sdk ] 
# -------------------------------------------------------------------------------------------------------------
BUILD_TARGET="all-user sign-user"



HOME_DIR=/home/$USER_NAME

PARALLEL_JOBS_COUNT=20

MAJOR_VERSION=2 
MINOR_VERSION=0 
BUILD_NUMBER=0

MAVEN3_PATH=/opt/apache-maven-3.1.1
JAVA_7_HOME=/usr/lib/jvm/java-7-openjdk-amd64
ANDROID_SDK_PATH=/opt/android-sdk_LB_ModeroE-G5_Tetris_1_8_1_linux-x86

# check the product name
if [[ $PRODUCT == roombook ]]; then
    echo "roombook.xml"
elif [[ $PRODUCT == moderox ]]; then
	echo "moderox.xml"
else
    echo "Invalid product name. Please set PRODUCT first."
    exit -1
fi

#check the base dir
if [[ -z $BASE_DIR ]]; then
    echo "Invalid base dir. Please set BASE_DIR first."
    exit -1
fi
mkdir -p $BASE_DIR

#check the branch
if [[ -d Modero-X ]]; then
    cd Modero-X
    if [[ ! -z $BRANCH ]]; then
        echo "git checkout $BRANCH"
		git checkout $BRANCH
    fi
else
    if [[ -z $BRANCH ]]; then
        echo "Please set a default branch for: git clone ssh://uss:29418/AmxProjects/Scripts/Modero-X"
        exit -1
    else
		echo "git clone ssh://uss:29418/AmxProjects/Scripts/Modero-X -b ${BRANCH}"
        git clone ssh://uss:29418/AmxProjects/Scripts/Modero-X -b ${BRANCH}
        cd Modero-X
    fi
fi

now=$(date '+%Y/%m/%d %H:%M:%S')
echo "Current time : $now"

mkdir -p $BASE_DIR
echo "Build time : $now" > $BASE_DIR/build.tag
echo "Build path : $(pwd)" >> $BASE_DIR/build.tag

if [[ ! -z $BRANCH ]]; then
	echo "branch : $BRANCH" >> $BASE_DIR/build.tag
fi


echo "ant -f ${PRODUCT}.xml -DBASE=${BASE_DIR} -DJOBS=-j${PARALLEL_JOBS_COUNT} -DREPO_JOBS=-j${PARALLEL_JOBS_COUNT} \
 -DmajorVersion=${MAJOR_VERSION} -DminorVersion=${MINOR_VERSION} -DbuildNumber=${BUILD_NUMBER} \
 -DMAVEN3_PATH=${MAVEN3_PATH} -DJAVA_7_HOME=${JAVA_7_HOME} -DANDROID_SDK_PATH=${ANDROID_SDK_PATH} ${BUILD_TARGET}" >> $BASE_DIR/build.tag


echo -e "\n\n\n------------------------------------------------------------------------------------------------------\n"
echo "ant -f ${PRODUCT}.xml -DBASE=${BASE_DIR} -DJOBS=-j${PARALLEL_JOBS_COUNT} -DREPO_JOBS=-j${PARALLEL_JOBS_COUNT} \
 -DmajorVersion=${MAJOR_VERSION} -DminorVersion=${MINOR_VERSION} -DbuildNumber=${BUILD_NUMBER} \
 -DMAVEN3_PATH=${MAVEN3_PATH} -DJAVA_7_HOME=${JAVA_7_HOME} -DANDROID_SDK_PATH=${ANDROID_SDK_PATH} ${BUILD_TARGET}"
echo -e "\n\n------------------------------------------------------------------------------------------------------\n\n\n"


ant -f ${PRODUCT}.xml -DBASE=${BASE_DIR} -DJOBS=-j${PARALLEL_JOBS_COUNT} -DREPO_JOBS=-j${PARALLEL_JOBS_COUNT} \
 -DmajorVersion=${MAJOR_VERSION} -DminorVersion=${MINOR_VERSION} -DbuildNumber=${BUILD_NUMBER} \
 -DMAVEN3_PATH=${MAVEN3_PATH} -DJAVA_7_HOME=${JAVA_7_HOME} -DANDROID_SDK_PATH=${ANDROID_SDK_PATH} ${BUILD_TARGET}
