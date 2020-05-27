#include "DeviceSocket.h"
#include <qdebug.h>
#include "mainApplication.h"
#include "mainWidget.h"
#include "commonLib.h"
#include "mainApplication.h"
#include "simpleQtLogger.h"

#define LOCAL_PORT	10050
#define DEVICE_PORT	19272

#define SEND_INTERVAL_IN_MS  500

DeviceSocket::DeviceSocket()
    : QObject(nullptr)
{
    m_pSocket = new QTcpSocket(this);
	m_bConnectedFlag = false;
	m_commandFlag[CMD_WAIT_LOGIN] = true;
	m_pTextStream = nullptr;

	m_pTimer = new QTimer(this);
	connect(m_pTimer, SIGNAL(timeout()), this, SLOT(onTimer_sendBuffer()));
	m_pTimer->start(SEND_INTERVAL_IN_MS);
}

DeviceSocket::~DeviceSocket()
{
	disconnect();
	delete m_pSocket;
}

QHostAddress DeviceSocket::getLocalAddr()
{
	return (m_localAddress);
}

QHostAddress DeviceSocket::getDeviceAddr()
{
	return (m_deviceAddress);
}

void DeviceSocket::disconnect()
{
	if (m_bConnectedFlag == true)
	{
		m_pSocket->disconnect();
		m_pSocket->disconnectFromHost();
	}

	if (m_pTextStream != nullptr)
	{
		delete m_pTextStream;
		m_pTextStream = nullptr;
	}

	m_bConnectedFlag = false;
	m_commandFlag[CMD_WAIT_LOGIN] = true;
}

/* This is a block function. In this function, the device should finish the connection and login as administrator */
bool DeviceSocket::setConnection(QHostAddress localAddress, QHostAddress deviceAddress)
{
	m_localAddress = localAddress;
	m_deviceAddress = deviceAddress;

	disconnect();

    // this is not blocking call
    m_pSocket->connectToHost(m_deviceAddress, DEVICE_PORT);
	L_INFO("SOCKET CONNECTING... " + m_deviceAddress.toString());

    // we need to wait...
    if(!m_pSocket->waitForConnected(4000))
    {
		L_WARN("Socket failed to connected to host:" + m_deviceAddress.toString());
		return (false);
    }

    connect(m_pSocket, SIGNAL(readyRead()),this, SLOT(readyRead()));
	m_bConnectedFlag = true;
	m_pTextStream = new QTextStream(m_pSocket);

	m_shakeHandFlag = false;
	m_nodeList.clear();
	m_valueList.clear();

	/* wait for connecttion as administrator */
	m_commandFlag[CMD_WAIT_LOGIN] = true;
	for (int i = 0; i < 200; i++)
	{
		g_pApp->msleep(10);
		if (m_commandFlag[CMD_WAIT_LOGIN] == false)
		{
			return (true);
		}
	}
	return (false);
}

void DeviceSocket::readyRead()
{
    while(m_pTextStream->device()->canReadLine())
    {
        QString dataFromTCP = m_pTextStream->readLine();
		onDispatchData(dataFromTCP);

		m_shakeHandFlag = true;
    }
}

typedef void ( *DispatchCrossRoadConsoleFunc)(DeviceSocket *m_pSocket, QStringList cmdList); 

typedef struct
{
	QString							m_strParam0;
	QString							m_strParam1;
	int								m_commandType;
	DispatchCrossRoadConsoleFunc	m_pFunc;
} DISPATCH_FUNC_PARAM3;

DISPATCH_FUNC_PARAM3 s_param3[] = {
	{ "",		"",						CMD_RESERVED,				nullptr				},
#if 0
	{ "set",	NODE_OUTPUT_VOLUME,		CMD_NULL,					onRecvOutputVolume	},
	{ "subr",	NODE_OUTPUT_VOLUME,		CMD_NULL,					onRecvOutputVolume	},
	{ "set",	NODE_INPUT_VOLUME,		CMD_NULL,					onRecvInputVolume	},
	{ "subr",	NODE_INPUT_VOLUME,		CMD_NULL,					onRecvInputVolume	},
	{ "set",	NODE_USB_STATUS,		CMD_NULL,					onRecvUSBStatus		},
	{ "subr",	NODE_USB_STATUS,		CMD_NULL,					onRecvUSBStatus		},
	{ "get",	NODE_SPL_VALUE,			CMD_NULL,					onRecvSPLValue		},
	{ "get",	NODE_SYSTEM_TIME,				CMD_NULL,							onGetSystemTime		},
	{ "set",	NODE_RTA_METER,					CMD_NULL,			onRecvRTAValue		}
#endif
};

