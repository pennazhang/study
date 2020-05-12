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

Stdin *g_pStdin = NULL;

MainApplication::MainApplication(int argc, char *argv[])
	: QCoreApplication(argc, argv)
{
	g_pStdin = new Stdin();
	g_pStdin->moveToThread(&m_stdinThread);
    connect(g_pStdin, SIGNAL(inputLine(const QString &)), this, SLOT(onStdinInput(const QString & )));
    connect(this, SIGNAL(startInput()), g_pStdin, SLOT(onStartInput()));

    m_stdinThread.start();
}

MainApplication::~MainApplication()
{
	m_stdinThread.exit();
	m_stdinThread.wait();
}


void MainApplication::startStdInput()
{
	emit startInput();
}

void MainApplication::onStdinInput(const QString & strInputLine)
{
	qDebug() << "Input : " << strInputLine;
}

void Stdin::onStartInput()
{
	QString strLine;
	for (;;)
	{
		strLine = m_stdIn.readLine();
		emit inputLine(strLine);
	}
}
