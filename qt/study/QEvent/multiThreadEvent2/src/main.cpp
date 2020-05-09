#include "mainwindow.h"
#include "main.h"
#include <QApplication>
#include <QDebug>

ReceiverObject *g_pReceiverObject = NULL;
ShellInputThread *g_pShellInputThread = NULL;
QThread *g_pMainThread = NULL;

QString getThreadName(QThread *pThread)
{
	if (pThread == g_pShellInputThread)
	{
		return ("ShellInputThread");
	}
	else if (pThread == g_pMainThread)
	{
		return ("MainThread");
	}
	else
	{
		return ("Unknown");
	}
}

void ShellInputThread::run()
{
	g_pReceiverObject = new ReceiverObject();
	qDebug() << "ShellInputThread = " << this << "= " << QThread::currentThread();
	exec();
}

bool ReceiverObject::event(QEvent *pEvent)
{
    switch(pEvent->type())
    {
		case EVENT_CUSTOMER_EVENT:
			{
				CustomerEvent *customerEvent = dynamic_cast<CustomerEvent*>(pEvent);
				qDebug() << "Captured CUSTOMER_EVENT: " << customerEvent->getValueString() << " in " << getThreadName(QThread::currentThread());
				return true;
			}

		case EVENT_CUSTOMER_EVENT_1:
			qDebug() << "Captured EVENT_CUSTOMER_EVENT_1 in " << getThreadName(QThread::currentThread());
			return true;	
	}
    return QObject::event(pEvent);
}

int main(int argc, char *argv[])
{
#ifdef _WINDOWS_
	/* Memory Leak Checking */
	_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

	g_pMainThread = QThread::currentThread();

	QApplication a(argc, argv);
    MainWindow w;
    w.show();

	g_pShellInputThread = new ShellInputThread(&a); //由主线程创建 
    g_pShellInputThread->start(); 

    return a.exec();
}
