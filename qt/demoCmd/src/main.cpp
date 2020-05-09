#include <QTextStream>
#include <QThread>
#include <QProcess>
#include <QString>
#include "mainApplication.h"


MainApplication *g_pApp = NULL;
QTextStream g_stdOut(stdout), g_stdIn(stdin);

int main(int argc, char **argv)
{
#ifdef _WINDOWS_
	/* Memory Leak Checking */
	_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

	QString strCmd;
    MainApplication app(argc, argv);
	g_pApp = &app;


	g_stdOut << "Hello, world!\n";
	return g_pApp->exec();
}
