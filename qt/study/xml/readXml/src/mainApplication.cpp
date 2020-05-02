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

#define QUIT_TIME_IN_SECOND 30

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
	static int index = 0;
	index++;

	*g_pCout << index << "... ";
	g_pCout->flush();
	if (index == 10)
	{
		*g_pCout << endl;
		exit(0);
	}
}


