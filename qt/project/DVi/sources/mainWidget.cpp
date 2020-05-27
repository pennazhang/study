#include "mainwidget.h"
#include "mainApplication.h"
#include <QMessageBox>
#include <QPainter>
#include "DeviceSearchingDialog.h"
#include "commonLib.h"
#include "BackgroundWidget.h"
#include "UpgradeDialog.h"
#include "ZoomDlg.h"
#include "simpleQtLogger.h"
#include "WaittingWidget.h"
#include "DeviceData.h"
#include "SQLiteDB.h"
#include "InputEQEditor.h"

#define MIN_SCROLL_WIDTH    1170
#define MIN_SCROLL_HEIGHT	645
MainWidget *g_pMainWidget = nullptr;

MainWidget::MainWidget(QWidget *parent)
    : QWidget(parent)
{
	m_debugActive = false;
	this->setObjectName(QStringLiteral("m_pMainWidget"));

#if 0
	m_pTopFrame = new QFrame(this);
	m_pTopFrame->setObjectName(QStringLiteral("m_pTopFrame"));

	createMenuBar(m_pTopFrame);
#else
	createMenuBar(this);
#endif

	m_pConnectButton = new MarkButton(tr("Offline"), this);
    m_pConnectButton->setObjectName(QStringLiteral("m_pConnectButton"));
	connect(m_pConnectButton, SIGNAL(clicked()), this, SLOT(onClickConnectButton()));
	m_pConnectButton->mark(false);

	/* Timer */
	m_pTimer1S = new QTimer(this);
	m_pTimer1S->start(1000);
	connect(m_pTimer1S, SIGNAL(timeout()), this, SLOT(on1STimerOut()));

	createPanelWidget();

	setMinimumSize(900, 400);
    resize(900, 400);

	connect(g_pApp, SIGNAL(deviceConnected()), this, SLOT(onDeviceConnected()));
	connect(g_pApp, SIGNAL(deviceDisconnected()), this, SLOT(onDeviceDisconnected()));

	g_pBroadcastCommunication = new BroadcastCommunication();
}

MainWidget::~MainWidget()
{
}


