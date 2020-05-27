#include "BroadcastCommunication.h"
#include <QMetaObject>
#include <QFileInfo>
#include <QMessageBox>
#include <QString>
#include <QWidget>
#include <QBuffer>
#include "commonLib.h"
#include "mainApplication.h"
#include "mainWidget.h"
#include "simpleQtLogger.h"

BroadcastCommunication *g_pBroadcastCommunication = nullptr;

BroadcastCommunication::BroadcastCommunication()
{
	m_nSocketCount = 0;
}

void BroadcastCommunication::createSocket(QWidget *pWidget, int localPort, int remoteBoardcastPort)
{
	releaseAll();

	QList<QNetworkInterface> list = QNetworkInterface::allInterfaces();
	
	//获取所有网络接口的列表
	foreach(QNetworkInterface interface,list)
	{  
        if ( interface.isValid() &&
             !interface.flags().testFlag(QNetworkInterface::IsLoopBack)  &&
             !interface.flags().testFlag(QNetworkInterface::IsPointToPoint) &&
             interface.flags().testFlag(QNetworkInterface::IsUp) &&
             interface.flags().testFlag(QNetworkInterface::IsRunning) &&
             interface.flags().testFlag(QNetworkInterface::CanBroadcast) )
        {
			QString macAddr = interface.hardwareAddress();
			qDebug() << "Hardware address:" << macAddr;
			UINT8 mac[6];
			int index;
			for (index = 0; index < 6; index++)
			{
				bool flag;
				QString value = macAddr.mid(3 * index, 2);
				mac[index] = value.toLong(&flag, 16);
				if (flag != true)
				{
					break;
				}
			}
			if (index != 6)
			{
				break;
			}
			char logInfo[501];
			FormatString(logInfo, mac, 6);
			qDebug() << "MAC Addr: " << logInfo;

			QList<QNetworkAddressEntry> entryList = interface.addressEntries();

			//获取IP地址条目列表，每个条目中包含一个IP地址，一个子网掩码和一个广播地址
			foreach(QNetworkAddressEntry entry,entryList)

			{//遍历每一个IP地址条目
				//IP地址
				QHostAddress localAddr = entry.ip();
				if ((localAddr.protocol() ==  QAbstractSocket::IPv4Protocol) && (localAddr.isLoopback() == false))
				{
					QUdpSocket *pUdpSocket = new QUdpSocket(pWidget);
					pUdpSocket->bind(localAddr, localPort);
					m_pSocket[m_nSocketCount].m_localAddr = localAddr;
					m_pSocket[m_nSocketCount].m_networkMask = entry.netmask();
					memcpy(m_pSocket[m_nSocketCount].m_hardwareAddr, mac, 6);
					m_pSocket[m_nSocketCount].m_localPort = localPort;
					m_pSocket[m_nSocketCount].m_boardcastAddr = entry.broadcast();
					m_pSocket[m_nSocketCount].m_boardcastPort = remoteBoardcastPort;
					m_pSocket[m_nSocketCount].m_pUdpSocket = pUdpSocket;
					qDebug()<<"IPAddr" << localAddr.toString() << ", Broadcast: " << entry.broadcast().toString();
					switch (m_nSocketCount)
					{
					case 0:
						QObject::connect(pUdpSocket, SIGNAL(readyRead()), this, SLOT(onRecvData_0()));
						break;
					case 1:
						QObject::connect(pUdpSocket, SIGNAL(readyRead()), this, SLOT(onRecvData_1()));
						break;
					case 2:
						QObject::connect(pUdpSocket, SIGNAL(readyRead()), this, SLOT(onRecvData_2()));
						break;
					case 3:
						QObject::connect(pUdpSocket, SIGNAL(readyRead()), this, SLOT(onRecvData_3()));
						break;
					case 4:
						QObject::connect(pUdpSocket, SIGNAL(readyRead()), this, SLOT(onRecvData_4()));
						break;
					case 5:
						QObject::connect(pUdpSocket, SIGNAL(readyRead()), this, SLOT(onRecvData_5()));
						break;
					case 6:
						QObject::connect(pUdpSocket, SIGNAL(readyRead()), this, SLOT(onRecvData_6()));
						break;
					case 7:
						QObject::connect(pUdpSocket, SIGNAL(readyRead()), this, SLOT(onRecvData_7()));
						break;
					case 8:
						QObject::connect(pUdpSocket, SIGNAL(readyRead()), this, SLOT(onRecvData_8()));
						break;
					case 9:
						QObject::connect(pUdpSocket, SIGNAL(readyRead()), this, SLOT(onRecvData_9()));
						break;
					}
					m_nSocketCount++;
				}
			}
		}
	}
}

void BroadcastCommunication::releaseAll()
{
	for (int i = 0; i < m_nSocketCount; i++)
	{
		m_pSocket[i].m_pUdpSocket->disconnect();
		m_pSocket[i].m_pUdpSocket->close();
	}
	m_nSocketCount = 0;
}

BroadcastCommunication::~BroadcastCommunication(void)
{
	releaseAll();
}


