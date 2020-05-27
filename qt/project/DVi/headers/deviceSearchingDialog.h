#ifndef DEVICE_SEARCHING_DIALOG_H
#define DEVICE_SEARCHING_DIALOG_H

#include <QProgressDialog>
#include <qprogressbar.h>
#include <QTimer>
#include <QTableWidget>
#include <QPushButton>
#include "types.h"
#include <QLabel>
#include "BroadcastCommunication.h"
#include "backgroundWidget.h"
#include "DeviceSocket.h"

typedef struct
{
	UINT32			m_localIPAddress;	/* local IP Address */
	UINT32			m_deviceAddr;		/* IP Address */
	QString			m_firmwareVersion;	/* model Version  */	
	QString			m_deviceName;		/* Theater:Hall */
	bool			m_connectFlag;	/* True: the device is already connectted by another GUI */
} DEVICE_INFO;

class DeviceSearchingDialog : public QDialog
{
    Q_OBJECT
public:
    DeviceSearchingDialog(QWidget *parent);
    ~DeviceSearchingDialog();
	DEVICE_INFO *getSelectedDevice();

protected slots:
	void onTimerOut();
	void onScanClicked();
	void onCancelClicked();
	void onDeviceSelected();
	void onConnectClicked();
	void onFoundDevice(UINT32 ulLocalIP, UINT32 ulPeerIP);
	void onSearchDevice();

protected:
	void resizeEvent(QResizeEvent * event);
	void refreshDeviceList();
	void refreshButton();
	void showDevice(int index, DEVICE_INFO *pDeviceInfo);
	void addDevice(DEVICE_INFO deviceInfo);
	void clearAllDevice();
	void dumpAvailableDevice();
	void onScanComplete();
	bool verifyDevice(UINT32 ulPeerAddress, DEVICE_INFO * pDeviceInfo);

protected:
	QTimer *m_pTimer;
	QTableWidget *m_pDeviceList;
	QPushButton *m_pScanButton, *m_pConnectButton, *m_pCancelButton, *m_pAddDevice; 
	QLabel *m_pLabel;
	BackgroundWidget *m_pWaitingWidget;
	DEVICE_INFO m_deviceListInfo[20];	/* At most 20 devices for one single broadcast scan */
	int m_deviceInfoCount, m_selectedDeviceIndex;
	bool m_scaningFlag;
	int m_scanTime;
	QList<int> m_deviceIPList;
};

#endif
