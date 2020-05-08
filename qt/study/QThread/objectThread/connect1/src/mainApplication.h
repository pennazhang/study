#ifndef MAIN_APPLICATION_H
#define MAIN_APPLICATION_H

#include <QtCore/QCoreApplication>
#include <QFile>
#include <QDebug>
#include <QTimer>
#include <QList>
#include <QThread>

class MainApplication : public QCoreApplication
{
    Q_OBJECT
public:	
	MainApplication(int argc, char *argv[]);
	~MainApplication();

public slots:
	void onTimer_CmdLoop();

protected:
	QTimer *m_pTimer_CmdLoop;

};

extern MainApplication *g_pApp;

#endif