void MainWidget::createPanelWidget()
{
	m_pPanelWidget = new QWidget(this);
	m_pPanelWidget->setObjectName(QStringLiteral("m_pPanelWidget"));

	m_pFrame1 = new QFrame(m_pPanelWidget);
//	m_pFrame1->setFrameShape(QFrame::VLine); 
	m_pFrame1->setObjectName(QStringLiteral("m_pFrame1"));

	m_pFrame2 = new QFrame(m_pPanelWidget);
//	m_pFrame2->setFrameShape(QFrame::VLine); 
	m_pFrame2->setObjectName(QStringLiteral("m_pFrame2"));

	m_pFrame3 = new QFrame(m_pPanelWidget);
//	m_pFrame3->setFrameShape(QFrame::VLine); 
	m_pFrame3->setObjectName(QStringLiteral("m_pFrame3"));

	m_pInputLabel = new QLabel(tr("INPUT"), m_pPanelWidget);
	m_pInputLabel->setAlignment(Qt::AlignCenter);
	m_pInputLabel->setObjectName(QStringLiteral("BlackFont14"));

	m_pLevelLabel = new QLabel(tr("LEVEL"), m_pPanelWidget);
	m_pLevelLabel->setAlignment(Qt::AlignCenter);
	m_pLevelLabel->setObjectName(QStringLiteral("BlackFont14"));

	m_pOutputLabel = new QLabel(tr("OUTPUT"), m_pPanelWidget);
	m_pOutputLabel->setAlignment(Qt::AlignCenter);
	m_pOutputLabel->setObjectName(QStringLiteral("BlackFont14"));

	m_pInputWidget = new InputWidget(QString(":input.png"), m_pPanelWidget);
	m_pInputWidget->setObjectName(QStringLiteral("m_pInputWidget"));

	m_pLevelWidget = new QWidget(m_pPanelWidget);
	m_pLevelWidget->setObjectName(QStringLiteral("m_pLevelWidget"));

	m_pOutputWidget = new QWidget(m_pPanelWidget);
	m_pOutputWidget->setObjectName(QStringLiteral("m_pOutputWidget"));

	m_pPresetFrame = new QFrame(m_pPanelWidget);
	m_pPresetFrame->setObjectName(QStringLiteral("m_pPresetFrame"));
	{
		m_pPresetName = new QLabel("1: DSP OFF", m_pPresetFrame);
		m_pPresetName->setObjectName(QStringLiteral("m_pPresetName"));

		m_pRecallButton = new QPushButton(tr("RECALL"), m_pPresetFrame);
		m_pRecallButton->setObjectName(QStringLiteral("m_pRecallButton"));

		m_pStoreButton = new MarkButton(tr("STORE"), m_pPresetFrame);
		m_pStoreButton->setObjectName(QStringLiteral("m_pStoreButton"));

		m_pSignalGenButton = new MarkButton(tr("SIG GEN"), m_pPresetFrame);
		m_pSignalGenButton->setObjectName(QStringLiteral("m_pSignalGenButton"));

		m_pAmpButton = new QPushButton(tr("AMP INFO"), m_pPresetFrame);
		m_pAmpButton->setObjectName(QStringLiteral("m_pAmpButton"));
	}

	m_pDSPFrame = new QFrame(m_pPanelWidget);
	m_pDSPFrame->setObjectName(QStringLiteral("m_pDSPFrame"));
	{
		m_pFocusFrame = new QFrame(m_pDSPFrame);
		m_pFocusFrame->setObjectName(QStringLiteral("m_pFocusFrame"));

		m_pInputMode1Button = new DBLButton("", m_pDSPFrame);
		m_pInputMode1Button->setObjectName(QStringLiteral("m_pInputModeButton"));
		connect(m_pInputMode1Button, SIGNAL(clicked()), this, SLOT(onInputMode1Clicked()));	
		m_pInputMode1Button->setWhatsThis(QString("m_pInputMode1Button"));

		m_pInputMode2Button = new DBLButton("", m_pDSPFrame);
		m_pInputMode2Button->setObjectName(QStringLiteral("m_pInputModeButton"));
		connect(m_pInputMode2Button, SIGNAL(clicked()), this, SLOT(onInputMode2Clicked()));	
		m_pInputMode2Button->setWhatsThis(QString("m_pInputMode2Button"));

		m_pInputEQ1Button = new DBLButton("", m_pDSPFrame);
		m_pInputEQ1Button->setObjectName(QStringLiteral("m_pInputEQButton"));
		connect(m_pInputEQ1Button, SIGNAL(clicked()), this, SLOT(onInputEQ1Clicked()));	
		connect(m_pInputEQ1Button, SIGNAL(doubleClicked()), this,  SLOT(onDblClickInputEQ1()));
		m_pInputEQ1Button->setWhatsThis(QString("m_pInputEQ1Button"));

		m_pInputEQ2Button = new DBLButton("", m_pDSPFrame);
		m_pInputEQ2Button->setObjectName(QStringLiteral("m_pInputEQButton"));
		connect(m_pInputEQ2Button, SIGNAL(clicked()), this, SLOT(onInputEQ2Clicked()));	
		m_pInputEQ2Button->setWhatsThis(QString("m_pInputEQ2Button"));

		m_pXOver1Button = new DBLButton("", m_pDSPFrame);
		m_pXOver1Button->setObjectName(QStringLiteral("m_pXOverButton"));
		connect(m_pXOver1Button, SIGNAL(clicked()), this, SLOT(onXOver1Clicked()));	
		m_pXOver1Button->setWhatsThis(QString("m_pXOver1Button"));

		m_pXOver2Button = new DBLButton("", m_pDSPFrame);
		m_pXOver2Button->setObjectName(QStringLiteral("m_pXOverButton"));
		connect(m_pXOver2Button, SIGNAL(clicked()), this, SLOT(onXOver2Clicked()));	
		m_pXOver2Button->setWhatsThis(QString("m_pXOver2Button"));

		m_pOutputEQ1Button = new DBLButton("", m_pDSPFrame);
		m_pOutputEQ1Button->setObjectName(QStringLiteral("m_pOutputEQButton"));
		connect(m_pOutputEQ1Button, SIGNAL(clicked()), this, SLOT(onOutputEQ1Clicked()));	
		m_pOutputEQ1Button->setWhatsThis(QString("m_pOutputEQ1Button"));

		m_pOutputEQ2Button = new DBLButton("", m_pDSPFrame);
		m_pOutputEQ2Button->setObjectName(QStringLiteral("m_pOutputEQButton"));
		connect(m_pOutputEQ2Button, SIGNAL(clicked()), this, SLOT(onOutputEQ2Clicked()));	
		m_pOutputEQ2Button->setWhatsThis(QString("m_pOutputEQ2Button"));

		m_pDelay1Button = new DBLButton("", m_pDSPFrame);
		m_pDelay1Button->setObjectName(QStringLiteral("m_pDelayButton"));
		connect(m_pDelay1Button, SIGNAL(clicked()), this, SLOT(onDelay1Clicked()));	
		m_pDelay1Button->setWhatsThis(QString("m_pDelay1Button"));

		m_pDelay2Button = new DBLButton("", m_pDSPFrame);
		m_pDelay2Button->setObjectName(QStringLiteral("m_pDelayButton"));
		connect(m_pDelay2Button, SIGNAL(clicked()), this, SLOT(onDelay2Clicked()));	
		m_pDelay2Button->setWhatsThis(QString("m_pDelay2Button"));

		m_pLimiter1Button = new DBLButton("", m_pDSPFrame);
		m_pLimiter1Button->setObjectName(QStringLiteral("m_pLimiterButton"));
		connect(m_pLimiter1Button, SIGNAL(clicked()), this, SLOT(onLimiter1Clicked()));	
		m_pLimiter1Button->setWhatsThis(QString("m_pLimiter1Button"));

		m_pLimiter2Button = new DBLButton("", m_pDSPFrame);
		m_pLimiter2Button->setObjectName(QStringLiteral("m_pLimiterButton"));
		connect(m_pLimiter2Button, SIGNAL(clicked()), this, SLOT(onLimiter2Clicked()));	
		m_pLimiter2Button->setWhatsThis(QString("m_pLimiter2Button"));

		m_pOutputModeButton = new DBLButton("", m_pDSPFrame);
		m_pOutputModeButton->setObjectName(QStringLiteral("m_pOutputModeButton"));
		connect(m_pOutputModeButton, SIGNAL(clicked()), this, SLOT(onOutputModeClicked()));	
		m_pOutputModeButton->setWhatsThis(QString("m_pOutputModeButton"));

		m_pFocusButton = nullptr;
	}

	m_pReminderFrame = new QFrame(m_pPanelWidget);
	m_pReminderFrame->setObjectName(QStringLiteral("m_pReminderFrame"));
	{
		m_pReminderLabel = new QLabel("Crown Audio. Inc.", m_pReminderFrame);
		m_pReminderLabel->setObjectName(QStringLiteral("m_pReminderLabel"));
	}

	m_pCrownLabel = new QFrame(m_pPanelWidget);
	m_pCrownLabel->setObjectName(QStringLiteral("m_pCrownLabel"));
}

