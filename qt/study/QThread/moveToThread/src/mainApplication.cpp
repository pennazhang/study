#include "mainApplication.h"
#include <QTime>
#include <QNetworkConfigurationManager>
#include <qnetworkaccessmanager.h>
#include <QUrl>
#include <QNetworkRequest>
#include <QDirIterator>
#include <QDebug>
#include <QThread>
#include <QString>
#include <QtCore/qmath.h> 
#include <QTextStream>
#include <QSocketNotifier>
#include "mainApplication.h"

void MainApplication::startWork(int work_id)
{
    Worker *pWorker = new Worker(work_id);
	pWorker->moveToThread(&m_workerThread);

	connect(this, SIGNAL(doWork(const int)), pWorker, SLOT(onDoWork(const int)));
    connect(pWorker, SIGNAL(workFinished(Worker *, int)), this, SLOT(onWorkFinished(Worker *, int)));

	m_workerThread.start();

	emit doWork(work_id);
}

void MainApplication::onWorkFinished(Worker *pWorker, int result)
{
	qDebug() << "[TID:" << QThread::currentThreadId() <<"]: "<< "workId" << "Finished!, result = " << result;
	delete pWorker;
}

MainApplication::MainApplication(int argc, char *argv[])
	: QCoreApplication(argc, argv)
{
	m_pTimer_CmdLoop = new QTimer(this);
	m_pTimer_CmdLoop->start(500);
	connect(m_pTimer_CmdLoop, SIGNAL(timeout()), this, SLOT(onTimer_CmdLoop()));
}

MainApplication::~MainApplication()
{
}

void MainApplication::onTimer_CmdLoop()
{
	static int count = 0;
	if (count < 20)
	{
		startWork(count);
	}
	else
	{
		exit(0);
	}
	count++;
}
