#ifndef MAIN_APPLICATION_H
#define MAIN_APPLICATION_H

#include <QtCore/QCoreApplication>
#include <QTextStream>
#include <QProcess>
#include <QTimer>

class MainApplication : public QCoreApplication
{
    Q_OBJECT
public:	
	MainApplication(int argc, char *argv[]);
	~MainApplication();
#ifdef _WINDOWS_
	void startWindowsScript(const QString &strScriptFile);
#endif
#ifdef _LINUX_
	void startLinuxScript(const QString &strScriptFile);
#endif

public slots:
	void onProcesFinished(int , QProcess::ExitStatus);
	void onTimer_CmdLoop();
	void onReadStdout();
	void onReadStderr();

protected:
	QProcess m_process;
	QTimer *m_pTimer_CmdLoop;
};


extern MainApplication *g_pApp;
extern QTextStream *g_pCout;
#endif