void MainWidget::refresh()
{
	refreshFocusButton();
}

void MainWidget::setFocusButton(QPushButton *pButton)
{
	m_pFocusButton = pButton;
	refreshFocusButton();
}

void MainWidget::refreshFocusButton()
{
	if (m_pFocusButton == nullptr)
	{
		m_pFocusFrame->hide();
		return;
	}

	m_pFocusFrame->show();
	QRect rect = m_pFocusButton->geometry();

	int differWidth = 3 + rect.width() * 0.1; 
	int differHeigh = 3 + rect.height() * 0.1;

	m_pFocusFrame->setGeometry(rect.left() - differWidth / 2, rect.top() - differHeigh / 2, rect.width() + differWidth, rect.height() + differHeigh);
#if 0
	MarkButton *pButtonList[] = {m_pInputMode1Button, m_pInputMode2Button, m_pInputEQ1Button, m_pInputEQ2Button, m_pXOver1Button, 
		m_pXOver2Button, m_pOutputEQ1Button, m_pOutputEQ2Button, m_pDelay1Button, m_pDelay2Button, m_pLimiter1Button, m_pLimiter2Button, m_pOutputModeButton };

	for (ULONG i = 0; i < sizeof(pButtonList) / sizeof(MarkButton *); i++)
	{
		if (m_pFocusButton == pButtonList[i])
		{
			pButtonList[i]->mark(true);
		}
		else
		{
			pButtonList[i]->mark(false);		
		}
	}
#endif
}

