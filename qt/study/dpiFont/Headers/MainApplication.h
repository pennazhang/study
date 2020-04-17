#ifndef MAIN_APPLICATION_H
#define MAIN_APPLICATION_H

#include <QApplication>

class MainApplication : public QApplication
{
    Q_OBJECT
public:	
	MainApplication(int &argc, char *argv[]);
	
	void mSleep(int ms);
};

extern MainApplication *g_pApp;

#endif // MAINWIDGET_H
