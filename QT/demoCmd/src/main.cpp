#include <QDebug>
#include <QtCore/QCoreApplication>
#include <QThread>

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);

	qDebug() << "[TID " << QThread::currentThreadId() << "] Hello, world!\n";
	
	return 0;
//	return g_pApp->exec();
}
