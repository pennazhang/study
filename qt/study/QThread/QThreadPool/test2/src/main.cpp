#include <QCoreApplication>
#include <QThreadPool>
#include <QThread>
#include <QRunnable>
#include <QDebug>
#include <QThread>

class SleepThread : public QThread {
public: 
   static inline void msleep(unsigned long msecs) { 
       QThread::msleep(msecs);
   }
};

class MyRun : public QRunnable 
{
public:
	void run() 
	{
		int i=3;
		while(i) 
		{
			i--;
			qDebug() << "thread start:" << QThread::currentThreadId();
			SleepThread::msleep(500);
		}
	}
};

int main(int argc, char *argv[]) 
{
	QCoreApplication a(argc, argv);
	qDebug()<<"Main:"<<QThread::currentThreadId();
	QThreadPool pool;
	QThreadPool *global_pool = QThreadPool::globalInstance();
	MyRun *run=new MyRun;
	if(!run->autoDelete()) 
	{
		qDebug()<<"QRunnable's autoDelete default value is not true";
		run->setAutoDelete(true);
	}
	pool.setMaxThreadCount(2);//修改了局部线程数量
	qDebug() << "Local Pool:";
    qDebug()<< "maxThreadCount() = " << pool.maxThreadCount();
    qDebug() << "expiryTimeout() = " << pool.expiryTimeout();
    qDebug()<< "activeThreadCount() = " << pool.activeThreadCount();

	qDebug() << "Global Pool:";
    qDebug()<< "maxThreadCount() = " << global_pool->maxThreadCount();
    qDebug() << "expiryTimeout() = " << global_pool->expiryTimeout();
    qDebug()<< "activeThreadCount() = " << global_pool->activeThreadCount();
	
	pool.start(run);
	qDebug()<<pool.activeThreadCount()<<global_pool->activeThreadCount();
	global_pool->start(new MyRun);
	qDebug()<<pool.activeThreadCount()<<global_pool->activeThreadCount();
	pool.waitForDone();
	global_pool->waitForDone();
	qDebug()<<pool.activeThreadCount()<<global_pool->activeThreadCount();
	return 0;
}