#include "shellServer.h"

ShellServer::ShellServer(QObject *parent) :
    QTcpServer(parent)
{

}

void ShellServer::start(qint16 port, QString strPassword)
{
    if(!this->listen(QHostAddress::Any,port))
    {
//        qDebug() << "Could not start server";
    }
    else
    {
//        qDebug() << "Listening...";
		m_password = strPassword;

        //notify connected objects
        emit OnStarted();
    }
}


void ShellServer::stop()
{
     this->close();
//     qDebug() << "Server stopped";

     //notify connected objects
     emit OnStopped();

}

void ShellServer::incomingConnection(int socketDescriptor)
{

	qDebug() << socketDescriptor << " Connecting..." << socketDescriptor << " Current Thread ID: " << QThread::currentThreadId(); 

    // We have a new connection
    // Every new connection will be run in a newly created thread
	ShellSocket *pShell = new ShellSocket(socketDescriptor, this, m_password);
    
    // connect signal/slot
    // once a thread is not needed, it will be beleted later
    connect(pShell, SIGNAL(finished()), pShell, SLOT(deleteLater()));
    
    pShell->run();
}