void MainWidget::resizeEvent(QResizeEvent * /* event */)
{
	int topFrameHeight = 26;

//    m_pTopFrame->setGeometry(0, 0, width(), topFrameHeight);
	{
		m_pMenuBar->setGeometry(0, 2, width() / 2, topFrameHeight);
	}

	m_pConnectButton->setGeometry(width() - 85, 2, 75, topFrameHeight - 4);

	int panelWidgetWidth = width() - 2;
	int panelWidgetHeight = height() - topFrameHeight - 2;

	m_pPanelWidget->setGeometry(1, topFrameHeight, panelWidgetWidth, panelWidgetHeight);

	m_pFrame1->setGeometry(panelWidgetWidth / 7 - 2, 7, 6, panelWidgetHeight - 14);
	m_pFrame2->setGeometry(panelWidgetWidth / 7 * 2 - 2, 7, 6, panelWidgetHeight - 14);
	m_pFrame3->setGeometry(panelWidgetWidth / 7 * 3 - 2, 7, 6, panelWidgetHeight - 14);

	int labelWidth = 100;
	int labelHeight = 20;
	m_pInputLabel->setGeometry(panelWidgetWidth / 14 - labelWidth / 2, panelWidgetHeight / 14, labelWidth, labelHeight);
	m_pLevelLabel->setGeometry(panelWidgetWidth  * 3 / 14 - labelWidth / 2, panelWidgetHeight / 14, labelWidth, labelHeight);
	m_pOutputLabel->setGeometry(panelWidgetWidth  * 5/ 14 - labelWidth / 2, panelWidgetHeight / 14, labelWidth, labelHeight);

	m_pInputWidget->setGeometry(5, panelWidgetHeight / 8, panelWidgetWidth / 7 - 10, panelWidgetHeight  * 7 / 8 - 5);
	m_pLevelWidget->setGeometry(5 + panelWidgetWidth / 7, panelWidgetHeight / 8, panelWidgetWidth / 7 - 10, panelWidgetHeight  * 7 / 8 - 5);
	m_pOutputWidget->setGeometry(5 + panelWidgetWidth * 2 / 7, panelWidgetHeight / 8, panelWidgetWidth / 7 - 10, panelWidgetHeight  * 7 / 8 - 5);

	m_pPresetFrame->setGeometry(panelWidgetWidth * 6.5/ 14, panelWidgetHeight / 14, panelWidgetWidth * 7 / 14, panelWidgetHeight * 3/ 14);
	{
		::setGeometry(m_pPresetName,  POSITION(0.03), POSITION(0.15), POSITION(0.6), POSITION(0.3));
		::setGeometry(m_pRecallButton,  POSITION(0.7), POSITION(0.15), POSITION(0.13), POSITION(0.3));
		::setGeometry(m_pStoreButton,  POSITION(0.85), POSITION(0.15), POSITION(0.13), POSITION(0.3));
		::setGeometry(m_pSignalGenButton,  POSITION(0.03), POSITION(0.6), POSITION(0.15), POSITION(0.3));
		::setGeometry(m_pAmpButton,  POSITION(0.25), POSITION(0.6), POSITION(0.15), POSITION(0.3));
	}
	m_pDSPFrame->setGeometry(panelWidgetWidth * 6.5/ 14, panelWidgetHeight * 6.5/ 14, panelWidgetWidth * 7 / 14, panelWidgetHeight * 4 / 14);
	{
		double ch1Top = 0.25;
		double ch2Top = 0.6;
		double buttonWidth = 0.073;
		double buttonHeight = 0.3;

		::setGeometry(m_pInputMode1Button,	POSITION(0.08 * 1), POSITION(ch1Top), POSITION(buttonWidth), POSITION(buttonHeight));
		::setGeometry(m_pInputEQ1Button,	POSITION(0.12 * 2), POSITION(ch1Top), POSITION(buttonWidth), POSITION(buttonHeight));
		::setGeometry(m_pXOver1Button,		POSITION(0.12 * 3), POSITION(ch1Top), POSITION(buttonWidth), POSITION(buttonHeight));
		::setGeometry(m_pOutputEQ1Button,	POSITION(0.12 * 4), POSITION(ch1Top), POSITION(buttonWidth), POSITION(buttonHeight));
		::setGeometry(m_pDelay1Button,		POSITION(0.12 * 5), POSITION(ch1Top), POSITION(buttonWidth), POSITION(buttonHeight));
		::setGeometry(m_pLimiter1Button,	POSITION(0.12 * 6), POSITION(ch1Top), POSITION(buttonWidth), POSITION(buttonHeight));

		::setGeometry(m_pInputMode2Button,	POSITION(0.08 * 1), POSITION(ch2Top), POSITION(buttonWidth), POSITION(buttonHeight));
		::setGeometry(m_pInputEQ2Button,	POSITION(0.12 * 2), POSITION(ch2Top), POSITION(buttonWidth), POSITION(buttonHeight));
		::setGeometry(m_pXOver2Button,		POSITION(0.12 * 3), POSITION(ch2Top), POSITION(buttonWidth), POSITION(buttonHeight));
		::setGeometry(m_pOutputEQ2Button,	POSITION(0.12 * 4), POSITION(ch2Top), POSITION(buttonWidth), POSITION(buttonHeight));
		::setGeometry(m_pDelay2Button,		POSITION(0.12 * 5), POSITION(ch2Top), POSITION(buttonWidth), POSITION(buttonHeight));
		::setGeometry(m_pLimiter2Button,	POSITION(0.12 * 6), POSITION(ch2Top), POSITION(buttonWidth), POSITION(buttonHeight));

		::setGeometry(m_pOutputModeButton,  POSITION(0.12 * 7), POSITION(0.4), POSITION(buttonWidth), POSITION(buttonHeight));
		refreshFocusButton();
	}

	int logoWidth = panelWidgetHeight * 1.2 / 14 * 379 / 106;
	m_pCrownLabel->setGeometry(panelWidgetWidth * 13.8 / 14 - logoWidth, panelWidgetHeight * 12/ 14, logoWidth, panelWidgetHeight * 1.2 / 14);
	int reminderWidth = panelWidgetWidth * 13.8 / 14 - logoWidth - panelWidgetWidth * 6.5/ 14 +20;
	m_pReminderFrame->setGeometry(panelWidgetWidth * 6.5/ 14, panelWidgetHeight * 12/ 14, reminderWidth, panelWidgetHeight * 1.2 / 14);
	{
		::setGeometry(m_pReminderLabel, POSITION(0.03), POSITION(0.1), POSITION(0.9), POSITION(0.8));
	}
}

