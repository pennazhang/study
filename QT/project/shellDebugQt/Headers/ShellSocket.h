#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QDebug>
#include <QByteArray>
#include <QThread>
#include <QScriptEngine>

void outputString(const QString & string);

class ShellSocket : public QObject
{
    Q_OBJECT
public:
    explicit ShellSocket(int socketDescriptor, QObject *parent, QString password);
    void run();
	void setBanna(QString banna);
	void sendString(const QString & string);

signals:
    void error(QTcpSocket::SocketError socketerror);
    
protected slots:
    void readyRead();
    void disconnected();

public slots:
//	void help();

protected:
    int m_sessionID;          //! The Socket ID
    QTcpSocket *m_pSocket;

	/* 
		TRUE:  Password is ok or no password is needed.
		FALSE: Password is needed and not verified yet.
	*/
    bool m_authenticated;     //! Determines if the client is authenticated
	QString m_strPassword;
	QString m_banna;
	QString m_strCmd;

};

extern ShellSocket *g_pActiveShell;

#endif // TCPCLIENT_H