QStringList splitCommand(QString strLine)
{
	QString line = QString(" ") + strLine;
	QStringList list;

	int pos = 0;
//	QRegExp rx("\s+([^\"\s]+)|\s+\"(.*)\"" );
	QRegExp rx("^\\s+([^\"\\s]+)|\\s+\"([^\"]*)\"");
	QRegExp rx1("^\\s+([^\"\\s]+)");
	QRegExp rx2("^\\s+\"([^\"]*)\"");
	while ((pos = rx.indexIn(line, pos)) != -1) 
	{
		QString subString = rx.cap(0);
//		qDebug() << "subString" << subString;
		QString matchString;
		if (rx1.indexIn(subString, 0) != -1)
		{
			matchString = rx1.cap(1);
		}
		else if (rx2.indexIn(subString, 0) != -1)
		{
			matchString = rx2.cap(1);
		}
		else
		{
			L_ERROR("splitCommand Error: " + strLine);
			list.clear();
			return list;
		}

		list << matchString;
//		qDebug() << matchString;

		pos += rx.matchedLength();
	}
	return (list);
}

void DeviceSocket::onDispatchData(QString strLine)
{
#ifdef _SOCKET_DEBUG
    qDebug() << "Recv data from Socket: << " << strLine.toStdString().c_str();
#endif

	/* Add shake hand here */

	if (BIT(g_debug, LOG_SOCKET_MESSAGE))
	{
		L_DEBUG("Recv from device " + getDeviceAddr().toString() + " :" + strLine);
	}

	if(strLine == "HiQnet Console")
    {
        QString command = QString("connect administrator administrator\n");
        writeToSocket(command.toStdString().c_str());
		return;
    }
	else if (strLine == "connect logged in as administrator")
	{
		m_commandFlag[CMD_WAIT_LOGIN] = false;
		return;
	}

	QStringList cmdList = splitCommand(strLine);

	/* Dispatch "error" info */
	if (cmdList.length() >= 1)
	{
		QString cmd = cmdList[0];
		if (cmd == QString("error"))
		{
			L_WARN("Recv error information from device: " + strLine);


			if (cmdList.length() == 2)
			{
				/* For example : error "\\Node\SV\Theater\r" */
				if (s_param3[0].m_strParam1 == cmdList[1])
				{
					m_commandFlag[CMD_RESERVED] = false;
					m_strCommandList[CMD_RESERVED].clear();
				}
			}
			return;
		}
	}

	if (cmdList.length() == 3)
	{
		QString param1 = cmdList[1];
#ifdef _SOCKET_DEBUG
		qDebug() << "Parameter Number 2 " << cmdList[0] << ", " << param1.trimmed();
#endif
//		qDebug(param2.trimmed();

        for (ULONG i = 0; i < sizeof(s_param3) / sizeof(DISPATCH_FUNC_PARAM3); i++)
		{
//			qDebug() << cmdList[0] << ", " << param1.trimmed();
//			qDebug() << s_param3[i].m_strParam0 << ", " << s_param3[i].m_strParam1;

			if ((cmdList[0] == s_param3[i].m_strParam0) && (param1.trimmed() == s_param3[i].m_strParam1))
			{
				int cmdType = s_param3[i].m_commandType;

				if ((cmdType > 0) && (cmdType < CMD_TOTAL_COUNT))
				{
					m_commandFlag[cmdType] = false;
					m_strCommandList[cmdType] = cmdList;
				}
				else if ((cmdType == CMD_NULL) && (s_param3[i].m_pFunc != nullptr))
				{
					s_param3[i].m_pFunc(this, cmdList);
				}

				return;
			}
		}
	}
}

bool DeviceSocket::getTheaterName(QString & theaterName, int timeoutInMs, int repeatCount)
{
	bool ret = DeviceSocket::getValueByNode(NODE_THEATER_NAME, theaterName, timeoutInMs, repeatCount);
	return (ret);
}


bool DeviceSocket::getHallName(QString & hallName, int timeoutInMs, int repeatCount)
{
	bool ret = DeviceSocket::getValueByNode(NODE_HALL_NAME, hallName, timeoutInMs, repeatCount);
	return (ret);
}


bool DeviceSocket::getDeviceFWVersion(QString & version, int timeoutInMs, int repeatCount)
{
	bool ret = DeviceSocket::getValueByNode(NODE_DEVICE_FIRMWARE_VERSION, version, timeoutInMs, repeatCount);
	return (ret);
}

