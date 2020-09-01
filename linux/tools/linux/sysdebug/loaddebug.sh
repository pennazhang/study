#!/bin/sh
set -x

echo "goodluck"
rm /dev/sysdev
/sbin/rmmod sysdev
/sbin/insmod sysdev.ko
mknod /dev/sysdev c 3722 0
