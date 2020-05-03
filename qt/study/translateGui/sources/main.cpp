#include "mainwindow.h"

#include <QApplication>
#include <QFile>
#include <QIcon>

QApplication *g_pApp = NULL;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
	g_pApp = &app;
	/* Set the default font, so we can use it in all kinds of windows version */
	{
//		QFont font("Tahoma", 12);
//		QFont font = app.font();
//		font.setPointSize(10);

//		app.setFont(font);
		QFont font("Tahoma", 10);
		app.setFont(font);
	}

    QFile qss(":/translateGui.qss");
    qss.open(QFile::ReadOnly);
    app.setStyleSheet(qss.readAll());
    qss.close();
    app.setWindowIcon(QIcon(":/logo.png"));

    MainWindow w;
    w.show();
    return app.exec();
}
