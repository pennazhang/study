
#include <MainWidget.h>
#include "MainApplication.h"

#include <QScreen>
#include <QFile>
#include <QIcon>
#include "IniSetting.h"

MainWidget *g_pMainWidget;
MainApplication *g_pApp;
IniSetting *g_pIniSetting;

int main(int argc, char *argv[])
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF|_CRTDBG_LEAK_CHECK_DF);
	
	QString iniFileName = QString("dpiFont.ini");
	g_pIniSetting = new IniSetting(iniFileName);
	g_pIniSetting->readAll();

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
	
    MainApplication app(argc, argv);
	g_pApp = &app;

	/* Set the default font, so we can use it in all kinds of windows version */
	{
		QFont font("Tahoma", 8);
		app.setFont(font);
	}

    QFile qss(":/dpiFont.qss");
    qss.open(QFile::ReadOnly);
    app.setStyleSheet(qss.readAll());
    qss.close();
    app.setWindowIcon(QIcon(":/logo.png"));

	/* Create the Main Window */
    MainWidget w;
	g_pMainWidget = &w;
    w.show();

	return app.exec();
}
