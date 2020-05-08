#include "mainApplication.h"
#include <QTime>
#include <QDebug>
#include "main.h"

senderObject *g_pSenderObject = NULL;
receiverObject *g_pReceiverObject = NULL;
SubThread * g_pSubThread = NULL;
ThirdThread *g_pThirdThread = NULL;
QThread *g_pMainThread = NULL;

MainApplication *g_pApp = NULL;

QString getThreadName(QThread *pThread)
{
	if (pThread == g_pSubThread)
	{
		return ("SubThread");
	}
	else if (pThread == g_pMainThread)
	{
		return ("MainThread");
	}
	else if (pThread == g_pThirdThread)
	{
		return ("ThirdThread");
	}
	else
	{
		return ("Unknown");
	}
}

void senderObject::callEmitSignal()
{ 
	QString msg = QString("sendObject's = %1, recvObject's = %2, emit signal in %3").arg(getThreadName(g_pSenderObject->thread())).arg(getThreadName(g_pReceiverObject->thread())).arg(getThreadName(QThread::currentThread())); 
    qDebug() << msg; 

    emit someSignal(); 
	qDebug()<< "continue running after signal";
} 

// slot function connected to obj's someSignal 
void receiverObject::someSlot() 
{ 
	QString msg = QString("=> slot echo in %1").arg(getThreadName(QThread::currentThread())); 
    qDebug() << msg; 
}

SubThread::SubThread(QObject *parent)  : QThread(parent)
{

}

void SubThread::run()
{
	g_pSenderObject = new senderObject();
	g_pReceiverObject = new receiverObject();
    QObject::connect(g_pSenderObject, SIGNAL(someSignal()), g_pReceiverObject, SLOT(someSlot())); 


	exec();
}

ThirdThread::ThirdThread(QObject *parent)  : QThread(parent)
{
}

void ThirdThread::run()
{

	exec();
}

int main(int argc, char **argv)
{
	MainApplication app(argc, argv);
	g_pApp = &app;
	g_pMainThread = QThread::currentThread();


	g_pSubThread = new SubThread(g_pApp); //由主线程创建 
    g_pSubThread->start(); 
	QThread::sleep(1);
    g_pThirdThread = new ThirdThread(g_pApp); //由主线程创建 
    g_pThirdThread->start(); 



	return g_pApp->exec();
}
