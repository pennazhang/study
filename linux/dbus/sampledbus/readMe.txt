
To Build Docker:
# cd ~/workspace/study/docker/dbus
# ./buildDockerImage.sh 

To Start Docker and build dbus:
# cd ~/workspace/study/docker/dbus
# ./startDockerImage.sh 
# cd ~/workspace/study/linux/dbus/sampledbus
# ./makeAll.sh

To run the dbus demos: 
--	You mustn't run it in the docker;  You must run it in the Host Linux.
1. Open a termianl in the host linux and run: 
  # cd ~/workspace/study/linux/dbus/sampledbus
  # cd ./adaptor
  
2. Open another terminal in the host Linux and run:
  # cd ~/workspace/study/linux/dbus/sampledbus
  # ./proxy
  # ./proxy
  
Then you will see the following information in the first terminal as below:
  
pzhang@video-and-control:~/workspace/study/linux/dbus/sampledbus$ ./adaptor
Hello 'world'
Hello 'world'


