
//#include <QtScript>
#include "mainApplication.h"
#include <QTime>
#include <QDebug>
#include "main.h"

MainApplication *g_pApp = nullptr;

int main(int argc, char **argv)
{
#ifdef _WINDOWS_
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF|_CRTDBG_LEAK_CHECK_DF);
#endif

    MainApplication app(argc, argv);
	g_pApp = &app;

	app.startStdInput();

	return g_pApp->exec();
}

