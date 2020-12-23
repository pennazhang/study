本例子主要演示tcp的server和client例子。
server采用阻塞式 + 线程来实现。
client采用select来判断是否有数据可读，如果有数据再去读socket数据。
