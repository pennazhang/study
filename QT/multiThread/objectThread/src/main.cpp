#include "mainApplication.h"
#include <QTime>
#include <QDebug>
#include "main.h"

ShellInputThread *g_pInputThread;
QObject gObject;

ShellInputThread::ShellInputThread(QObject* parent) 
    : QThread(parent)
 {
     m_pdata = new QObject;
     qDebug() << "m_myData belongs to " << m_pdata->thread()->currentThreadId();
 }

void ShellInputThread::run()
{
    qDebug() << "ShellInputThread ID = " << this->currentThreadId();
    qDebug() << "m_myData belongs to " << m_pdata->thread()->currentThreadId();
    qDebug() << "gObject belongs to " << gObject.thread()->currentThreadId();
}

int main(int argc, char **argv)
{
	QCoreApplication app(argc, argv);
	qDebug() << "[Main Thread " << QThread::currentThreadId() << "] Let's begin here...\n";
	
	ShellInputThread thread;
	thread.start();

	return g_pApp->exec();
}
