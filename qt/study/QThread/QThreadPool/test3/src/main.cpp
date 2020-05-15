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
		int i = 3;
		qDebug() << "thread:" << QThread::currentThreadId() << ", this = " << this;
		while(i) 
		{
			i--;
			qDebug() << "thread:" << QThread::currentThreadId() << ", count = " << i;
			SleepThread::msleep(1000);
		}
	}
};

int main(int argc, char *argv[]) 
{
	QCoreApplication a(argc, argv);
	qDebug()<<"Main:"<<QThread::currentThreadId();

	QThreadPool *global_pool = QThreadPool::globalInstance();

	if (global_pool)
	{
		global_pool->setMaxThreadCount(2);
		global_pool->setExpiryTimeout(-1);
//		global_pool->setExpiryTimeout(2000);

		for (int i = 0; i < 3; i++)
		{
			MyRun *pMyRun = new MyRun();
			pMyRun->setAutoDelete(true);
			global_pool->start(pMyRun);
		}
		qDebug() << "Let's start the another task";
		MyRun *panotherRun = new MyRun();
		panotherRun->setAutoDelete(true);
		global_pool->start(panotherRun);
		
		global_pool->waitForDone();
//		global_pool->clear();
	}
	return 0;
}