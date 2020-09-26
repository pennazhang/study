
#ifndef ShellDebug_H
#define ShellDebug_H

#include <QTcpServer>
#include "shellSocket.h"

class ShellServer : public QTcpServer
{
    Q_OBJECT
public:

    /*!
    * Constructor
    * @param parent The parent object
    */
    explicit ShellServer(QObject *parent = 0);

    /*!
    * Starts the TCP Server
    * @param port The port to start the server on
    */
    void start(qint16 port, QString password);

    /*!
    * Stops the TCP Server
    */
    void stop();

signals:
    /*!
    * Signal indicates the TCP Server has stopped
    */
    void OnStarted();

    /*!
    * Signal indicates the TCP Server has stopped
    */
    void OnStopped();

public slots:


protected:

    /**
    * Processes incomming TCP connections
    * @param socketDescriptor The descriptor of the conection
    */
    void incomingConnection(int socketDescriptor);

private:
	QString m_password;

};

#endif // TCPSERVER_H
