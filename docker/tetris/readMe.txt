

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

安装前：
1. 请确保文件存在： $HOME_DIR/workspace/cacheData/android-sdk_LB_ModeroE-G5_Tetris_1_8_1_linux-x86-updated.tar.gz
	该文件是在android-sdk_LB_ModeroE-G5_Tetris_1_8_1_linux-x86.zip的基础上运行./android，然后在UI界面上升级数次Android SDK Tools，最后再打包形成该文件。
	因此我们可以直接使用该文件，省去了在UI界面中升级的烦恼。
2. 请确保文件存在： $HOME_DIR/workspace/cacheData/uss.amx.internal.lib.tar.gz
	该文件是 运行下面的命令得到的：svn checkout http://uss.amx.internal/scm/trunk/tools/java/lib lib && tar zcvf uss.amx.internal.lib.tar.gz lib

安装后
1. 完成container后，请手动修改/etc/hosts如下：
	127.0.1.1       tetris
	10.35.95.20  sw-mars.amx.internal
	其中 10.35.95.20是sw-mars.ad.harman.com的ip地址。
	# ping sw-mars.ad.harman.com
	PING sw-mars.ad.harman.com (10.35.95.20) 56(84) bytes of data.
	64 bytes from sw-mars.amx.internal (10.35.95.20): icmp_seq=1 ttl=59 time=203 ms
		
	# ping sw-mars.amx.internal
	PING sw-mars.amx.internal (10.35.95.20) 56(84) bytes of data.
	64 bytes from sw-mars.amx.internal (10.35.95.20): icmp_seq=1 ttl=59 time=203 ms
	64 bytes from sw-mars.amx.internal (10.35.95.20): icmp_seq=2 ttl=59 time=197 ms
2. 手动下载oracle java 6u45: jdk-6u45-linux-x64.bin ,因为版权，该程序无法自动下载安装：
	cd ~/workspace/cacheData/temp/
	./jdk-6u45-linux-x64.bin
	sudo mv jdk1.6.0_45 /usr/lib/jvm/.
	cd /usr/lib/jvm
	sudo ln -s jdk1.6.0_45 java-6-sun
	
3. 请在~/.m2/目录下检查下面这个文件settings.xml：
          xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
          xsi:schemaLocation="http://maven.apache.org/SETTINGS/1.0.0 http://maven.apache.org/xsd/settings-1.0.0.xsd">

  <profiles>
 
    <profile>
      <id>maven-https</id>
      <activation>
          <activeByDefault>true</activeByDefault>
      </activation>
      <repositories>
          <repository>
              <id>central</id>
              <url>https://repo1.maven.org/maven2</url>
              <snapshots>
                  <enabled>false</enabled>
              </snapshots>
          </repository>
      </repositories>
      <pluginRepositories>
          <pluginRepository>
              <id>central</id>
              <url>https://repo1.maven.org/maven2</url>
              <snapshots>
                  <enabled>false</enabled>
              </snapshots>
          </pluginRepository>
      </pluginRepositories>
    </profile>
  </profiles>

  <activeProfiles>
    <activeProfile>maven-https</activeProfile>
  </activeProfiles>
</settings>	
