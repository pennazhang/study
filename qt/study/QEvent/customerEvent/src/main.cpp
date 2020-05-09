#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
#ifdef _WINDOWS_
	/* Memory Leak Checking */
	_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
