
#ifndef _MAIN_H
#define _MAIN_H

#include <QThread>

class ShellInputThread: public QThread
{
    Q_OBJECT
public:
    ShellInputThread(int id, QObject * parent = 0);

protected:
    void run();

protected:
	int m_id;
};

#endif
