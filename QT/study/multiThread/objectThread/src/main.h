
#ifndef _MAIN_H
#define _MAIN_H

#include <QThread>
#include <QDebug>

class ShellInputThread: public QThread
{
    Q_OBJECT
public:
    ShellInputThread(QObject* parent = 0);
    void run();

public:
    QObject  *m_pdata;
};

#endif
