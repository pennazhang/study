
#ifndef _MAIN_H
#define _MAIN_H

#include <QThread>
#include <QDebug>
#include <QTimer>

class ShellInputThread: public QThread
{
    Q_OBJECT
public:
    ShellInputThread(QObject* parent = 0);
    void run();

private slots:
    void onTimeout()
    {
        qDebug()<<"Thread::onTimeout get called from : "<< QThread::currentThreadId();
    }

public:
    QTimer *m_pTimer;;
};

#endif
