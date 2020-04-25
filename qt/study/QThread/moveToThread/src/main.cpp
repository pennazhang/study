
//#include <QtScript>
#include "mainApplication.h"
#include <QTime>
#include <QDebug>
#include "main.h"



MainApplication *g_pApp = nullptr;

int main(int argc, char **argv)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF|_CRTDBG_LEAK_CHECK_DF);

    MainApplication app(argc, argv);
	g_pApp = &app;

	return g_pApp->exec();
}