void MainWidget::onClickConnectButton()
{
	if (g_pApp->getConnectStatus() == DEVICE_CONNECTED)
	{
		DeviceSocket *pDeviceSocket = g_pApp->getDeviceConnection();
		pDeviceSocket->endHeartBeat();
		g_pApp->msleep(100);
		g_pApp->disconnectDevice();
	}
	else
	{
		DeviceSearchingDialog searchingDlg(this);
		searchingDlg.setModal(true);
		int ret = searchingDlg.exec();
		if (ret == QDialog::Accepted) 
		{
			// connect Button clicked 
			DEVICE_INFO *device = searchingDlg.getSelectedDevice();
			g_pApp->connectDevice(QHostAddress(device->m_localIPAddress), QHostAddress(device->m_deviceAddr)); 
		}
	}
}


void MainWidget::onDeviceConnected()
{
//	CPi2000Data *pData = getCPi2000Data();

	bool dispatchOK = false;
	QString strVersion;

	QCursor oldCursor = cursor();
	setCursor(QCursor(Qt::WaitCursor));

	if (g_pApp->getConnectStatus() == DEVICE_CONNECTED)
	{
		m_pConnectButton->mark(true);
		m_pConnectButton->setText(tr("Connecting..."));
//		g_pApp->getDeviceConnection()->saveDB();
//		g_pApp->getDeviceConnection()->SaveDeviceFile(REMOTE_DEFAULT_DEVICE_FILE);


		// We can use this to replace g_pApp->msleep(3000);
		QString hallName;
		if (g_pApp->getDeviceConnection()->getHallName(hallName))
		{
//			pData->m_strHallName = hallName;
		}
		else
		{
			L_ERROR("Failed to get hall name!");
			dispatchOK = false;
			goto END;	
		}
		g_pApp->msleep(1000);
	}
	else
	{
		L_ERROR("Invalid socket connecting state!");
		dispatchOK = false;
		goto END;	
	}
	
	if (g_pApp->getDeviceConnection()->getDeviceFWVersion(strVersion) == false)
	{
		L_ERROR("Failed to get firmware version!");
		dispatchOK = false;
		goto END;
	}

	/* Let's upgrade the firmware */
	if (strVersion != DVi_NEW_FW_VERSION)
	{
		if (strVersion.indexOf(DVi_MAIN_VERSION_MASK) == 0)
		{
			QRegExp rx("(\\d+).(\\d+).(\\d+).(\\d+)"); 
			int deviceVersion = 0, currentVersion = 0;
	
			rx.exactMatch(DVi_NEW_FW_VERSION);
			currentVersion = rx.cap(4).toInt();

			rx.exactMatch(strVersion);
			deviceVersion = rx.cap(4).toInt();

			if (currentVersion > deviceVersion)
			{
				// Reminder the user to upgrade device
				UpgradeDialog upgradeDlg(this);
				upgradeDlg.setModal(true);

				QString reminder = QString(tr("The device's firmware version is %1. It can be upgraded to %2. However it can still work even you don't upgrade the firmware.").arg(strVersion).arg(DVi_NEW_FW_VERSION));
				upgradeDlg.setReminder(reminder);
				upgradeDlg.exec();
	
				if (upgradeDlg.getUpgradeFlag() == true)
				{
					L_INFO("Upgrade Firmware");
					g_pApp->getDeviceConnection()->resetDevice();
					g_pApp->msleep(1000);
					emit m_pConnectButton->clicked();
					goto END;
				}
			}
			else if (currentVersion < deviceVersion)
			{
				// Reminder the user to upgrade GUI
				QString reminder = QString(tr("There is new version of GUI application for this device's firmware. Please upgrade the GUI application ASAP.")); 
				QMessageBox::warning(this, tr("Waring"), reminder, tr("Ok"));
			}
		}
		else
		{
			UpgradeDialog upgradeDlg(this);
			upgradeDlg.setModal(true);

			QString reminder = QString(tr("The device's firmware version is %1, and it can't match current GUI application. It must be upgraded to %2 first.").arg(strVersion).arg(DVi_NEW_FW_VERSION));
			upgradeDlg.setReminder(reminder);
			upgradeDlg.exec();
	
			if (upgradeDlg.getUpgradeFlag() == true)
			{
				L_INFO("Upgrade Firmware");
				g_pApp->getDeviceConnection()->resetDevice();
				g_pApp->msleep(1000);
			}
			emit m_pConnectButton->clicked();
			goto END;
		}
	}

	/* 	Add by percy for get .dev file	*/
#if 0
	QString tempDevFile = MainApplication::getTempDevFileName();
	QString tempSpeakerFile = MainApplication::getTempSpeakerDBFileName();
	QString tempPresetFile = MainApplication::getTempPresetDBFileName();

	QFile::remove(tempDevFile);
	QFile::remove(tempSpeakerFile);
	QFile::remove(tempPresetFile);

	L_INFO("Geting device file...");
	if (g_pApp->getFtpFile(REMOTE_DEFAULT_DEVICE_FILE, tempDevFile) == false)
	{
		L_ERROR("Failed to get device file");
		QMessageBox::warning(this, "Error", "Failed to load CPi2000 device file!", QMessageBox::Ok);
		emit m_pConnectButton->clicked();
		setCursor(oldCursor);
		return;
	}
	else
	{
		if (loadFromDeviceFile(tempDevFile) == false)
		{
			emit m_pConnectButton->clicked();
			setCursor(oldCursor);
			return;		
		}
	}


	qDebug() << "==============================================================================";

	/* SUB Master Volume */
	DeviceSocket *pDeviceSocket = g_pApp->getDeviceConnection();
	pDeviceSocket->subOutputVolume();

	if (m_currentTabIndex == 2)
	{
		g_pApp->getDeviceConnection()->subInputVolume();
	}
	else
	{
		g_pApp->getDeviceConnection()->unsubInputVolume();		
	}


//	bool dhcpEnable;
	NETWORK_SETTING networkSetting;
	if (pDeviceSocket->getDeviceNetworkAddr(networkSetting, 2) == true)
	{
		pData->setNetworkAddress(networkSetting);
	}

	/* Get Serial Number */
	QString serialNumber = "Not available";
	pDeviceSocket->getSerialNumber(serialNumber);
	pData->m_strSerialNumber = serialNumber;

	float masterVolumeinDB;
	if (pDeviceSocket->getMasterVolume(masterVolumeinDB))
	{
		float msaterVolume = DBToMasterVolume(masterVolumeinDB);
		pData->setMasterVolume(msaterVolume);
	}

	g_pApp->getDeviceConnection()->subUSBStatus();

	m_pLogWidget->clearLog();
	m_pBasicSettingWidget->setSyncChecked(false);
	refresh();
#endif
	g_pApp->msleep(1000);
	dispatchOK = true;
	setWindowTitle(QString("DVi - %1").arg(g_pApp->getDeviceConnection()->getDeviceAddr().toString()));

	if (g_debugEnable == false)
	{
		clearBit(g_debug, LOG_SOCKET_MESSAGE);
	}

END:
	setCursor(oldCursor);
	setEnabled(true);

	if (dispatchOK == false)
	{
//		QMessageBox::warning(this, "Internet Error", "Failed to connecte device!", QMessageBox::Ok);
		g_pApp->disconnectDevice();
	}
	refreshConnectedButton();
}


