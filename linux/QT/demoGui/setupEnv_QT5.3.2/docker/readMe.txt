
Project Description:
    junebox is a project developed by Harman Shenzhen development center.

Build Docker:

    PreBuild:
        1. Modify buildDocker.sh to set the correct HOST_GIT_DIR.
        2. You must download the junebox source code at: $HOST_GIR_DIR/junebox. 
    
    Build:
        1. run buildDocker.sh to setup the host environment and the docker image. 
               ./buildDocker.sh
    
            After building, the docker image: junebox_docker:0.2 will be created.

Start Docker:

    PreBuild:
        1. Modify the startDocker.sh to set the correct HOST_GIT_DIR.
        2. You can download your junebox source code at any sub-folder of $HOST_GIT_DIR, such as: /mnt/disk2/git/junebox_release.
        3. Also you can modify the folder map between Host and Container with the parameter: DIR_MAP="-v $HOST_GIT_DIR:/git"
	4. Also you can modify the MACRO definition with the parameter: RUN_ENV

    Build:
        1. run startDocker.sh to start the docker container and build the project.
               ./startDocker.sh
    
        Once the docker container is start, just run the following commands in the container to build the project: 
               cd /git/junebox_release/software
               ./makeAll.sh
    
        After building, the final images can be found at Host: $HOST_GIT_DIR/junebox_release/software/Atmel/binaries.
	
	2. run startDocker_buildAll.sh to start the docker container and build the project automatically.
               ./startDocker_buildAll.sh
    	After building, the final images can be found at Host: $HOST_GIT_DIR/junebox_release/software/Atmel/binaries.
	
	2.1 Notice: Before running startDocker_buildAll.sh, you need to modify the last line: "cd /git/junebox/software && ./makeAll.sh";
	For example, if your project is lies in $HOST_GIT_DIR/junebox_release, you should modify it to "cd $HOST_GIT_DIR/junebox_release/software && ./makeAll.sh";
