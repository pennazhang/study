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

	connect(&m_process, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(onProcesFinished(int, QProcess::ExitStatus)));

	connect(&m_process, SIGNAL(readyReadStandardOutput()), this, SLOT(onReadStdout()));
	connect(&m_process, SIGNAL(readyReadStandardError()), this, SLOT(onReadStderr()));
}

void MainApplication::onReadStdout()
{
	if (m_process.state() == QProcess::Running)
	{
	    while(m_process.canReadLine())
		{
	        g_stdOut << m_process.readLine() << endl;
	    }
	}
}

void MainApplication::onReadStderr()
{
	if (m_process.state() == QProcess::Running)
	{
		QString strOutput = m_process.readAllStandardError();
		g_stdOut << "stderr: " << strOutput;
	}
}

MainApplication::~MainApplication()
{
}

void MainApplication::onTimer_CmdLoop()
{
	static int index = 0;
	index++;
	
	switch (index)
	{
	case 1:
		g_stdOut << QString("This program will quit in %1 seconds...").arg(QUIT_TIME_IN_SECOND) << endl;
		break;
	case 2:
		g_stdOut << "Starting script..." << endl;
#ifdef _WINDOWS_
		startWindowsScript("c:/temp/test.bat");
#endif
#ifdef _LINUX_
		startLinuxScript("/git/temp/test.sh");
#endif
		break;
	case QUIT_TIME_IN_SECOND:
		exit(0);
	default:
		break;
	}
}

#ifdef _WINDOWS_
void MainApplication::startWindowsScript(const QString &strScriptFile)
{
	m_process.setProgram("cmd");
    QStringList argument;
    argument<<"/c" << strScriptFile;
    m_process.setArguments(argument);
    m_process.start();
}
#endif

#ifdef _LINUX_
void MainApplication::startLinuxScript(const QString &strScriptFile)
{
    m_process.start(QString("bash -c %1").arg(strScriptFile));
}
#endif


void MainApplication::onProcesFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
	g_stdOut << "Recv the notification of Process, exitCode = " <<  exitCode  <<", exitStatus = " << exitStatus <<  endl;
	m_process.waitForFinished();
    QString strResult = m_process.readAllStandardOutput();
	g_stdOut << "onProcesFinished:\n" << strResult << endl;

//	exit(0);
}
