#include "mainApplication.h"
#include <QTime>
#include <QDebug>
#include "main.h"

ShellInputThread::ShellInputThread(int id, QObject * parent)
	: QThread(parent)
{
	m_id = id;
}

void ShellInputThread::run()
{
	for (int i = 0; i < 5; i++)
	{
		qDebug() << "[TID " << QThread::currentThreadId() << "] id "<< m_id << ", index " << i;
		QThread::msleep(500);
	}
	qDebug() << "[TID " << QThread::currentThreadId() << "] id "<< m_id << ", quit " << m_id;
}

MainApplication *g_pApp = NULL;
ShellInputThread *g_pInputThread;

int main(int argc, char **argv)
{
    MainApplication app(argc, argv);
	g_pApp = &app;

	qDebug() << "[TID " << QThread::currentThreadId() << "] Let's begin here...\n";
	
	ShellInputThread *pThread1 = new ShellInputThread(1);
	ShellInputThread *pThread2 = new ShellInputThread(2);
	pThread1->start();
	pThread2->start();

	return g_pApp->exec();
}
