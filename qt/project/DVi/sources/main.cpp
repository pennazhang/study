#include "MainWidget.h"
#include "MainApplication.h"
#include <QSharedMemory>
#include <QMessageBox>
#include "SQLiteDB.h"
#include <QTextStream>
#include <QDir>
#include <QFile>
#include <QScreen>
#include <QFontDatabase>
#include "IniSetting.h"
#include "simpleQtLogger.h"
#include "WaittingWidget.h"
#include "commonLib.h"

MainApplication *g_pApp = nullptr;
IniSetting *g_pIniSetting;

QTextStream g_log;
QFile g_logFile;
int g_defaultFontSize = 8;

QSettings *g_pRegisterSetting = nullptr;	/* Access Windows Register, the Key is:  "Computer\HKEY_CURRENT_USER\Software\Harman\DVi" */

int main(int argc, char *argv[])
{
	int processID;

	g_pRegisterSetting = new QSettings("Harman", "DVi");

	{
		/* Read DVi.ini */
		QString iniFileName = QString("%1/DVi.ini").arg(QDir::tempPath());
		qDebug() << iniFileName;
		g_pIniSetting = new IniSetting(iniFileName);
		g_pIniSetting->readAll();

		/* Scale in windows */
		if (g_pIniSetting->m_scaleFlag == true)
		{
			qputenv("QT_AUTO_SCREEN_SCALE_FACTOR", "1");

			QString scaleFactorString =  QString::number(g_pIniSetting->m_scaleFactor, 'f', 1);
			qputenv("QT_SCALE_FACTOR", scaleFactorString.toLatin1());
			QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling); 
		}
		else
		{
			QGuiApplication::setAttribute(Qt::AA_DisableHighDpiScaling); 
		}

		/* get process ID */
		processID = g_pIniSetting->getPID();
		int nextProcessID = processID + 1;
		if (nextProcessID >= 100)
		{
			nextProcessID = 0;
		}
		g_pIniSetting->setPID(nextProcessID);
	}

	MainApplication app(argc, argv);
	app.installEventFilter(new mouseEventFilter());
	app.setPid(processID);
	logInit(QDir::tempPath());
#if 0
	/* We just need only one instance */
    QSharedMemory shared("62d60669-bba4-4a94-99bb-b964893a7e85");
	if( !shared.create( 512, QSharedMemory::ReadWrite) )
    {
//		qWarning() << "Can't start more than one instance of the application.";


		QMessageBox::warning(nullptr, QObject::tr("Warning"), QObject::tr("Another DVi Application is already running now. You can't start more than one instance of the application!"), 
			QObject::tr("OK"));
		exit(0);
	}
#endif
	g_pApp = &app;

    QFile qss(":DVi.qss");
    qss.open(QFile::ReadOnly);
    app.setStyleSheet(qss.readAll());
    qss.close();

	/* Set the default font, so we can use it in all kinds of windows version */
	{
		QFont font("Tahoma");
//		int fontId = QFontDatabase::addApplicationFont(":/binaries/msyh.ttf"); 
//		QString msyh = QFontDatabase::applicationFontFamilies ( fontId ).at(0); 
//		QFont font(msyh); 

		QFontDatabase database;
		foreach(const QString &family, database.families(QFontDatabase::SimplifiedChinese))
		{
			qDebug() << family;
		}

		QScreen *pScreen = g_pApp->primaryScreen ();
		float currentDPI = pScreen->logicalDotsPerInch();
		int pointSize = qRound((float)8 * 96 / currentDPI);
		g_defaultFontSize = pointSize;
		font.setPointSize(pointSize);
		
//		QMessageBox::information(nullptr, "Information", QString("pointSize = %1, logicalDotsPerInch = %2, scaleFlag = %3, scaleFactor = %4").arg(pointSize).arg(currentDPI).arg(g_pIniSetting->m_scaleFlag).arg(g_pIniSetting->m_scaleFactor), "OK");
		app.setFont(font);
	}

    MainWidget w;
	g_pMainWidget = &w;
    w.show();

#if 1
	QStringList fileList;
//	fileList << "myFile1.txt" << "myFile2.txt";
//	g_pApp->zipFilesToFile("myfile.zip", fileList);
//	g_pApp->unzipFileToFolder("test.dev", "myFolder");
#endif

#ifdef LOG_FILE
	QString logFileName	= QString("%1/DViLog.txt").arg(QDir::tempPath());
	g_logFile.setFileName(logFileName);
	qDebug() << "Set log file to: " << logFileName;
	g_logFile.open(QIODevice::WriteOnly);
	g_log.setDevice(&g_logFile);
#endif

    return app.exec();
}