void MainWidget::onDeviceDisconnected()
{
	refreshConnectedButton();
	setWindowTitle(QString("DVi"));
}

void MainWidget::refreshConnectedButton()
{
	DEVICE_CONNECT_STATUS status = g_pApp->getConnectStatus();

	bool connectedFlag = false;

	switch (status)
	{
	case DEVICE_DISCONNECTED:
		m_pConnectButton->mark(0);
		m_pConnectButton->setText(tr("Offline"));
//		m_pSendButton->setEnabled(false);
		break;
	default:
		m_pConnectButton->mark(1);
		m_pConnectButton->setText(tr("Connected"));
		connectedFlag = true;
//		m_pSendButton->setEnabled(true);
	}

	m_pActionLoadDeviceFile->setEnabled(true);
	m_pActionSaveDeviceFile->setEnabled(true);
	m_pActionRestore->setEnabled(true);
	m_pActionAdjustTime->setEnabled(true);
	m_pActionSpeakerEditor->setEnabled(true);
	m_pActionImportSpeaker->setEnabled(connectedFlag);
	if (connectedFlag)
	{
		m_pActionUpgrade->setEnabled(true);
	}
	else
	{
		m_pActionUpgrade->setEnabled(false);
	}

	m_pSetIPAddress->setEnabled(true);
//	refreshUSBFileMenu();
//	m_pLogWidget->refreshButtonStatus(connectedFlag);

	return;
}

void MainWidget::on1STimerOut()
{
	static int s_sendCount = 0;		/* Time stamp that send the message to CPi2000 */

	if (g_pApp->getConnectStatus() == DEVICE_CONNECTED)
	{
		DeviceSocket *pDeviceSocket = g_pApp->getDeviceConnection();
		pDeviceSocket->sendHeartBeat();

		s_sendCount++;
		if (pDeviceSocket->getShakeHandFlag() == true)
		{
			s_sendCount = 0;					
		}

		if (s_sendCount > 5)
		{
			g_waittingWidget.stopWaitting();
			L_ERROR("Recvive no message from device " + pDeviceSocket->getDeviceAddr().toString() + " in 5 seconds, socket is broken and CPi2000 is disconnected!");
			QMessageBox::warning(this, tr("Connection broken"), tr("The CPi2000 is disconnected. Please check the device status!"), QMessageBox::Ok);
			emit m_pConnectButton->clicked();
		}
	}
#if 0
	// refresh the recv count. 
	static int oldRecvCount = m_recvCount;
	if (oldRecvCount != m_recvCount)
	{
		m_pRecvEdit->setText(QString::number(m_recvCount, 10));
		oldRecvCount = m_recvCount;
	}
#endif
}


