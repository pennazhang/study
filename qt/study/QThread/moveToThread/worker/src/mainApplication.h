#ifndef MAIN_APPLICATION_H
#define MAIN_APPLICATION_H

#include <QtCore/QCoreApplication>
#include <QFile>
#include <QDebug>
#include <QTimer>
#include <QList>
#include <QThread>

class Worker:public QObject
{
    Q_OBJECT
public:
    Worker(int workId, QObject* parent = nullptr)
		: QObject(parent)
	{
		m_workId = workId;
	}

public slots:
     // doWork here...
    void onDoWork(const int startCounter)
    {
		int result = startCounter;
       qDebug() << "[TID:" << QThread::currentThreadId() <<"]: "<<"workID:" << m_workId << "start work at =" << startCounter;

	   for(int i = 0; i!=100000; ++i)
       {
            result++;
       }
       // end of work.
       qDebug() << "[TID:" << QThread::currentThreadId() <<"]: "<<"workID:" << m_workId << "Finish the work and sent the resultReady signal, result =" << startCounter;
       emit workFinished(this, result);
    }

signals:
    void workFinished(Worker *, int result);

protected:
	int m_workId;
};

class MainApplication : public QCoreApplication
{
    Q_OBJECT
public:	
	MainApplication(int argc, char *argv[]);
	~MainApplication();
	void upgradeFirmware();
	void startWork(int param);

signals:
    void doWork(const int);

public slots:
	void onWorkFinished(Worker *, int result);
	void onTimer_CmdLoop();

protected:
	QTimer *m_pTimer_CmdLoop;
    QThread m_workerThread;

};

extern MainApplication *g_pApp;

#endif
