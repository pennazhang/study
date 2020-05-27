#include "MainApplication.h"
#include <QTime>
#include <QNetworkConfigurationManager>
#include <qnetworkaccessmanager.h>
#include <QUrl>
#include <QNetworkRequest>
#include <QDirIterator>
#include <QMessageBox.h>
#include "mainWidget.h"
#include "simpleQtLogger.h"
#include "SQLiteDB.h"

MainApplication::MainApplication(int argc, char *argv[])
	: QApplication(argc, argv)
{
	m_deviceConnecteStatus = DEVICE_DISCONNECTED;
	m_pid = rand() % 100;

	g_pDatabase = new SQLiteDB();
}

MainApplication::~MainApplication()
{
	disconnectDevice();

	delete g_pDatabase;
}


void MainApplication::msleep(int ms)
{
    QTime dieTime = QTime::currentTime().addMSecs(ms);

	for ( ; QTime::currentTime() <= dieTime; )
	{
		QCoreApplication::processEvents( QEventLoop::AllEvents);
	}
}

void MainApplication::setConnectStatus(DEVICE_CONNECT_STATUS status) 
{ 
	DEVICE_CONNECT_STATUS oldStatus = m_deviceConnecteStatus;
	m_deviceConnecteStatus = status;

	if (status != oldStatus)
	{
		if (status == DEVICE_DISCONNECTED)
		{
			emit deviceDisconnected();
		}
		else
		{
			emit deviceConnected();
		}
	}
}

DeviceSocket * MainApplication::getDeviceConnection()
{
	return (&m_deviceSocket);
}

bool MainApplication::connectDevice(QHostAddress localAddress, QHostAddress peerAddress)
{
	disconnectDevice();

	L_INFO("Connecting to Device: " + peerAddress.toString());
	
	bool ret = m_deviceSocket.setConnection(localAddress, peerAddress);
	if (ret == true)
	{
		setConnectStatus(DEVICE_CONNECTED);
		L_INFO("Connected with device: " + peerAddress.toString());
	}
	else
	{
		L_ERROR("Failed to connected with device: " + peerAddress.toString());
	}
	return (ret);
}

void MainApplication::disconnectDevice() 
{
	if (m_deviceConnecteStatus == DEVICE_CONNECTED)
	{
		m_deviceSocket.endHeartBeat();
		m_deviceSocket.sendBye();
		msleep(100);
		m_deviceSocket.disconnect();
	}

	setConnectStatus(DEVICE_DISCONNECTED);
}

bool MainApplication::putFtpFile(QString& localFileName, QString& remoteFileName, QString& userName, QString& password, int timeoutInSecond)
{
	if (getConnectStatus() != DEVICE_CONNECTED)
	{
		L_ERROR("putFtpFile failed for not connected with device!");
		return (false);
	}

	QFile localFile(localFileName);
    if (localFile.open(QIODevice::ReadOnly) == false)
	{
		L_ERROR("putFtpFile failed for open local file failed :" + localFileName);
		return (false);
	}

    QByteArray data = localFile.readAll();
    localFile.close(); 

    QNetworkAccessManager networkManager(this);
    QUrl url(remoteFileName);
    url.setUserName(userName);
    url.setPassword(password);
    QNetworkRequest req(url);

    connect(&networkManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(uploadFinished(QNetworkReply*)));
    networkManager.put(QNetworkRequest(url), data);

	m_ftpFinished = false; 
	m_ftpSuccess = false;
	m_ftpData.clear();
	m_ftpErrorInfo = QString("Timeout");
	for (int i = 0; i < timeoutInSecond * 100; i++)
	{
		if ((m_ftpFinished == true) || (getConnectStatus() != DEVICE_CONNECTED))
		{
			break;
		}
		msleep(10);
	}
	if (m_ftpSuccess == false)
	{
		L_WARN(QString("Failed to put file:%1").arg(remoteFileName)); 
		qDebug() << "Upload file: " << remoteFileName << " Failed: " << m_ftpErrorInfo;
	}
	else
	{
		qDebug() << "Upload file: " << remoteFileName << " Successfully";
	}

	return (m_ftpSuccess);
}

void MainApplication::downloadFinished(QNetworkReply* reply)
{
	m_ftpFinished = true;

	if (reply->error() == QNetworkReply::NoError)
	{
		m_ftpSuccess = true;
		m_ftpData = reply->readAll();
	}
	else
	{
		m_ftpSuccess = false;
	}

	m_ftpErrorInfo = reply->errorString();
}


void MainApplication::uploadFinished(QNetworkReply* reply)
{
	m_ftpFinished = true;

	if (reply->error() == QNetworkReply::NoError)
	{
		m_ftpSuccess = true;
	}
	else
	{
		m_ftpSuccess = false;
	}
	m_ftpErrorInfo = reply->errorString();
}


QString MainApplication::getTempPresetDBFileName()
{
	QString tempDir = QDir::tempPath();
	QString presetDBFileName = QString("%1/DVi_Presets_%2.db").arg(tempDir).arg(m_pid);
	qDebug() << "Temp Preset File: " << presetDBFileName;
	return (presetDBFileName);
}

