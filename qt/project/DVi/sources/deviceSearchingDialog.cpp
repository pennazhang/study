#include "deviceSearchingDialog.h"
#include <QHostAddress>
#include <QHeaderView>
#include "types.h"
#include <QEvent>
#include <QInputDialog>
#include "mainApplication.h"
#include <QMessageBox>
#include "simpleQtLogger.h"

DeviceSearchingDialog::DeviceSearchingDialog(QWidget *parent)
	: QDialog(parent)
{
	m_pTimer = new QTimer(this);
	m_pTimer->start(500);
	connect(m_pTimer, SIGNAL(timeout()), this, SLOT(onTimerOut()));
	m_scanTime = 0;

	m_pLabel = new QLabel(tr("Available Device List"), this);
	m_pLabel->setObjectName(QStringLiteral("deviceListLabel"));

    QStringList labels;
    labels << tr("Device Name") << tr("Device Address") << tr("Firmware");

	m_pDeviceList = new QTableWidget(this);
    m_pDeviceList->setColumnCount(3);
    m_pDeviceList->setHorizontalHeaderLabels(labels);
	m_pDeviceList->setEditTriggers(QAbstractItemView::NoEditTriggers);
	m_pDeviceList->setSelectionBehavior(QAbstractItemView::SelectRows);	
	m_pDeviceList->setSelectionMode(QAbstractItemView::SingleSelection);
	m_pDeviceList->verticalHeader()->setVisible(false);//隐藏左边垂直
//	connect(m_pDeviceList, SIGNAL(itemClicked(QTableWidgetItem *)), this, SIGNAL(onDeviceSelected(QTableWidgetItem *)));
	connect(m_pDeviceList, SIGNAL(itemSelectionChanged()), this, SLOT(onDeviceSelected()));

	QHeaderView *pHeaderView = m_pDeviceList->horizontalHeader();
	pHeaderView->resizeSections(QHeaderView::Fixed);
	pHeaderView->setFixedHeight(40);
//	headerView->setMov(false);//去除表头的移动
//	m_pDeviceList->horizontalHeader()->setVisible(false);

	m_pAddDevice = new QPushButton("+", this);
	connect(m_pAddDevice, SIGNAL(clicked()), this, SLOT(onSearchDevice()));		
	

	m_pScanButton = new QPushButton(tr("Scan"), this);
	connect(m_pScanButton, SIGNAL(clicked()), this, SLOT(onScanClicked()));		

	m_pConnectButton = new QPushButton(tr("Connect"), this);
	connect(m_pConnectButton, SIGNAL(clicked()), this, SLOT(onConnectClicked()));

	m_pCancelButton = new QPushButton(tr("Cancel"), this);
	connect(m_pCancelButton, SIGNAL(clicked()), this, SLOT(onCancelClicked()));	

	m_pWaitingWidget = new BackgroundWidget(QString(":ActivityIndicator.png"), this, 20, 2);
	m_pWaitingWidget->hide();

	connect(g_pBroadcastCommunication, SIGNAL(onFoundDevice(UINT32, UINT32)), this, SLOT(onFoundDevice(UINT32, UINT32)));

	m_deviceInfoCount = 0;
	m_scaningFlag = false;

	setFixedSize(620, 420);
	emit m_pScanButton->clicked();
}

DeviceSearchingDialog::~DeviceSearchingDialog()
{
	g_pBroadcastCommunication->releaseAll();
}

void DeviceSearchingDialog::onFoundDevice(UINT32 ulLocalIP, UINT32 ulPeerIP)
{
	QString theaterName, hallName, version;

	QString info = QString("Found:") + QHostAddress(ulLocalIP).toString() + QString(" <-> ") +  QHostAddress(ulPeerIP).toString();
	L_INFO(info);

	if (!m_deviceIPList.contains(ulPeerIP))
	{
		m_deviceIPList.append(ulPeerIP);
	}
}

void DeviceSearchingDialog::onCancelClicked()
{
	reject();
}

void DeviceSearchingDialog::onScanComplete()
{
	DEVICE_INFO deviceInfo;
	for (int i = 0; i < m_deviceIPList.length(); i++)
	{
		if (verifyDevice(m_deviceIPList.at(i), &deviceInfo))
		{
			addDevice(deviceInfo);		
		}
	}

	m_scaningFlag = false;
	m_pScanButton->setEnabled(true);
	m_pAddDevice->setEnabled(true);
	m_pWaitingWidget->hide();
	refreshDeviceList();
}

