#include "mainApplication.h"
#include <QTime>
#include <QDebug>
#include "main.h"

MainApplication *g_pApp = NULL;
QObject g_object;

ShellInputThread::ShellInputThread(QObject* parent) 
    : QThread(parent)
 {
     m_myData = new QObject;
     qDebug() << "m_myData belongs to " << m_myData->thread();
 }

void ShellInputThread::run()
{
    qDebug() << "ShellInputThread = " << this;
    qDebug() << "m_myData belongs to " << m_myData->thread();
    qDebug() << "g_object belongs to " << g_object.thread();
	delete m_myData;
	m_myData = new QObject;
	qDebug() << "re-create m_myData";
    qDebug() << "m_myData belongs to " << m_myData->thread();
}

int main(int argc, char **argv)
{
	MainApplication app(argc, argv);
	g_pApp = &app;
	qDebug() << "Main Thread =  " << g_pApp->thread();
	
	ShellInputThread thread;
	thread.start();

	return g_pApp->exec();
}
