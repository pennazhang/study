

Build Docker:
	./buildDockerImage.sh
	After building, use command: "docker images" to check the running result.
	
	In buildDockerImage.sh, the following folders are mapped when building docker:
	
	$HOST_GIT_DIR => /git/Project  (HOST_GIT_DIR must be mapped) 
	$HOST_CACHE_DIR => /cacheData  (HOST_CACHE_DIR may be defined. It is used to store some files that can't be download from public network)
	/home/$USER_NAME => /home/$USER_NAME (/home/$USER_NAME must be mapped)
	
Start Docker:
	./startDockerImage.sh - autoRunInDocker.sh will be executed automatically when docker is start. 
	or 
	./startDockerImage.sh makeAll.sh - autoRunInDocker.sh and makeAll.sh will be executed automatically when docker is start.
	
	When starting docker, the following folders are mapped when starting docker:
		$CurrentDir      => /bin/docker	     (Must be mapped)
		/home/$USER_NAME => /home/$USER_NAME (/home/$USER_NAME must be mapped)	
		$XDG_RUNTIME_DIR => $XDG_RUNTIME_DIR   (XDG_RUNTIME_DIR is defined only for display. You can delete it if you don't want to display in docker")
		/usr/share/fonts => /usr/share/fonts (it is mapped only for fonts. You can delete it if you don't want to display Chinese in docker")

	
	The file: makeAll.sh  will be execute automatically when the docker image is start.

Other issue:
	Before building docker, we should download the infinity-linux-kernel as: 
	cd ~/workspace
	git clone  ssh://git@bitbucket.harman.com:7999/hpromixer/infinity-linux-kernel.git