Topic: How to use syslog() function

pzhang@video-and-control:~/workspace/study/linux/syslog$ make
rm -f *.o *.exe *.bak *.pre syslog
g++ -c  -DRUN_SYSTEM_WHEN_POSSIBLE  syslog.cpp -o syslog.o 
cc -o syslog syslog.o -L. -g -Wall  -lstdc++ 

pzhang@video-and-control:~/workspace/study/linux/syslog$ ./syslog

pzhang@video-and-control:~/workspace/study/linux/syslog$ sudo tail -5 /var/log/syslog
Aug 19 14:35:47 BuildServer whoopsie[970]: [14:35:47] Found usable connection: /org/freedesktop/NetworkManager/ActiveConnection/1
Aug 19 14:35:53 BuildServer whoopsie[970]: [14:35:53] online
Aug 19 14:35:58 BuildServer systemd[1]: NetworkManager-dispatcher.service: Succeeded.
Aug 19 14:37:34 video-and-control exampleprog[26550]: A tree falls in a forest.
Aug 19 14:37:34 video-and-control exampleprog[26550]: Program started by User 1004.
