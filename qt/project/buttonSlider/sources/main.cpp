#include "mainwindow.h"
#include <QApplication>
#include <QFile>

int main(int argc, char *argv[])
{
#ifdef _WINDOWS_
	/* Memory Leak Checking */
	_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif
    QApplication app(argc, argv);

    QFile qss(":/demoGui.qss");
    qss.open(QFile::ReadOnly);
    app.setStyleSheet(qss.readAll());
    qss.close();
    app.setWindowIcon(QIcon(":/logo.png"));

    MainWindow w;
    w.show();
    return app.exec();
}
