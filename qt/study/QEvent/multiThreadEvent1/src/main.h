
#ifndef _MAIN_H
#define _MAIN_H

#include <QThread>

class ShellInputThread: public QThread
{
    Q_OBJECT
public:
	ShellInputThread(QObject * parent = 0) : QThread(parent) {};

protected:
    void run();
	bool event(QEvent *event);
};

extern ShellInputThread *g_pShellInputThread;

#endif