void DeviceSearchingDialog::refreshButton()
{
	m_pConnectButton->setEnabled(false);
	m_pAddDevice->setEnabled(m_scaningFlag == false);
	if (m_scaningFlag == true)
	{
		m_pScanButton->setEnabled(false);
	}
	else
	{
		m_pScanButton->setEnabled(true);
		QList<QTableWidgetItem*> itemList = m_pDeviceList->selectedItems();
		int count = itemList.count();
		if (count == 3)
		{
			QTableWidgetItem* pItem = itemList[0];
			int index = pItem->row();
			if (m_deviceListInfo[index].m_connectFlag == false)
			{
				m_pConnectButton->setEnabled(true);
			}
		}
	}
}

void DeviceSearchingDialog::onScanClicked()
{
	m_scaningFlag = true;

	/* Scan Network */
	clearAllDevice();
	m_scanTime = 0;
	m_pDeviceList->setRowCount(0);
	m_pWaitingWidget->show();

	m_deviceIPList.clear();
	g_pBroadcastCommunication->scanDevice();

	refreshButton();
}

bool validate_ip_address(QString& s)
{
//	QRegExp rx("0*(2(5[0-5]|[0-4]\d)|1?\d{1,2})(\.0*(2(5[0-5]|[0-4]\d)|1?\d{1,2})){3}");
	QRegExp rx("[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}" );
	bool result = rx.exactMatch(s);
	return result;
}

bool DeviceSearchingDialog::verifyDevice(UINT32 ulPeerAddress, DEVICE_INFO *pDeviceInfo)
{
	QString theaterName("MCL"), hallName("VIP_1"), version("0.2.0.5");

	DeviceSocket deviceSocket;
	if (deviceSocket.setConnection(QHostAddress("0.0.0.0"), QHostAddress(ulPeerAddress)) == false)
	{
		return false;
	}

	if (deviceSocket.getTheaterName(theaterName) == false)
	{
		return false;
	}

	if (deviceSocket.getHallName(hallName) == false)
	{
		return false;
	}

	QString connectStatus;
	if (deviceSocket.getValueByNode(NODE_GUI_CONNECTION_STATUS, connectStatus) == false)
	{
		return false;
	}

	if (connectStatus == "1")
	{
		pDeviceInfo->m_connectFlag = true;	
	}
	else
	{
		pDeviceInfo->m_connectFlag = false;	
	}

	if (deviceSocket.getDeviceFWVersion(version) == false)
	{
		return false;
	}

	pDeviceInfo->m_localIPAddress = 0;
	pDeviceInfo->m_deviceAddr = ulPeerAddress;
	pDeviceInfo->m_firmwareVersion = version;
	pDeviceInfo->m_deviceName = theaterName + ":" + hallName;

	return true;
}

void DeviceSearchingDialog::onSearchDevice()
{
	m_deviceIPList.clear();

	QString deviceAddress = g_pRegisterSetting->value("deviceAddress").toString();

	bool ok;
    QString ipAddr = QInputDialog::getText(this, tr("Add device..."),
                                         tr("Device IP Address:"), QLineEdit::Normal,
                                         deviceAddress, &ok);
    if (!ok || ipAddr.isEmpty())
	{
		return;
	}

	if (validate_ip_address(ipAddr) == false)
	{
		QMessageBox::warning(this, tr("Invalid Input"), tr("What you input is not a valid IP Address"), tr("OK"));
		return;	
	}
	UINT32 ulPeerIP = QHostAddress(ipAddr).toIPv4Address(&ok);
	if (ok == false)
	{
		QMessageBox::warning(this, tr("Invalid Input"), tr("What you input is not a valid IP Address"), tr("OK"));
		return;	
	}

	DEVICE_INFO deviceInfo;
	if (verifyDevice(ulPeerIP, &deviceInfo) == false)
	{
		QMessageBox::warning(this, tr("Can't find the Device"), tr("Failed to connect device with the IP address: %1, please check the IP address again!").arg(ipAddr));
		return;
	}

	addDevice(deviceInfo);
	refreshDeviceList();

	g_pRegisterSetting->setValue("deviceAddress", ipAddr);
}

void DeviceSearchingDialog::onTimerOut()
{
	if (m_scaningFlag == true)
	{
		if ((m_scanTime >= 1) && (m_scanTime <= 2))
		{
			g_pBroadcastCommunication->scanDevice();
		}
		m_scanTime++;
		if (m_scanTime == 4)
		{
			onScanComplete();
		}
	}
}

//void DeviceSearchingDialog::onDeviceSelected(QTableWidgetItem *pItem)
void DeviceSearchingDialog::onDeviceSelected()
{
	refreshButton();
}