void MainWidget::createMenuBar(QWidget *pParent)
{
	m_pActionLoadDeviceFile = new QAction(tr("Device File..."), this);
    connect(m_pActionLoadDeviceFile, SIGNAL(triggered()), this, SLOT(onOpenDeviceFile()));

	m_pActionLoadFromUSB = new QAction(tr("from USB..."), this);
    connect(m_pActionLoadFromUSB, SIGNAL(triggered()), this, SLOT(onLoadUSBFile()));

	m_pActionSaveDeviceFile = new QAction(tr("Device File..."), this);
    connect(m_pActionSaveDeviceFile, SIGNAL(triggered()), this, SLOT(onSaveDeviceFile()));

	m_pActionSaveToUSB = new QAction(tr("to USB..."), this);
    connect(m_pActionSaveToUSB, SIGNAL(triggered()), this, SLOT(onSaveUSBFile()));

	m_pActionExit = new QAction(tr("Exit"), this);
	connect(m_pActionExit, SIGNAL(triggered()), this, SLOT(close()));

	m_pMenuBar = new QMenuBar(pParent);
	m_pMenuBar->setObjectName(QStringLiteral("m_pMenuBar"));

    m_pMenu1 = m_pMenuBar->addMenu(tr("File"));
	
	m_pMenuOpen = new QMenu(tr("Load"));
    m_pMenuOpen->addAction(m_pActionLoadDeviceFile);
    m_pMenuOpen->addAction(m_pActionLoadFromUSB);
	m_pMenu1->addMenu(m_pMenuOpen);

	m_pMenuSave = new QMenu(tr("Save"));
    m_pMenuSave->addAction(m_pActionSaveDeviceFile);
    m_pMenuSave->addAction(m_pActionSaveToUSB);
	m_pMenu1->addMenu(m_pMenuSave);
	m_pMenu1->addAction(m_pActionExit);

	m_pActionSpeakerEditor = new QAction(tr("Speaker Editor..."), this);
    connect(m_pActionSpeakerEditor, SIGNAL(triggered()), this, SLOT(onSpeakerEditor()));

	m_pActionImportSpeaker = new QAction(tr("Import Speaker..."), this);
    connect(m_pActionImportSpeaker, SIGNAL(triggered()), this, SLOT(onSpeakerImport()));

	m_pActionAdjustTime = new QAction(tr("Calibrate Time..."), this);
    connect(m_pActionAdjustTime, SIGNAL(triggered()), this, SLOT(onAdjustTime()));

	m_pActionUpgrade = new QAction(tr("Upgrade Firmware..."), this);
    connect(m_pActionUpgrade, SIGNAL(triggered()), this, SLOT(onUpgradeFirmware()));
	m_pActionUpgrade->setEnabled(false);

	m_pActionRestore = new QAction(tr("Restore Factory Settings..."), this);
    connect(m_pActionRestore, SIGNAL(triggered()), this, SLOT(onRestoreFactory()));

	m_pSetIPAddress = new QAction(tr("Set IP Address..."), this);
    connect(m_pSetIPAddress, SIGNAL(triggered()), this, SLOT(onSetIPAddress()));

	m_pActionZoom = new QAction(tr("Zoom..."), this);
    connect(m_pActionZoom, SIGNAL(triggered()), this, SLOT(onZoom()));

	m_pActionOption = new QAction(tr("Debug Option..."), this);
    connect(m_pActionOption, SIGNAL(triggered()), this, SLOT(onOption()));

	m_pMenu2 = m_pMenuBar->addMenu(tr("Tools"));
	m_pMenu2->addAction(m_pActionSpeakerEditor);
//	m_pMenu2->addAction(m_pActionImportSpeaker);
	m_pMenu2->addAction(m_pActionAdjustTime);
	m_pMenu2->addAction(m_pActionUpgrade);
	m_pMenu2->addAction(m_pActionRestore);
	m_pMenu2->addAction(m_pSetIPAddress);
//	m_pMenu2->addSeparator();
	m_pMenu2->addAction(m_pActionZoom);
	m_pActionZoom->setVisible(false);
	m_pMenu2->addAction(m_pActionOption);
	m_pActionOption->setVisible(false);

	m_pActionAbout = m_pMenuBar->addAction(tr("About"));
	connect(m_pActionAbout, SIGNAL(triggered()), this, SLOT(about()));

	connect(m_pMenu1, SIGNAL(aboutToShow()), this, SLOT(refreshAction())); 

    m_pMenuShortcut = new QShortcut(QKeySequence((Qt::CTRL + Qt::Key_C), (Qt::CTRL + Qt::Key_F)), this);
    m_pMenuShortcut->setContext(Qt::ApplicationShortcut);
    connect(m_pMenuShortcut, SIGNAL(activated()), this, SLOT(onMenuShortcutActivated()));
}

