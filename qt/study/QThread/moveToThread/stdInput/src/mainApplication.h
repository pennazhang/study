#ifndef MAIN_APPLICATION_H
#define MAIN_APPLICATION_H

#include <QtCore/QCoreApplication>
#include <QFile>
#include <QDebug>
#include <QTimer>
#include <QList>
#include <QThread>
#include <QTextStream>

class Stdin: public QObject
{
	Q_OBJECT
public:
	Stdin(QObject *parent = NULL) : QObject(parent), m_stdIn(stdin) {};
	
signals:
    void inputLine(const QString & strInputLine);

public slots:
	void onStartInput();

protected:
	QTextStream m_stdIn;
};

class MainApplication : public QCoreApplication
{
    Q_OBJECT
public:	
	MainApplication(int argc, char *argv[]);
	~MainApplication();
	void startStdInput();

signals:
    void startInput();

public slots:
	void onStdinInput(const QString & strInputLine);

protected:
    QThread m_stdinThread;

};

extern MainApplication *g_pApp;

#endif
