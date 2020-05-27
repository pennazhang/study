#ifndef DEVICE_SOCKET_H
#define DEVICE_SOCKET_H

#include <QObject>
#include <QTcpSocket>
#include <QHostAddress>
#include "types.h"
#include <QTextStream>
#include <QTimer>

#define CMD_NULL							-1
#define CMD_WAIT_LOGIN						0
#define CMD_RESERVED						15
#define CMD_TOTAL_COUNT						16

/* Platform */
#define	NODE_PC_CONNECTION		"\\\\Node\\SV\\PCConnectStatus"
#define NODE_HEART_BEAT			"\\\\Node\\SV\\HeartBeat\\r"
#define NODE_GUI_CONNECTION_STATUS "\\\\Node\\SV\\PCConnectStatus"
/* Version */
#define NODE_DEVICE_FIRMWARE_VERSION	"\\\\Node\\SV\\Version\\r"


/* TAB 1: Overview */
#define NODE_HALL_NAME			"\\\\Node\\SV\\Hall\\r"
#define NODE_THEATER_NAME		"\\\\Node\\SV\\Theater\\r"

class DeviceSocket : public QObject
{
	Q_OBJECT

public:
	explicit DeviceSocket();
	~DeviceSocket();
	bool setConnection(QHostAddress localAddress, QHostAddress deviceAddress);

	void disconnect();
	bool getShakeHandFlag();

	QHostAddress getLocalAddr();
	QHostAddress getDeviceAddr();

	/* Get Device FW Version */
	bool getDeviceFWVersion(QString & version, int timeoutInMs = 3000, int repeatCount = 1);

	/* Get Hall Name */
	bool getHallName(QString & hallName, int timeoutInMs = 3000, int repeatCount = 1);

	/* Get Theater Name */
	bool getTheaterName(QString & theaterName, int timeoutInMs = 3000, int repeatCount = 1);

	bool getValueByNode(QString node, QString &value, int timeoutInMs = 3000, int repeatCount = 1);

	void sendHeartBeat();	/* We should send heart beat every 1 second */
	void endHeartBeat(void);		/* Notify the CPi2000 that heart beat is over */

	void sendBye(void);

	void resetDevice();

public slots:
	void readyRead();
	void onTimer_sendBuffer();

protected:
	void onDispatchData(QString strLine);
	void writeToBuffer(QString & node, QString &value);		/* For some message, We should not send the same message within 500ms */
	void writeToSocket(const char *lpszBuffer);

protected:
	QTcpSocket *m_pSocket;
	QHostAddress m_localAddress;
	QHostAddress m_deviceAddress;
	bool m_bConnectedFlag;
	QTextStream *m_pTextStream;

	/* Wait Model Version */
	bool m_commandFlag[CMD_TOTAL_COUNT];
	QStringList m_strCommandList[CMD_TOTAL_COUNT];

	QTimer *m_pTimer;
	bool m_shakeHandFlag;
	QStringList m_nodeList, m_valueList; /* it is used to store the SV and value which is sent by the function: writeToBuffer */
};
#endif // #define DEVICE_SOCKET_H
