JAVA常用的类：
StringBuffer - 对字符串进行各种修改操作，包括添加、删除、替换等。
Runtime - 获取JVM的空闲内存、运行垃圾回收器释放空间、以及执行本机命令。
Locale - 资源国际化
System - 与系统调用相关的类，比如： 退出程序，运行垃圾回收，返回以毫秒为单位的时间，数组复制操作，获取系统的属性，根据键值获取属性等。
Date Calender - 与日期、时间相关的类
Math - 与数学函数相关的类
Random - 与随机数相关的类
NumberFormat - 格式化处理数字
Comparable - 用于排序等有比较操作的接口
Observable 类 - 实现观察者模式
Pattern / Matcher - 正则表达式类
Timer 、 TimerTask - 定时器
File / RandomAccessFile - 文件操作
Reader / Writer / OutputStream / InputStream - 字符流
InputStreamReader - 将输入的字节流转换成字符流
OutputStreamWriter - 将输出的字符流转换成字节流。
ByteArrayInputStream / ByteArrayOutputStream - 以内存字节数组为对象的输入输出操作。
PipedOutputStream - 使用管道实现线程之间的通信
PrintStream - 格式化输出
Scanner - 格式化输入
ProcessBuilder / Process / ProcessHandle - 进程操作
URLConnection - 获取URL连接内容
jsoup - HTML 文本解析器
WebController - 网络爬虫框架



Thread - 线程：
1. 如果要在类的静态函数中实现同步（临界区），我们可以使用synchronized(Student.class) { ... }
	如果要在类的成员函数中实现同步（临界区），我们可以使用synchronized(this) { ... }	
 
 
 