void DeviceSearchingDialog::resizeEvent(QResizeEvent * /*event*/)
{
	int leftMargin = 30;
	int topMargin = 70;
	int tableWidth = width() - 2 * leftMargin;
	m_pLabel->setGeometry(leftMargin, 40, 120, 20);
	m_pAddDevice->setGeometry(leftMargin + 120, 40, 22, 22);

	m_pDeviceList->setGeometry(leftMargin, topMargin, tableWidth, 260);
	for (int col = 0; col < 3; col++)
	{
		m_pDeviceList->setColumnWidth(col, tableWidth / 3);
	}

	int buttonWidth = 100;
	int buttonHeight = 35;
	int buttonLeftMargin = 70;
	int buttonTop = 360;
	m_pScanButton->setGeometry(buttonLeftMargin, buttonTop, buttonWidth, buttonHeight);
	m_pConnectButton->setGeometry((width() - buttonWidth) / 2, buttonTop, buttonWidth, buttonHeight);
	m_pCancelButton->setGeometry(width() - buttonLeftMargin - buttonWidth, buttonTop, buttonWidth, buttonHeight);

	int w = m_pWaitingWidget->pixelWidth();
	m_pWaitingWidget->setGeometry((width() - w) / 2, 160, w, m_pWaitingWidget->pixelHeight());
}

void DeviceSearchingDialog::refreshDeviceList()
{
	m_pDeviceList->setRowCount(m_deviceInfoCount);

	for (int i = 0; i < m_deviceInfoCount; i++)
	{
		DEVICE_INFO * pDeviceInfo = &m_deviceListInfo[i];
		showDevice(i, pDeviceInfo);
	}
}

void DeviceSearchingDialog::showDevice(int index, DEVICE_INFO *pDeviceInfo)
{
	m_pDeviceList->setRowHeight(index, 50);

	QTableWidgetItem *pTemp = new QTableWidgetItem(pDeviceInfo->m_deviceName);
	pTemp->setTextAlignment(Qt::AlignCenter);
	m_pDeviceList->setItem(index, 0, pTemp);

	if (pDeviceInfo->m_deviceAddr != 0xFFFFFFFF)
	{
		pTemp = new QTableWidgetItem(QHostAddress(pDeviceInfo->m_deviceAddr).toString());
	}
	else
	{
		pTemp = new QTableWidgetItem("USB Connected");	
	}
	pTemp->setTextAlignment(Qt::AlignCenter);
	if (pDeviceInfo->m_connectFlag == true)
	{
		pTemp->setIcon(QIcon(QPixmap(":lock.png")));
	}
	m_pDeviceList->setItem(index, 1, pTemp);

	pTemp = new QTableWidgetItem(pDeviceInfo->m_firmwareVersion);
	pTemp->setTextAlignment(Qt::AlignCenter);
	m_pDeviceList->setItem(index, 2, pTemp);
}


void DeviceSearchingDialog::clearAllDevice()
{
	m_deviceInfoCount = 0;
}

void DeviceSearchingDialog::addDevice(DEVICE_INFO deviceInfo)
{
	int i;

	for (i = 0; i < m_deviceInfoCount; i++)
	{
		if (m_deviceListInfo[i].m_deviceAddr == deviceInfo.m_deviceAddr)
		{
			break;
		}
	}

	m_deviceListInfo[i] = deviceInfo;

	if (i == m_deviceInfoCount)
	{
		L_INFO(QString("Add device: ") + QHostAddress(m_deviceListInfo[i].m_deviceAddr).toString() + QString(":") + m_deviceListInfo[i].m_deviceName);
		m_deviceInfoCount++;
	}
}

void DeviceSearchingDialog::dumpAvailableDevice()
{
	int i;
	qDebug() << "Dumping available devices...";
	for (i = 0; i < m_deviceInfoCount; i++)
	{
		qDebug() << "index " << i << ": " << m_deviceListInfo[i].m_deviceAddr << "-" << m_deviceListInfo[i].m_deviceName;
	}
}

void DeviceSearchingDialog::onConnectClicked()
{
	QList<QTableWidgetItem*> itemList = m_pDeviceList->selectedItems();
	int count = itemList.count();
	if (count == 3)
	{
		m_pConnectButton->setEnabled(true);
	}

	QTableWidgetItem* pItem = itemList[0];
	m_selectedDeviceIndex = pItem->row();

	accept();
}

DEVICE_INFO * DeviceSearchingDialog::getSelectedDevice()
{
	return (m_deviceListInfo + m_selectedDeviceIndex);
}