bool DeviceSocket::getValueByNode(QString node, QString &value, int timeoutInMs, int repeatCount)
{
	bool ret = false;
    QString command = QString("get \"%1\"\n").arg(node);

	if (m_bConnectedFlag == false)
	{
		return (false);
	}

	s_param3[0].m_strParam0 = "get";
	s_param3[0].m_strParam1 = node;
	m_commandFlag[CMD_RESERVED] = true;

	for (int index = 0; index < repeatCount; index++)
	{
		if (BIT(g_debug, LOG_SOCKET_MESSAGE))
		{
			writeToSocket(command.toStdString().c_str());
			L_INFO(QString("Send to device: %1 :%2").arg(m_deviceAddress.toString()).arg(command));
		}

		for (int i = 0; i < timeoutInMs / 10; i++)
		{
			g_pApp->msleep(10);
			if (m_bConnectedFlag == false)
			{
				return (false);
			}
			if (m_commandFlag[CMD_RESERVED] == false)
			{
				QStringList cmdList = m_strCommandList[CMD_RESERVED];
				if (cmdList.length() == 3)
				{
					value = cmdList[2];
					ret = true;
				}
				goto END;
			}
		}
	}

END:
	s_param3[0].m_strParam0 = "";
	s_param3[0].m_strParam1 = "";
	return (ret);
}



void DeviceSocket::sendBye()
{
	if (m_bConnectedFlag == false)
	{
		return;
	}

    QString command = QString("exit\r\n");
	writeToSocket(command.toStdString().c_str());
	qDebug() << command.toStdString().c_str();
}

void DeviceSocket::sendHeartBeat()
{
    QString sv, value;
	static int s_value = 0;

	if (m_bConnectedFlag == false)
	{
		return;
	}

	s_value++;
	if (s_value > 100)
	{
		s_value = 0;
	}

    sv = QString(NODE_HEART_BEAT);
    value = QString::number(s_value);
    writeToBuffer(sv, value);
}

bool DeviceSocket::getShakeHandFlag()
{
	bool ret = m_shakeHandFlag;

	m_shakeHandFlag = false;
	return (ret);
}

void DeviceSocket::endHeartBeat()
{
	static int s_value = 0;

	if (m_bConnectedFlag == false)
	{
		return;
	}

	s_value++;
	if (s_value > 100)
	{
		s_value = 0;
	}
    QString command = QString("set \"%1\" \"0\"\n").arg(NODE_PC_CONNECTION);
	writeToSocket(command.toStdString().c_str());
}

void DeviceSocket::resetDevice()
{
	QString command = QString("set \\\\Node\\SV\\Reboot Yes\n");
	writeToSocket(command.toStdString().c_str());
	qDebug() << command.toStdString().c_str();
}

void DeviceSocket::writeToBuffer(QString & node, QString &value)
{
	int i;
	QString command;

	for (i = 0; i < m_nodeList.length(); i++)
	{
		if (m_nodeList.at(i) == node)
		{
			m_valueList[i] = value;
			break;
		}
	}

	if (i == m_nodeList.length())
	{
		m_nodeList << node;
		m_valueList << value;
	}
}

void DeviceSocket::onTimer_sendBuffer()
{
	QString command;

	/* SOCKET is connected and Login is OK */
	if ((m_commandFlag[CMD_WAIT_LOGIN] == false) && (m_bConnectedFlag == true))
	{
		for (int i = 0; i < m_nodeList.length(); i++)
		{
			command = QString("set \"%1\" \"%2\"\n").arg(m_nodeList.at(i)).arg(m_valueList.at(i));
			writeToSocket(command.toStdString().c_str());
			if (BIT(g_debug, LOG_SOCKET_MESSAGE))
			{
				qDebug() << command.toStdString().c_str();
			}
		}
		m_nodeList.clear();
		m_valueList.clear();
	}
}

void DeviceSocket::writeToSocket(const char *lpszBuffer)
{
	if (m_bConnectedFlag == false)
	{
		L_ERROR(QString("Failed to send to device %1 : %2").arg(getDeviceAddr().toString()).arg(lpszBuffer));
		return;
	}

	if (BIT(g_debug, LOG_SOCKET_MESSAGE))
	{
		L_DEBUG(QString("Send to device %1 : %2").arg(getDeviceAddr().toString()).arg(lpszBuffer));
	}
	m_pSocket->write(lpszBuffer);
}
