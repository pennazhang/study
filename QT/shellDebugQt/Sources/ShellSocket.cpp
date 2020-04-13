#include <QWaitCondition>
#include "MainWidget.h"
#include "MainApplication.h"
#include <QDebug>
#include "ShellScript.h"
#include "ShellSocket.h"

ShellSocket* g_pActiveShell = nullptr;

ShellSocket::ShellSocket(int socketDescriptor, QObject *parent, QString password) :
    QObject(parent), m_sessionID(socketDescriptor), m_strPassword(password), m_banna("->")
{
}

void ShellSocket::run()
{
    // thread starts here
//    qDebug() << " Thread started";

    m_pSocket = new QTcpSocket();

    // set the ID
    if(!m_pSocket->setSocketDescriptor(m_sessionID))
    {
        // something's wrong, we just emit a signal
        emit error(m_pSocket->error());
        return;
    }

    // connect socket and signal
    // note - Qt::DirectConnection is used because it's multithreaded
    //        This makes the slot to be invoked immediately, when the signal is emitted.

    connect(m_pSocket, SIGNAL(readyRead()), this, SLOT(readyRead()), Qt::DirectConnection);
    connect(m_pSocket, SIGNAL(disconnected()), this, SLOT(disconnected()), Qt::DirectConnection);

//    qDebug() << m_sessionID << " session Connected";

	if (m_strPassword.isEmpty())
	{
	    m_authenticated = true;
		sendString(m_banna);
	}
	else
	{
	    m_authenticated = false;
		sendString("Enter a password: ");
	}
}

void ShellSocket::sendString(const QString & string)
{
	m_pSocket->write(string.toLatin1());
}

void ShellSocket::setBanna(QString banna)
{
	m_banna = banna;
}

void ShellSocket::readyRead()
{
    //Keep adding to the command buffer
    QByteArray Data = m_pSocket->readAll();
    QString CommandBuffer(Data);

    //Check to see if the CommandBuffer has a command
    if(CommandBuffer.endsWith('\n'))
    {
        //Process the command
        if(m_authenticated == false)
        {
            //Authenticate the client
			if (CommandBuffer.trimmed() == m_strPassword)
			{
				m_authenticated = true;
				sendString(m_banna);
			}
			else
			{
//				qDebug() << "Password is not valid. "<< CommandBuffer.trimmed() << " != " << m_strPassword; 
				sendString("Enter a password: ");
			}
        }
        else
        {
			QString strTemp = CommandBuffer.trimmed();

			if ((strTemp.length() >= 1) && (strTemp[strTemp.length()-1]  == '\\'))
			{
				int index = CommandBuffer.lastIndexOf(QChar('\\'));
//				qDebug() << "Found \\ at" <<index;
				CommandBuffer.resize(index);
				CommandBuffer.append("\r\n");
//				qDebug() << "new line:" << CommandBuffer;
				m_strCmd += CommandBuffer;
			}
			else
			{
				m_strCmd += CommandBuffer;


				if (m_strCmd.trimmed().isEmpty())
				{
					sendString(m_banna);					
					return;
				}

				if (m_strCmd.trimmed() == "exit")
				{
					m_pSocket->flush();
					disconnected();
					return;
				}

				QString strResult;
				if ((m_strCmd.trimmed() == "help") || (m_strCmd.trimmed() == "lkup")) 
				{
					strResult = QString("Run: shell.lkup(\"\", \"\") for more help\r\n\r\n");
				}
				else
				{
					int result = 0;
					QString failReason;

					g_pActiveShell = this;
					bool runFlag = g_pShellScript->runCmd(m_strCmd, result, failReason);
					g_pActiveShell = nullptr;

					if (runFlag == true)
					{
						strResult.sprintf("\r\nExecute successfully, result = %d = 0x%X\r\n\r\n", result, (quint32)result);
					}
					else
					{
						strResult = QString("Running cmd : ") + m_strCmd + QString("Execute failed! Reason: ") + failReason + QString("\r\n\r\n");					
					}
				}
				sendString(strResult);
				m_strCmd.clear();
				sendString(m_banna);
			}
        }
    }
}

void ShellSocket::disconnected()
{
    qDebug() << m_sessionID << " session disconnected";

    //Cleanup
    m_pSocket->deleteLater();
}
