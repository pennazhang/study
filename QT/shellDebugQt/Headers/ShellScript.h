#ifndef SHELLSCRIPT_H
#define SHELLSCRIPT_H

#include <QObject>
#include <QTcpSocket>
#include <QDebug>
#include <QByteArray>
#include <QThread>
#include <QScriptEngine>
#include <QStringList>

void outputString(const QString & string);

struct ObjectInfo
{
	QString m_objectName;
	QStringList m_functionList;
};

class ShellScript: public QObject
{
	Q_OBJECT

public:
	ShellScript();
	~ShellScript();
	bool runCmd(QString strCmd, int& ret, QString &failReason);
	void addToHelpInfo(const QString & objectName, const QStringList & funcList);

public slots:
	void lkup(const QString & objectName, const QString&funcName);
	void sleep(int ms);
	void showParam(QString str, int value);
	void showParam(QString str, qreal value);

protected:
    QScriptEngine *m_pScriptEngine;
    QScriptValue *m_pScriptValue;
	QList <ObjectInfo *> m_helpInfo;
};

extern ShellScript * g_pShellScript;

#endif // TCPCLIENT_H
