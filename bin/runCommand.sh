#!/bin/bash
set -e

MY_FILE=$0
while [ -h $MY_FILE ]
do
   b=`ls -ld $MY_FILE|awk '{print $NF}'`
   c=`ls -ld $MY_FILE|awk '{print $(NF-2)}'`
   [[ $b =~ ^/ ]] && MY_FILE=$b  || MY_FILE=`dirname $c`/$b
done
echo $MY_FILE

MY_DIR=`dirname "$MY_FILE"`; MY_DIR=`realpath "$MY_DIR"`
echo $MY_DIR
cd $MY_DIR

if [ ! -f checkCmd ]; then
	./setupEnv_qt4.sh
fi

export LD_LIBRARY_PATH=$MY_DIR:$LD_LIBRARY_PATH
export PATH=$MY_DIR:$PATH

rm -rf nextCmd.sh
./checkCmd
if [ -f nextCmd.sh ]; then
	chmod +x nextCmd.sh
	dos2unix nextCmd.sh || true
	./nextCmd.sh 
fi
