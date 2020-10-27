Yocto Project Description:
	This document describes how to setup a Yocto Project build environment with docker for an i.MX board by using the i.MX release layer and i.MX-specific usage.
	The Yocto Project is an open-source collaboration focused on embedded Linux® OS development. For more information on Yocto Project, see the Yocto Project page: www.yoctoproject.org/. 
	There are several documents on the Yocto Project home page that describe in detail how to use the system. To use the basic Yocto Project without the i.MX release layer, follow the instructions in the Yocto Project Quick Start found at www.yoctoproject.org/docs/current/yocto-project-qs/yocto-project-qs.html.

Build Docker:
	run buildDocker.sh to setup the docker image.
	
	pzhang@Ubuntu18:/git/docker/qt4.8.7$ ./buildDocker.sh
	
	After building, the docker image: qt4.8.7:0.2 will be created.

Start Docker:
	1. To just start a docker image to build QT program or run a QT program without GUI, you can run: ./startDocker.sh
		
	pzhang@Ubuntu18:/git/docker/qt4.8.7$ ./startDocker.sh
	
	Once the docker container is start, just run the following commands to build the demo project: 
	~$ sh -c "cd /git/docker/qt4.8.7/setupEnv/demoGui && qmake && make"
	
	After building, the project output file: /git/docker/qt4.8.7/setupEnv/demoGui/demoGui can be build.
	Notice: You can run not run ./demoGui in this docker, because this docker didn't support GUI.
	
	2. To start a docker image to build QT program and run a QT program with GUI, you should run ./startDocker_withGUI.sh
	
		pzhang@Ubuntu18:/git/docker/qt4.8.7$ ./startDocker_withGUI.sh
		
	Once the docker container is start, just run the following commands to build the demo project: 
	~$ sh -c "cd /git/docker/qt4.8.7/setupEnv/demoGui && qmake && make"
	
	After building, the project output file: /git/docker/qt4.8.7/setupEnv/demoGui/demoGui can be build, and you can run ./demoGui in this docker, because this docker can support GUI.
	Notice: you must start the docker in terminal shell of Linux Desktop, not the terminal shell through SSH.
	
		