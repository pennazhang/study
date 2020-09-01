#!/bin/sh
test_file=test.tar

help_message()
{
	echo "usage: $0 test_folder test_time_in_second"
	echo "For example: $0 /mnt/mmcblk0p1 100"
}


zip_test()
{
	if [ $test_folder != $current_folder ]; then
		cp -f $test_file $test_folder/.
	fi
	
	rm -rf $test_folder/bonnie_temp
	mkdir -p $test_folder/bonnie_temp
	tar xvf $test_folder/$test_file -C $test_folder/bonnie_temp > /dev/null 2>&1
	sync

	result=0
	i=1
	while [ $i -le $1 ]
	do		
		cd $test_folder
		rm -rf $test_folder/stress_test
		mkdir -p $test_folder/stress_test
		cd $test_folder/stress_test
   		echo "$test_folder :$i unzip testing..."
    	tar xvf $test_folder/$test_file  > /dev/null 2>&1
    	result=$?
    	sync
    	if [ $result = 0 ]; then
    		echo "$test_folder :$i unzip test OK!" 
    	else
    		echo "$test_folder :$i unzip test Failed!"
    		return 1
    	fi

		diff -r $test_folder/bonnie_temp $test_folder/stress_test  > /dev/null 2>&1
		result=$?
		if [ $result = 0 ]
		then
			echo "----$test_folder :$i File compare OK!----"
		else
			echo "----$test_folder :$i File compare error!----"
			retrurn 1
		fi
		
    	rm -rf $test_folder/$test_file
   		echo "$test_folder :$i zip testing..."
    	tar cvf $test_folder/$test_file * >  /dev/null 2>&1
    	result=$?
    	sync
		
		current_time=`date +%s`
    	if [ $result = 0 ]; then
    		echo "$test_folder :$i zip test OK! total cost: $(($current_time-$begin_time)) seconds"
    	else
    		echo "$test_folder :$i zip test Failed! total cost: $(($current_time-$begin_time)) seconds"
    		return 1
    	fi
    	    	
    	i=$(($i+1))

		if [ $current_time -ge $end_time ]
		then 
	    	i=$(($1+1))
		fi		
	done
	
	return $result
}


if [ $# != 2 ]; then
	help_message;
	exit 1;
fi

if [ $2 -le 0 ]
then
	help_message
	exit 1
fi

if [ ! -e ${test_file} ]; 
then
	echo "can not find the file: $testfile"
	exit 1;
fi

if [ ! -e $1 ]; 
then
	echo "can not find the tested folder: $1"
	exit 1;
fi

current_folder=$(printf %s "$PWD")
test_folder=$(cd "$(dirname -- "$1/.")"; printf %s "$PWD")

echo "=======================file system test ========================="
echo "test file = $test_file"
echo "test folder = $test_folder"
echo "========================= begin test ============================"
echo ""
begin_time=`date +%s`
end_time=$(($begin_time+$2))
rm -rf $test_folder/bonnie_temp $test_folder/stress_test 
zip_test 1000000000
result=$?
end_time=`date +%s`
last_time=$(($end_time-$begin_time))
echo "Total test time: $last_time seconds"
rm -rf $test_folder/bonnie_temp $test_folder/stress_test 
if [ $test_folder != $current_folder ]; then
	rm -rf $test_folder/$testfile
fi
exit $result
