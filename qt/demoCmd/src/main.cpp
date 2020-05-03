#include <QTextStream>
#include <QThread>
#include <QProcess>
#include <QString>
#include "mainApplication.h"


MainApplication *g_pApp = NULL;
QTextStream g_stdOut(stdout), g_stdIn(stdin);

int main(int argc, char **argv)
{
	QString strCmd;
    MainApplication app(argc, argv);
	g_pApp = &app;


	g_stdOut << "Hello, world!\n";
	return g_pApp->exec();
}
