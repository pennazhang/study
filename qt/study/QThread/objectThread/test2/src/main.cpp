#include "mainApplication.h"
#include <QTime>
#include <QDebug>
#include "main.h"

MainApplication *g_pApp = NULL;
QObject g_object;

ShellInputThread::ShellInputThread(QObject* parent) 
    : QThread(parent)
 {
     m_pTimer = new QTimer(this);
	 m_pTimer->start(1000);
	 connect(m_pTimer, SIGNAL(timeout()), this,  SLOT(onTimeout()));
 }

void ShellInputThread::run()
{
    qDebug()<<"From worker thread: "<<currentThreadId();
//    QTimer timer;
//    connect(&timer, SIGNAL(timeout()), this, SLOT(onTimeout()));
//    timer.start(1000);

    exec();
}

int main(int argc, char **argv)
{
	MainApplication app(argc, argv);
	g_pApp = &app;
	qDebug() << "Main Thread =  " << QThread::currentThreadId();
	
	ShellInputThread thread;
	thread.start();

	return g_pApp->exec();
}
