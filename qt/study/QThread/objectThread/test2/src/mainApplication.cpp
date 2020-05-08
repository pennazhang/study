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
	// timer func called here...
}