void MainWidget::onMenuShortcutActivated()
{
	m_debugActive = true;
	m_pActionZoom->setVisible(m_debugActive);
	m_pActionOption->setVisible(m_debugActive);
}


void MainWidget::onZoom()
{
	ZoomDlg zoomDlg(this);
	zoomDlg.setModal(true);

	zoomDlg.exec();
}

void MainWidget::onOption()
{
	OptionDlg optionDlg(this);
	optionDlg.setModal(true);

	optionDlg.exec();
}

void MainWidget::showEvent(QShowEvent * /*event*/)
{
	DeviceData *pData = getDeviceData();
	static int count = 0;

	if (count == 0)
	{
		QString localPresetDB	= g_pApp->getTempPresetDBFileName();
		g_pDatabase->closeDB();				/* We must close the Database before copy file */
		QFile::remove(localPresetDB);
		QFile::copy(QString(":/binaries/FactoryPresets.db"), localPresetDB);
		QFile::setPermissions(localPresetDB, QFileDevice::WriteOwner | QFileDevice::WriteUser | QFileDevice::WriteGroup | QFileDevice::WriteOther | QFileDevice::ReadOwner | QFileDevice::ReadUser | QFileDevice::ReadGroup | QFileDevice::ReadOther);

		/* Load Presets.db */
		{
			bool flag = pData->loadFromJSON(localPresetDB);
			if (flag == false)
			{
				QMessageBox::warning(this, QString("Internal Error"), QString("Failed to load %1").arg(localPresetDB), QMessageBox::Ok);
			}

			pData->setCurrentPresetID(pData->getCurrentPresetID() + 1);
			pData->setMasterGain1(pData->getMasterGain1() + 100);
			pData->saveToJSON("Presets_xxx.db");
		}

		count++;
	}
}

void MainWidget::onInputMode1Clicked()
{
	setFocusButton(m_pInputMode1Button);
}

void MainWidget::onInputMode2Clicked()
{
	setFocusButton(m_pInputMode2Button);
}

void MainWidget::onInputEQ1Clicked()
{
	setFocusButton(m_pInputEQ1Button);
}

void MainWidget::onInputEQ2Clicked()
{
	setFocusButton(m_pInputEQ2Button);
}

void MainWidget::onXOver1Clicked()
{
	setFocusButton(m_pXOver1Button);
}

void MainWidget::onXOver2Clicked()
{
	setFocusButton(m_pXOver2Button);
}

void MainWidget::onOutputEQ1Clicked()
{
	setFocusButton(m_pOutputEQ1Button);
}

void MainWidget::onOutputEQ2Clicked()
{
	setFocusButton(m_pOutputEQ2Button);
}

void MainWidget::onDelay1Clicked()
{
	setFocusButton(m_pDelay1Button);
}

void MainWidget::onDelay2Clicked()
{
	setFocusButton(m_pDelay2Button);
}

void MainWidget::onLimiter1Clicked()
{
	setFocusButton(m_pLimiter1Button);
}

void MainWidget::onLimiter2Clicked()
{
	setFocusButton(m_pLimiter2Button);
}

void MainWidget::onOutputModeClicked()
{
	setFocusButton(m_pOutputModeButton);
}

void MainWidget::onDblClickInputEQ1()
{
	InputEQEditor editor(this);
	/* 临时解决方案，保证可以在editor中使用save to device*/
//	connect(&editor, SIGNAL(applyToDevice(SpeakerData * )), this, SLOT(SaveSpeakerToDevice(SpeakerData *)));
//	editor.setSpeakerData(&speakerData);
//	editor.setApplyChannel(s_result.m_speakerChannelApplied);
	editor.setModal(true);
	int editorResult = editor.exec();

	if (editorResult == QDialog::Accepted)
	{
//		SaveSpeakerToDevice(editor.getSpeakerData());
	}
//	disconnect(&editor, SIGNAL(applyToDevice(SpeakerData * )), this, SLOT(SaveSpeakerToDevice(SpeakerData *)));
}

QString MainWidget::reminderInfo() 
{
	return (m_reminderInfo);
}

void MainWidget::setReminderInfo(QString reminderInfo)
{
//	qDebug() << "setReminderInfo" << reminderInfo;
	if (m_reminderInfo == reminderInfo)
	{
		return;
	}

	m_reminderInfo = reminderInfo;
	if (m_reminderInfo.isEmpty())
	{
		m_pReminderLabel->setText("Crown Audio. Inc.");
	}
	else
	{
		m_pReminderLabel->setText(m_reminderInfo);
	}
}


DBLButton::DBLButton(QString buttonText , QWidget *pParent)
	:QPushButton(buttonText, pParent)
{

}

void DBLButton::mouseDoubleClickEvent(QMouseEvent *)
{
	emit doubleClicked();
}
