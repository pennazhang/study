
#include <MainWidget.h>
#include "MainApplication.h"
#include "ShellServer.h"
#include "ShellScript.h"

MainWidget *g_pMainWidget;
MainApplication *g_pApp;

int main(int argc, char *argv[])
{
//	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF|_CRTDBG_LEAK_CHECK_DF);

    MainApplication app(argc, argv);
	g_pApp = &app;

//    QFile qss(":kx200.qss");
//    qss.open(QFile::ReadOnly);
//    app.setStyleSheet(qss.readAll());
//    qss.close();

	/* Set the default font, so we can use it in all kinds of windows version */
	QFont font("Tahoma", 8);
	app.setFont(font);

//    app.setWindowIcon(QIcon(":/logo.png"));

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