void BroadcastCommunication::scanDevice(void)
{
	createSocket(g_pMainWidget, BROADCAST_LOCAL_PORT, BROADCAST_PEER_PORT);

	UINT8 msg[] = {0x02, 0x19, 0x00, 0x00, 0x00, 0x48,
		0xef, 0x8c, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
		0x05, 0x00, 0x00, 0xef, 0x8c, 0x01, 0x00, 0x10, 0x6b, 0x19, 0xac, 0x2b, 0xca, 0x81, 0x7b, 0x46,
		0xb0, 0x34, 0x73, 0x04, 0xa3, 0x62, 0xef, 0x79, 0x00, 0x10, 0x00, 0x00, 0x27, 0x10, 0x01, 0x3c,
		0x97, 0x0e, 0xa9, 0xe1, 0x97, 0x01, 0xc0, 0xa8, 0x01, 0x65, 0xff, 0xff, 0xff, 0x00, 0xc0, 0xa8,
		0x01, 0x01 };

	for (int i = 0; i < m_nSocketCount; i++)
	{
		memcpy(msg + 53, m_pSocket[i].m_hardwareAddr, 6);	/* Fill the MAC Address */
		*(UINT32 *)(msg + 60) = qt_htonl(m_pSocket[i].m_localAddr.toIPv4Address());	/* Fill the IP Address */
		*(UINT32 *)(msg + 64) = qt_htonl(m_pSocket[i].m_networkMask.toIPv4Address());	/* Fill the Network mask Address */
		*(UINT32 *)(msg + 68) = qt_htonl(m_pSocket[i].m_localAddr.toIPv4Address());	/* default gateway IP Address */
		qDebug() << "Index " << i << " Send %d :" << sizeof(msg);
		m_pSocket[i].m_pUdpSocket->writeDatagram((char *)&msg, sizeof(msg), m_pSocket[i].m_boardcastAddr, m_pSocket[i].m_boardcastPort);
	}
	L_INFO("Scan device...");
	setBit(g_debug, LOG_SOCKET_MESSAGE);
}

void dumpSocketInfo(SOCKET_INFO *pInfo)
{
	QHostAddress sendAddress, recvAddress;
	sendAddress.setAddress(pInfo->m_sendAddr);
	recvAddress.setAddress(pInfo->m_recvAddr);
	qDebug() << "Sender " << sendAddress.toString() << ":" <<pInfo->m_sendPort << ", Recver " << recvAddress.toString() <<":"<<pInfo->m_recvPort;
}


void BroadcastCommunication::onRecvData_0(void)
{
	onRecvData(0);
}

void BroadcastCommunication::onRecvData_1(void)
{
	onRecvData(1);
}

void BroadcastCommunication::onRecvData_2(void)
{
	onRecvData(2);
}

void BroadcastCommunication::onRecvData_3(void)
{
	onRecvData(3);
}

void BroadcastCommunication::onRecvData_4(void)
{
	onRecvData(4);
}

void BroadcastCommunication::onRecvData_5(void)
{
	onRecvData(5);
}

void BroadcastCommunication::onRecvData_6(void)
{
	onRecvData(6);
}

void BroadcastCommunication::onRecvData_7(void)
{
	onRecvData(7);
}

void BroadcastCommunication::onRecvData_8(void)
{
	onRecvData(8);
}

void BroadcastCommunication::onRecvData_9(void)
{
	onRecvData(9);
}


void BroadcastCommunication::onRecvData(int socketIndex)
{
	QUdpSocket *pUdpSocket = m_pSocket[socketIndex].m_pUdpSocket;
//	int count, i;
	QString strBookName, strDBName;

//	SOCKET_INFO_MSG *pSocketInfoMsg;
	QString strHostName;

    while (m_pSocket[socketIndex].m_pUdpSocket->hasPendingDatagrams()) 
	{
        char aucData[2000];
		int nRecvLen = pUdpSocket->pendingDatagramSize();
		if (nRecvLen == -1)
		{
			break;
		}
        pUdpSocket->readDatagram(aucData, nRecvLen);

		char log[501];
		FormatString(log, (BYTE *)aucData, nRecvLen);
		qDebug() << "Index" << socketIndex << " Recv " <<nRecvLen << " : " << log;
		if (nRecvLen == 72) 
		{
			UINT32 ulPeerIP= *(UINT32 *)(aucData + 60);
			UINT32 ulLocalIP = m_pSocket[socketIndex].m_localAddr.toIPv4Address();
            if (ulLocalIP != qt_htonl(ulPeerIP))
			{
                emit onFoundDevice(ulLocalIP, qt_htonl(ulPeerIP));
			}
		}
/*
		CMD_ID cmdID = *(CMD_ID *)aucData;
		switch (cmdID)
		{
		case CMD_ID_CHECK_SOCKET_INFO_ACK:
			pSocketInfoMsg = (SOCKET_INFO_MSG *)aucData;
			aucData[nRecvLen] = 0;
			emit onFoundDevice((BYTE *)aucData, nRecvLen);
			break;
		}
*/
	}
}
