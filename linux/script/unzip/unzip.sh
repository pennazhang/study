#!/bin/bash

FILE_NAME=$1
CURRENT_DIR=$(cd `dirname $0`; pwd)
echo CURRENT_DIR=$CURRENT_DIR

for filePath in $*
do
	cd $CURRENT_DIR

	DIR=`dirname $filePath`
	FILE_NAME=`basename $filePath`
	cd $DIR
	
	#overwrite the existing files.
	unzip -o $FILE_NAME
	
	#never overwrite existing files
	#unzip -n $FILE_NAME
	
done
