#include <QTextStream>
#include <QThread>
#include <QProcess>
#include <QString>
#include <QFileInfo>
#include <QSettings>
#include <QFile> 
#include <QtXml>  
#include <QDomDocument> 
#include "main.h"
#include "Logger.h"

QTextStream g_stdOut(stdout);

	
int main(int argc, char **argv)
{
	QString strCmd;
    QCoreApplication app(argc, argv);

	g_pLogger = new Logger();

	QStringList childList;
	QString strCommand;
	QString strChoice;
	int nChoice;

	for (int i = 0; i < 5; i++)
	{
		QString strLog = QString("Current Count = %1").arg(i);
		LOG_ERROR(strLog);
	}

	return 0;
}