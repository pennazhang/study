#include "mainApplication.h"
#include <QTime>
#include <QDebug>
#include <QString>
#include "main.h"

LogList::LogList() :
	m_mutex(QMutex::Recursive)
{
}

QString LogList::getLog()
{
	/* Let's lock the mutex first */
	QMutexLocker locker(&m_mutex);

	QString ret;
	if (m_logList.isEmpty())
	{
		return ("");
	}
	else
	{
		ret = m_logList.first();
		m_logList.removeFirst();
		qDebug() << "--" << ret << ", count" << m_logList.count();
		return (ret);
	}
}

bool LogList::putLog(const QString & log)
{
	/* Let's lock the mutex first */
	QMutexLocker locker(&m_mutex);

	if (m_logList.count() >= 10)
	{
		return false;
	}
	else
	{
		m_logList.append(log);
		qDebug() << "++" << log << ", count" << m_logList.count();
		return true;
	}
}

logInputThread::logInputThread(int id, QObject * parent)
	: QThread(parent)
{
	m_id = id;
}

void logInputThread::run()
{
	for (int i = 0; i < 50; i++)
	{
		QString logInfo = QString("[TID=%1] put %2").arg(m_id).arg(i);
		if (g_logList.putLog(logInfo) == false)
		{
			QThread::sleep(1);
			i--;
		}
	}
}


logOutputThread::logOutputThread(QObject * parent)
	: QThread(parent)
{
}
   
void logOutputThread::run()
{
	for (int i = 0; i < 1000; i++)
	{
		QString logInfo = g_logList.getLog();
		if (logInfo.isEmpty())
		{
			QThread::sleep(1);
			i--;
		}
	}
}

MainApplication *g_pApp = nullptr;
LogList g_logList;

int main(int argc, char **argv)
{
    MainApplication app(argc, argv);
	g_pApp = &app;

	qDebug() << "[TID " << QThread::currentThreadId() << "] Let's begin here...\n";
	
	logInputThread *pThread1 = new logInputThread(1);
	logInputThread *pThread2 = new logInputThread(2);
	logOutputThread *pOutputThread = new logOutputThread();

	pThread1->start();
	pThread2->start();
	pOutputThread->start();

	return g_pApp->exec();
}
