

Build Docker:
	./buildDocker.sh
	After building, use command: "docker images" to check the running result.
	
	In buildDocker.sh, the following folders are mapped when building docker:
	
	$HOST_GIT_DIR => /git/Project  (HOST_GIT_DIR must be mapped) 
	$HOST_CACHE_DIR => /cacheData  (HOST_CACHE_DIR may be defined. It is used to store some files that can't be download from public network)
	/home/$USER_NAME => /home/$USER_NAME (/home/$USER_NAME must be mapped)
	

Start Docker:
	./startDocker.sh
	or 
	./startDocker.sh "cd ~/workspace/study/qt/demoGui/project && rm -rf out Makefile && qmake && make && ./out/release/bin/demoGui && bash"
	
	In startDocker.sh, the following folders are mapped when starting docker:
	
	for example: -v /home/pzhang:/home/pzhang  -v /run/user/1004:/run/user/1004  -v /usr/share/fonts:/usr/share/fonts
	
	/home/$USER_NAME => /home/$USER_NAME (/home/$USER_NAME must be mapped)	
	$XDG_RUNTIME_DIR => $XDG_RUNTIME_DIR   (XDG_RUNTIME_DIR is defined only for display. You can delete it if you don't want to display in docker")
	/usr/share/fonts => /usr/share/fonts (it is mapped only for fonts. You can delete it if you don't want to display Chinese in docker")

Other issue:
	Before building docker, we should download the infinity-linux-kernel as: 
	cd $HOME_DIR/workspace
	git clone  ssh://git@bitbucket.harman.com:7999/hpromixer/infinity-linux-kernel.git