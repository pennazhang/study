

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

Other issue:
	Please check: https://confluence.harman.com/confluence/display/HPROVC/Via+Build+Instructions for the process.
	Before building the docker image, you must apply for the account: ELINA_Guests and stash_elina_r.

	When building and running the docker, you must input the P4 password ( 2 times ).
	Normally the P4 Password is the same as the windows domain login password.
	Also you must input "no" when you are asked to input your choice for the command: dpkg-reconfigure dash. 
	
	Again, You must enter 2 times of Harman domain password and then ("no") when building docker image.
	