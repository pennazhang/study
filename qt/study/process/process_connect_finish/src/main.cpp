#include <QTextStream>
#include <QThread>
#include <QProcess>
#include <QString>
#include "mainApplication.h"


MainApplication *g_pApp = NULL;
QTextStream *g_pCout = NULL;

int main(int argc, char **argv)
{
	QString strCmd;
    MainApplication app(argc, argv);
	g_pApp = &app;

	QTextStream cout(stdout);
	QTextStream cin(stdin);
	g_pCout = &cout;

	cout << "Hello, world!\n";
	return g_pApp->exec();
}
