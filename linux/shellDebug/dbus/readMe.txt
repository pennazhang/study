1. 本例子主要是利用dbus接口来进行调试，因此只能在linux下运行。
2. 编译并运行hello-daemon：  
	cd hello-daemon
	mkdir build
	cd build
	cmake ..
	make -j8
	./hello-daemon
	
3. 编译并运行dc-daemon：  
	cd dc-daemon
	mkdir build
	cd build
	cmake ..
	make -j8
	./dc-daemon
	
4. 在另外一台电脑上运行secureCrt，并通过该软件的telent协议连接到dc-daemon的电脑上的tcp:5023端口，并运行：
 -> help
^Jadd:a b > return the sum of a and b
set:variable:param > set parameter to the variable
get:variable > get the value of the variable

help > shows this help

  retult = 0

 -> set
^JFormat -> set:key:[value]
 For example -> set:muteFlag:true
 
Available key: 
    muteFlag -> set muteFlag to true or false (example: muteFlag:true)
    delayTime -> set delayTime in ms (example: delayTime:90)
    updateUnit -> updateUnit > updateUnit remoteIP(example: updateUnit:192.168.1.100)


  retult = 0


 -> set:delayTime: 50
^J"set delayTime = 50"

  retult = 0

 -> get:delayTime
^J50
  retult = 0

5. 使用语法：
    add:2 3
	set:delayTime: 50
	set: muteFlag : true
	get : delayTime
	get : muteFlag
	