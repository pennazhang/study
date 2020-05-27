#pragma once

#include <QUdpSocket>
#include <QHostAddress>
#include <QWidget>
#include <QNetworkInterface>
#include "types.h"


#define MAX_FRAME_LEN  1480

#define BROADCAST_LOCAL_PORT	3804
#define BROADCAST_PEER_PORT		3804


void FormatString(char lpszBuffer[501], UINT8 * pucMsg, int len);

#pragma pack(4)

typedef struct
{
	QHostAddress	m_localAddr;
	int				m_localPort;
	QHostAddress	m_boardcastAddr;
	int				m_boardcastPort;
	QHostAddress	m_networkMask;
	U8				m_hardwareAddr[6];
	QUdpSocket	*	m_pUdpSocket;
} UPD_SOCKET;

typedef enum
{
	CMD_ID_CHECK_SOCKET_INFO	= 0,		// SOCKET_INFO
	CMD_ID_CHECK_SOCKET_INFO_ACK = 1,	// SOCEKET_INFO
	CMD_ID_GET_BOOK_LIST		= 2,
	CMD_ID_GET_BOOK_LIST_ACK	= 3,
	CMD_ID_GET_BOOK_INFO		= 4,
	CMD_ID_GET_BOOK_INFO_ACK	= 5,
	CMD_ID_COPY_TO_DEVICE		= 6,
	CMD_ID_COPY_TO_DEVICE_ACK	= 7,
	CMD_ID_DELETE_FROM_DEVICE		= 8,
	CMD_ID_DELETE_FROM_DEVICE_ACK	= 9,
	CMD_ID_COPY_FILE_TO_DEVICE		= 10,
	CMD_ID_COPY_FILE_TO_DEVICE_ACK	= 11,
	CMD_ID_COPY_FROM_DEVICE			= 12,
	CMD_ID_COPY_FROM_DEVICE_ACK		= 13,
/*
	CMD_ID_GET_FILE_SIZE		= 2,	// char *FileName
	CMD_ID_GET_FILE_SIZE_ACK	= 3,	// int fileLen
	CMD_ID_GET_FILE_DATA		= 4,	// int startAddr, int len
	CMD_ID_GET_FILE_DATA_ACK	= 5,	// char * fileContent
	CMD_ID_DELETE_FILE			= 6,	// char *FileName
	CMD_ID_DELETE_FILE_ACK		= 7,	// int	0: success, 1: fail
	CMD_ID_ADD_FILE				= 8,	// char * fileName;
	CMD_ID_ADD_FILE_ACK			= 9,	// int  0: success, 1: fail
*/
} CMD_ID;

typedef struct 
{
	 UINT32 m_sendAddr;
	 UINT32 m_sendPort;
	 UINT32 m_recvAddr;
	 UINT32 m_recvPort;
} SOCKET_INFO;

typedef struct 
{
	CMD_ID			m_cmdID;	//	m_cmdID = SOCKET_INFO
	SOCKET_INFO		m_socketInfo;
} SOCKET_INFO_MSG;

typedef struct
{
	UINT32			m_MCUVersion;
	UINT32			m_DSPVersion;
	UINT32			m_connectFlag;		/* 0: Disconnected,   1: Connected */
	UINT32			m_reservedField;
	char			m_hostName[28];		/* 28 character at most */
} BASIC_INFO;

typedef struct
{
	CMD_ID			m_cmdID;	//	m_cmdID = SOCKET_INFO_ACK
	SOCKET_INFO		m_socketInfo;
	BASIC_INFO		m_basicInfo;
} SOCKET_INFO_ACK;

#pragma pack()

class BroadcastCommunication : public QObject
{
	Q_OBJECT

public:
	BroadcastCommunication();
	void createSocket(QWidget *pWidget, int localPort, int remoteBoardcastPort);
	void releaseAll();
	~BroadcastCommunication(void);
	void scanDevice(void);

signals:
	void onFoundDevice(UINT32 ulLocalIP, UINT32 ulPeerIP);

protected:
	void onRecvData(int socketNum);

protected slots:
	void onRecvData_0(void);
	void onRecvData_1(void);
	void onRecvData_2(void);
	void onRecvData_3(void);
	void onRecvData_4(void);
	void onRecvData_5(void);
	void onRecvData_6(void);
	void onRecvData_7(void);
	void onRecvData_8(void);
	void onRecvData_9(void);

protected:
	UPD_SOCKET m_pSocket[10];
	int m_nSocketCount;		/* At most 10 networks */
};

extern void dumpSocketInfo(SOCKET_INFO *pInfo);

extern BroadcastCommunication *g_pBroadcastCommunication;
