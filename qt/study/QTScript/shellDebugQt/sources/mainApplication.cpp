#include "mainApplication.h"
#include <QTime>

MainApplication::MainApplication(int &argc, char *argv[])
	: QApplication(argc, argv)
{
}

void MainApplication::mSleep(int ms)
{
    QTime dieTime = QTime::currentTime().addMSecs(ms);

	for ( ; QTime::currentTime() <= dieTime; )
	{
		QCoreApplication::processEvents( QEventLoop::AllEvents);
	}
}
