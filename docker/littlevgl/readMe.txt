
Project Description:
	LittlevGL is a free and open-source graphics library providing everything you need to create embedded GUI with easy-to-use graphical elements, beautiful visual effects and low memory footprint.
	This project can show how to build and run the demo program on docker container.

Build Docker:
	run buildEnv.sh to setup the host environment and the docker image. To install the required package, you need to input the password for sudo during the setup process.

	pzhang@Ubuntu18:/git/docker/littlevgl$ ./buildEnv.sh
	
	After building, the host can be setup and the docker image: littlevgl:0.1 will be created.

Start Docker:
	You must start a shell within Host Desktop (That means you can't start a shell from SSH Client).
	In the shell, run startEnv.sh to start the docker container and build the project.
		
	pzhang@Ubuntu18:/git/docker/littlevgl$ ./startEnv.sh
	
	Once the docker container is start, just run the following commands to build the demo project: 
	~$ sh -c "cd /git/pc_simulator_sdl_eclipse && ./makeAll.sh"
	
	After building, the project output file: /git/littlevgl/demo can be build. You can run ./demo to show the demo.