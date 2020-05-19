#include "mainwindow.h"


void setPosition(QWidget *pWidget, qreal x, qreal y, qreal w, qreal h)
{
	QWidget *pParentWidget = pWidget->parentWidget();
		
	int parentWidth = pParentWidget->width();
	int parentHeight = pParentWidget->height();

	int xp, yp, wp, hp;
	if ((x > 0) && (x < 1))
	{
		xp = x * parentWidth;
	}
	else
	{
		xp = x;
	}

	if ((y > 0) && (y < 1))
	{
		yp = y * parentHeight;
	}
	else
	{
		yp = y;
	}

	if ((w > 0) && (w < 1))
	{
		wp = w * parentWidth;
	}
	else
	{
		wp = w;
	}

	if ((h > 0) && (h < 1))
	{
		hp = h * parentHeight;
	}
	else
	{
		hp = h;
	}
	pWidget->setGeometry(xp, yp, wp, hp);
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
//	this->setObjectName("BackGround");

	m_gain = 0.0;

	m_pPEQCutSpin1 = new QDoubleSpinBox(this);
	m_pPEQCutSpin1->setRange(-12.6, 0);
	m_pPEQCutSpin1->setSingleStep(0.1);
	m_pPEQCutSpin1->setDecimals(1);
	m_pPEQCutSpin1->setSuffix(" dB");
	connect(m_pPEQCutSpin1, SIGNAL(valueChanged(double)), this, SLOT(onPEQCutChanged1(double)));



	// ------------------------------m_pButtonSlider1--------------------------------------------------
	m_pButtonSlider1 = new ButtonSlider(this, Qt::Vertical);
	m_pButtonSlider1->setTextColor(QColor(0x10, 0x10, 0x10));
	m_pButtonSlider1->setMarkColor(QColor(0x10, 0x10, 0x10));
//	QStringList analogStringList;
//	analogStringList << "-12.6dB" << "0dB"<<"1" << "2";
//	m_pButtonSlider1->setMark(2, 4, analogStringList);
//	m_pButtonSlider1->setMarkLength(5, 7);
	m_pButtonSlider1->setMarkSide(false, true);
	m_pButtonSlider1->setRange(-126, 0);
//	m_pButtonSlider1->setGrooveHeight(30);
	m_pButtonSlider1->setSingleStep(1);
	m_pButtonSlider1->setPageStep(1);
	m_pButtonSlider1->setCenterOffset(-11);		/* This value should be set as QSlider::groove:vertical.left / 2 */

	for (int i = -126; i <= 0; i++)
	{
		if ((i % 10) == 0)
		{
			if ((i % 20) == 0)
			{
				m_pButtonSlider1->addExtraLongMark(i, QString("%1").arg(i/10), true);
			}
			else
			{
				m_pButtonSlider1->addExtraShortMark(i, QString(""));
			}
		}
	}
	connect(m_pButtonSlider1, SIGNAL(valueChanged(int)), this, SLOT(onSWPEQGainSliderChanged1(int)));

	// ------------------------------m_pButtonSlider2--------------------------------------------------
	m_pButtonSlider2 = new ButtonSlider(this);
	m_pButtonSlider2->setTextColor(QColor(0x10, 0x10, 0x10));
	m_pButtonSlider2->setMarkColor(QColor(0x10, 0x10, 0x10));
	QStringList analogStringList;
	analogStringList << "-12.6dB" << "-6.3dB" << "0dB";
	m_pButtonSlider2->setMark(3, 4, analogStringList);
	m_pButtonSlider2->setMarkLength(5, 7);
	m_pButtonSlider2->setMarkSide(true, true);
	m_pButtonSlider2->setRange(-126, 0);
	m_pButtonSlider2->setGrooveHeight(30);
	m_pButtonSlider2->setSingleStep(1);
	m_pButtonSlider2->setPageStep(1);
	m_pButtonSlider2->setValue(0);
	connect(m_pButtonSlider2, SIGNAL(valueChanged(int)), this, SLOT(onSWPEQGainSliderChanged2(int)));

	// ------------------------------m_pButtonSlider3--------------------------------------------------
	m_pButtonSlider3 = new ButtonSlider(this, Qt::Horizontal);
	m_pButtonSlider3->setRange(-40, 6);
	m_pButtonSlider3->setSingleStep(1);
	m_pButtonSlider3->setPageStep(1);
	m_pButtonSlider3->setMarkSide(true, false);
	m_pButtonSlider3->setCenterOffset(10);	/* This value should be set as QSlider::groove:horizontal.top / 2 */

//	m_pButtonSlider3->setGrooveHeight(30);
	for (int i = -50; i <= 5; i++)
	{
		if ((i % 5) != 0)
		{
			m_pButtonSlider3->addExtraShortMark(i, QString(""));
		}
		else
		{
			bool boldFlag = false;
			QString text;
			if ( i == 0)
			{
				text.sprintf("0");
				boldFlag = true;
			}
			else if (i % 10 == 0)
			{
				text.setNum(i);
			}
			m_pButtonSlider3->addExtraLongMark(i, text, boldFlag);
		}
	}
	m_pButtonSlider3->addExtraLongMark(6, "+6");
	m_pButtonSlider3->setValue(0);

	// ------------------------------m_pButtonSlider4--------------------------------------------------
	m_pButtonSlider4 = new ButtonSlider(this, Qt::Horizontal);
	m_pButtonSlider4->setTextColor(QColor(0x10, 0x10, 0x10));
	m_pButtonSlider4->setMarkColor(QColor(0x10, 0x10, 0x10));
	analogStringList.clear();
	analogStringList << "-12.6dB" << "-6.3dB" << "0dB";
	m_pButtonSlider4->setMark(3, 4, analogStringList);
	m_pButtonSlider4->setMarkLength(5, 7);
	m_pButtonSlider4->setMarkSide(true, true);
	m_pButtonSlider4->setRange(-126, 0);
	m_pButtonSlider4->setGrooveHeight(30);
	m_pButtonSlider4->setSingleStep(1);
	m_pButtonSlider4->setPageStep(1);
	m_pButtonSlider4->setValue(0);
	m_pButtonSlider3->setCenterOffset(10);	/* This value should be set as QSlider::groove:horizontal.top / 2 */


    
    m_pOutputSettingButton = new QPushButton(tr("Output Setting"), this);
//	m_pOutputSettingButton->setObjectName(QStringLiteral("tabButton"));
//	connect(m_pOutputSettingButton, SIGNAL(clicked()), this, SLOT(onOutputSettingClicked()));
    m_pOutputSettingButton->setCheckable(true);

    setWindowTitle(tr("QT - GUI Demo"));
    setMinimumSize(400, 300);
    resize(600, 450);
}

