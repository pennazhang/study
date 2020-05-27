#include "UpgradeDialog.h"
#include "mainWidget.h"
#include "mainApplication.h"
#include <QMessageBox>
#include <QDate>
#include <QTime>

void UpgradeDialog::setReminder(QString reminder)
{
	m_pVerisonLabel->setText(reminder);
}

void UpgradeDialog::setOperation(UPDATE_OPERATION operaton) 
{ 
	m_operation = operaton;
	QString remainder;
	if (m_operation == UPGRADE_OPEATION)
	{
		remainder = QString ("\n") + tr("Do you want to start the upgrading process?");
	}
	else
	{
		remainder = QString ("\n") + tr("Do you want to start the downgrading process?");
	}
	m_pRemainderLabel->setText(remainder);
}

UpgradeDialog::UpgradeDialog(QWidget *parent)	: QDialog(parent)
{
	m_upgradeFlag = false;
	m_operation = UPGRADE_OPEATION;

	m_pVerisonLabel = new QLabel("", this);
	m_pVerisonLabel->setObjectName(QStringLiteral("BlackFont18"));
	m_pVerisonLabel->setWordWrap(true);
	
	QString remainder = QString ("\n") + tr("Do you want to start the upgrading process?");
	m_pRemainderLabel = new QLabel(remainder, this);
	m_pRemainderLabel->setObjectName(QStringLiteral("BlackFont24"));
	m_pRemainderLabel->setAlignment(Qt::AlignLeft | Qt::AlignTop);
	m_pRemainderLabel->setWordWrap(true);

	
	m_pPage1_Yes = new QPushButton(tr("Yes"), this);
	m_pPage1_Yes->setObjectName(QStringLiteral("BlackFont18"));
	connect(m_pPage1_Yes, SIGNAL(clicked()), this, SLOT(onClickPage1Yes()));

	m_pPage1_Cancel = new QPushButton(tr("No"), this);
	m_pPage1_Cancel->setObjectName(QStringLiteral("BlackFont18"));
	connect(m_pPage1_Cancel, SIGNAL(clicked()), this, SLOT(reject()));

	m_pWaitingWidget = new BackgroundWidget(QString(":ActivityIndicator.png"), this, 20, 2);
	m_pWaitingWidget->hide();

	setFixedSize(560, 340);
}

void UpgradeDialog::resizeEvent(QResizeEvent * /*event*/)
{
	int leftMargin = 40;
	int bottomMargin = 20;
	m_pVerisonLabel->setGeometry(leftMargin, bottomMargin, width() - 2 *  leftMargin, 80);
	m_pRemainderLabel->setGeometry(leftMargin, height() / 4 + 10, width() - 2 * leftMargin, 120);

//	int buttonTop = 310;
	int buttonWidth = 100;
	int buttonHeight = 35;
	m_pPage1_Yes->setGeometry(width() - 2 * leftMargin - 2 * buttonWidth, height() - bottomMargin - buttonHeight, buttonWidth, buttonHeight);
	m_pPage1_Cancel->setGeometry(width() - leftMargin - buttonWidth, height() - bottomMargin - buttonHeight, buttonWidth, buttonHeight);

	int w = m_pWaitingWidget->pixelWidth();
	int h = m_pWaitingWidget->pixelHeight();
	m_pWaitingWidget->setGeometry((width() - w) / 2, (height() - h) / 2, w, h);

}

void UpgradeDialog::onClickPage1Yes()
{
//	int leftMargin = 40;

	DeviceSocket *pDeviceSocket = g_pApp->getDeviceConnection();
	if (pDeviceSocket == nullptr)
	{
		QMessageBox::warning(this, tr("Warning"), tr("Can't connect to CPi2000 device, please check the network connection!"), QMessageBox::Ok);
		reject();
		return;
	}


	qDebug() << "Upgrading start...";
	m_pPage1_Yes->hide();
	m_pPage1_Cancel->hide();
	m_pVerisonLabel->hide();

	if (m_operation == UPGRADE_OPEATION)
	{
		m_pRemainderLabel->setText(tr("Upgrading..."));
	}
	else
	{
		m_pRemainderLabel->setText(tr("Downgrading..."));
	}

	m_pRemainderLabel->setAlignment(Qt::AlignHCenter);
	m_pWaitingWidget->show();

	
#if 0
	/* Before Upgrading, we need to backup the Preset.db first */
	do 
	{
		/* Add a telnet program to backup the presets.db in CPi2000 */
		BlockedTelnet telnet(this);
		QString strHostAddr = g_pApp->getDeviceConnection()->getDeviceAddr().toString();

	    if (!telnet.connectToHost(strHostAddr, 23, 5000))
		{
			qDebug() << "Failed to connect to Host:" << strHostAddr;
			break;
		}
		
		if (!telnet.waitFor("CPi2000 login", 3000))
		{
			qDebug() << "Failed to wait login";
			break;
		}
		telnet.sendData("root\r\n");

		if (!telnet.waitFor("Password:", 3000))
		{
			qDebug() << "Failed to Password";
			break;
		}
		telnet.sendData("cpidev\r\n");

		if (!telnet.waitFor("#", 3000))
		{
			qDebug() << "Failed to prompt";
			break;
		}

		telnet.sendData("cp /home/cpi/Software/Firmware/Model/Build/embBuild/Model/release/Presets.db /\r\n");
		if (!telnet.waitFor("#", 3000))
		{
			qDebug() << "Failed to prompt";
			break;
		}

		qDebug() << "telnet successfully!";
	} while (0);
#endif


	QString remoteFile = QString("ftp://%1/%2").arg(g_pApp->getDeviceConnection()->getDeviceAddr().toString()).arg("cpi2000.upd");
    QString localFile = QString(":/binaries/cpi2000.upd");
    QString userName = QString("cpi2000");
    QString password = QString("ftpftp");
    if (g_pApp->putFtpFile(localFile, remoteFile, userName, password) == false)
	{
		QMessageBox::warning(this, tr("Network Abnormal"), tr("Failed to transfer CPi2000 device file!"), QMessageBox::Ok);
		setUpgradeFlag(true);
		setUpgradeResult(false);
		reject();
		return;
	}

	m_pWaitingWidget->hide();
	
	if (m_operation == UPGRADE_OPEATION)
	{
		m_pRemainderLabel->setText(tr("Firmware is transfered to CPi2000 device, please re-connect the CPi2000 after its upgrading."));
	}
	else
	{
		m_pRemainderLabel->setText(tr("Firmware is transfered to CPi2000 device, please re-connect the CPi2000 after its downgrading."));
	}

	setUpgradeFlag(true);
	setUpgradeResult(true);
	m_pPage1_Cancel->setText(tr("OK"));
	m_pPage1_Cancel->show();

}
