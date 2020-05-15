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
	QThreadPool m;
	MyRun *run=new MyRun;
	if(!run->autoDelete()) 
	{
		qDebug()<<"QRunnable's autoDelete default value is not true";
		run->setAutoDelete(true);
	}
	qDebug()<< "maxThreadCount() = " << m.maxThreadCount();
	qDebug() << "expiryTimeout() = " << m.expiryTimeout();
	qDebug()<< "activeThreadCount() = " << m.activeThreadCount();
	qDebug() << "start(run)";
	m.start(run);
	qDebug()<< "activeThreadCount() = " << m.activeThreadCount();
	qDebug() << "waitForDone()";
	m.waitForDone();
	qDebug()<< "activeThreadCount() = " << m.activeThreadCount();
	return 0;
}
