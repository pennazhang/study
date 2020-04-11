
#include <MainWidget.h>
#include "MainApplication.h"
#include "ShellServer.h"
#include "ShellScript.h"
#include <QScreen>
#include <QFile>
#include <QIcon>

MainWidget *g_pMainWidget;
MainApplication *g_pApp;

int main(int argc, char *argv[])
{
//	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF|_CRTDBG_LEAK_CHECK_DF);

    MainApplication app(argc, argv);
	g_pApp = &app;

	/* Set the default font, so we can use it in all kinds of windows version */
	{
//		QFont font("Tahoma", 12);
		QFont font = g_pApp->font();
		font.setPointSize(10);

		QScreen *pScreen = g_pApp->primaryScreen ();
		QRect mm =pScreen->availableGeometry() ;
		int screen_width = mm.width();
		int screen_height = mm.height();
		float currentDPI = pScreen->logicalDotsPerInch();
		QMessageBox::information(nullptr, "Information", QString("logicalDotsPerInch = %1, screenWidth = %2, screenHeight = %3").arg(currentDPI).arg(screen_width).arg(screen_height), "OK");

//		int pointSize = qRound((float)8 * 96 / currentDPI);
//		g_defaultFontSize = pointSize;
//		font.setPointSize(pointSize);
		
		app.setFont(font);
	}

    QFile qss(":/shellDebugQt.qss");
    qss.open(QFile::ReadOnly);
    app.setStyleSheet(qss.readAll());
    qss.close();
    app.setWindowIcon(QIcon(":/logo.png"));

	/* Create the Main Window */
    MainWidget w;
	g_pMainWidget = &w;
    w.show();

	ShellScript shellScript;
	g_pShellScript = &shellScript;


	/* For the formal product, we must disable the network service */
#if 1
	/* Let's create a Telnet Shell for debug and maintaince*/
	ShellServer debugServer;
	debugServer.start(5023, "1234");
#endif

	return app.exec();
}
