#!/bin/bash

is_empty_dir(){
    return `ls -A $1 | wc -w`
}

function del_empty_dir(){
    for file in `ls $1` #注意此处这是两个反引号，表示运行系统命令
    do
        if [ -d $1"/"$file ] #注意此处之间一定要加上空格，否则会报错
        then
            del_empty_dir $1"/"$file
        else
            : #echo $1"/"$file #在此处处理文件即可
        fi
    done

    if is_empty_dir $1
    then
        rm -rf $1
    fi
}

#读取第一个参数
if [ ! $# -eq 1 ];
then
    echo usage: $0 dir_name
    exit
fi

del_empty_dir $1