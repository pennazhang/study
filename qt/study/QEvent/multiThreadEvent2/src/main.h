
#ifndef _MAIN_H
#define _MAIN_H

#include <QThread>

class ReceiverObject: public QObject
{
    Q_OBJECT
public:
	ReceiverObject(QObject * parent = 0) : QObject(parent) {}
	bool event(QEvent *event);
};

class ShellInputThread: public QThread
{
    Q_OBJECT
public:
	ShellInputThread(QObject * parent = 0) : QThread(parent) {};
protected:
    void run();
};

extern ShellInputThread *g_pShellInputThread;
extern ReceiverObject *g_pReceiverObject;

#endif
