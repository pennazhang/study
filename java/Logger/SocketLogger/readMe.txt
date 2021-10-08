说明： SocketLogger可以通过TCP协议将要打印的信息发送到服务器上。前提条件是服务器的TCP Server已经运行起来，并且SocketLogger能正确连接到TCP Server上。
如果我们运行SocketLogger，在服务器上打印信息如下：

Z:\workspace\study\java\Logger\SocketLogger\LogServer\out\production\LogServer>java com.harman.test.Main
TCP Server Listening at port:5023
Type "exit" to quit...
WorkerThread:0 start...
onRecv0: <?xml version="1.0" encoding="UTF-8" standalone="no"?>
onRecv0: <!DOCTYPE log SYSTEM "logger.dtd">
onRecv0: <log>
onRecv0: <record>
onRecv0:   <date>2021-08-25T09:43:10.618427200Z</date>
onRecv0:   <millis>1629884590618</millis>
onRecv0:   <nanos>427200</nanos>
onRecv0:   <sequence>0</sequence>
onRecv0:   <logger>my.logger.SocketHandlerTest</logger>
onRecv0:   <level>INFO</level>
onRecv0:   <class>com.harman.study.Main</class>
onRecv0:   <method>main</method>
onRecv0:   <thread>1</thread>
onRecv0:   <message>SocketHandler is running now......</message>
onRecv0: </record>
onRecv0: </log>
Bye, WorkerThread0...