void MainWindow::refreshGEQGroup_SW()
{
	bool oldSlotEnableFlag = m_slotEnableFlag;
	m_slotEnableFlag = false;

#if 0
	CPi2000Data *pData = getCPi2000Data();
	OutputChannel *pChannel = pData->getCurrentOutputChannel();

	EQData *pEQ = pChannel->getLShelf();	/* We will use m_lShelf as PEQ1 */
#endif 
	int gain_10x = qRound(m_gain * 10);

	m_pPEQCutSpin1->setValue(m_gain);
	m_pButtonSlider1->setValue(gain_10x);
	m_pButtonSlider2->setValue(gain_10x);

	m_slotEnableFlag = oldSlotEnableFlag;

}


void MainWindow::onSWPEQGainSliderChanged2(int gain_10x)
{
	if (m_slotEnableFlag == false)
	{
		return;
	}

#if 1
	m_gain = (double)gain_10x / 10;
#else
	CPi2000Data *pData = getCPi2000Data();
	OutputChannel *pChannel = pData->getCurrentOutputChannel();

	EQData *pEQ = pChannel->getLShelf();	/* We will use m_hShelf as PEQ */
	pEQ->setGain((float)gain_10x / 10);

	DeviceSocket *pSocket = g_pApp->getDeviceConnection();
	if (g_pApp->getConnectStatus() == DEVICE_NETWORK_CONNECTED)
	{
		pSocket->setSWBellCut(1, (float)gain_10x / 10);
	}
#endif	
	refreshGEQGroup_SW();
}


void MainWindow::onSWPEQGainSliderChanged1(int gain_10x)
{
	if (m_slotEnableFlag == false)
	{
		return;
	}

#if 1
	m_gain = (double)gain_10x / 10;
#else
	CPi2000Data *pData = getCPi2000Data();
	OutputChannel *pChannel = pData->getCurrentOutputChannel();

	EQData *pEQ = pChannel->getLShelf();	/* We will use m_hShelf as PEQ */
	pEQ->setGain((float)gain_10x / 10);

	DeviceSocket *pSocket = g_pApp->getDeviceConnection();
	if (g_pApp->getConnectStatus() == DEVICE_NETWORK_CONNECTED)
	{
		pSocket->setSWBellCut(1, (float)gain_10x / 10);
	}
#endif	
	refreshGEQGroup_SW();
}


void MainWindow::onPEQCutChanged1(double gain)
{
	if (m_slotEnableFlag == false)
	{
		return;
	}

#if 1
	m_gain = gain;
#else
	CPi2000Data *pData = getCPi2000Data();
	OutputChannel *pChannel = pData->getCurrentOutputChannel();

	EQData *pEQ = pChannel->getLShelf();	/* We will use m_hShelf as PEQ */
	pEQ->setGain(gain);

	DeviceSocket *pSocket = g_pApp->getDeviceConnection();
	if (g_pApp->getConnectStatus() == DEVICE_NETWORK_CONNECTED)
	{
		pSocket->setSWBellCut(1, gain);
	}
#endif
	refreshGEQGroup_SW();

}


MainWindow::~MainWindow()
{
}

void MainWindow::resizeEvent(QResizeEvent* /* event */)
{
    int buttonWidth = width() / 5;
    if (buttonWidth < 140)
    {
        buttonWidth = 140;
    }

	setPosition(m_pOutputSettingButton, width() - buttonWidth, 0 , buttonWidth, 0.1);

	
	setPosition(m_pButtonSlider1, 0.1, 0.1 , 0.15, 0.5);
	setPosition(m_pPEQCutSpin1, 0.1, 0.025 , 0.15, 0.07);

	setPosition(m_pButtonSlider2, 0.3, 0.1 , 0.15, 0.5);

	setPosition(m_pButtonSlider3, 0.1, 0.6 , 0.7, 0.15);
	setPosition(m_pButtonSlider4, 0.1, 0.8 , 0.7, 0.15);


}
