#ifndef MAIN_APPLICATION_H
#define MAIN_APPLICATION_H

#include "types.h"
#include <QApplication>
#include <QFile>
#include <QDebug>
#include <QSettings>

#include <QNetworkReply>
#include <private/qzipreader_p.h>
#include <private/qzipwriter_p.h>
#include "DeviceSocket.h"

#define DVi_GUI_VERSION "1.0.4.0"
#define DVi_MAIN_VERSION_MASK "1.0.4"
#define DVi_NEW_FW_VERSION  "1.0.4.0"

//#define CROSSROAD_CONSOLE_DEBUG

//#define _SOCKET_DEBUG
//#define LOG_FILE


#ifdef LOG_FILE
extern QTextStream g_log;
extern QFile g_logFile;
#define LOG 		g_log <<endl << QDate::currentDate().toString(Qt::ISODate) + " " + QTime::currentTime().toString() + " "
#define WARNING     g_log <<endl << QDate::currentDate().toString(Qt::ISODate) + " " + QTime::currentTime().toString() + " Warning: "
#define FATAL_ERROR     g_log << endl << QDate::currentDate().toString(Qt::ISODate) + " " + QTime::currentTime().toString() + " Error: "
#else
#define LOG 		qDebug() << QDate::currentDate().toString(Qt::ISODate) + " " + QTime::currentTime().toString() + " "
#define WARNING     qDebug() << QDate::currentDate().toString(Qt::ISODate) + " " + QTime::currentTime().toString() + " Warning: "
#define FATAL_ERROR     qDebug() << QDate::currentDate().toString(Qt::ISODate) + " " + QTime::currentTime().toString() + " Error: "
#endif

#define REMOTE_DEFAULT_DEVICE_FILE "SAVE_DEFAULT.dev"

typedef enum
{
	DEVICE_DISCONNECTED = 0,
	DEVICE_CONNECTED,
} DEVICE_CONNECT_STATUS;

class MainApplication : public QApplication
{
    Q_OBJECT

public:	
	MainApplication(int argc, char *argv[]);
	~MainApplication();
	void msleep(int ms);

	DEVICE_CONNECT_STATUS getConnectStatus()  { return m_deviceConnecteStatus; }
	void setConnectStatus(DEVICE_CONNECT_STATUS status);
	DeviceSocket *getDeviceConnection();
	bool connectDevice(QHostAddress localAddress, QHostAddress peerAddress);
	void disconnectDevice();
	bool getFtpFile(QString strRemoteFileName, QString strLocalFileName);
	bool unzipFileToFolder(QString zipFileName, QString folder);
	bool zipFolderToFile(QString zipFileName, QString folder);
	bool zipFilesToFile(QString zipFileName, QStringList fileNameList);
//	void onRecvMasterVolume(QString outputVolume);
	bool putFtpFile(QString& strLocalFileName, QString& strRemoteFileName, QString& userName, QString& password, int timeoutInSecond = 80);
	QString getTempPresetDBFileName();
	void setPid(int processID) {m_pid = processID; }

signals:
	void deviceConnected();
	void deviceDisconnected();

private slots:
	void downloadFinished(QNetworkReply*);
	void uploadFinished(QNetworkReply*);

protected:
	DEVICE_CONNECT_STATUS m_deviceConnecteStatus;
	DeviceSocket m_deviceSocket;

	bool m_ftpSuccess;
	bool m_ftpFinished;
	QString m_ftpErrorInfo;
	QByteArray m_ftpData;
	int m_pid;
};


extern MainApplication *g_pApp;
extern QTextStream g_log;
extern QFile g_logFile;

extern QSettings *g_pRegisterSetting;	/* Access Windows Register, the Key is:  "Computer\HKEY_CURRENT_USER\Software\Harman\DVi" */

#endif // MAINWIDGET_H
