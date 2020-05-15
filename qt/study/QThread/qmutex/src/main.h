
#ifndef _MAIN_H
#define _MAIN_H

#include <QThread>
#include <QStringList>
#include <QMutex>

class LogList
{
public:
	LogList();
	QString getLog();
	bool putLog(const QString &);

protected:
	QStringList m_logList;
	QMutex m_mutex;
};

class logInputThread: public QThread
{
    Q_OBJECT
public:
    logInputThread(int id, QObject * parent = 0);

protected:
    void run();

protected:
	int m_id;
};

class logOutputThread: public QThread
{
    Q_OBJECT
public:
    logOutputThread(QObject * parent = 0);

protected:
    void run();

protected:
	int m_id;
};

extern LogList g_logList;

#endif
