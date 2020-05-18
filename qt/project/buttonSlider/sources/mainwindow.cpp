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

#if 1
	m_pButtonSlider1 = new ButtonSlider(this, Qt::Vertical);
//	m_pButtonSlider1->setTextColor(QColor(0x10, 0x10, 0x10));
//	m_pButtonSlider1->setMarkColor(QColor(0x10, 0x10, 0x10));
//	QStringList analogStringList;
//	analogStringList << "-12.6dB" << "0dB"<<"1" << "2";
//	m_pButtonSlider1->setMark(2, 4, analogStringList);
	m_pButtonSlider1->setMarkLength(5, 7);
	m_pButtonSlider1->setMarkSide(true, true);
	m_pButtonSlider1->setRange(-126, 0);
	m_pButtonSlider1->setGrooveHeight(30);
	m_pButtonSlider1->setSingleStep(1);
	m_pButtonSlider1->setPageStep(1);

	for (int i = -126; i <= 0; i++)
	{
		if ((i % 10) == 0)
		{
			if ((i % 20) == 0)
			{
				m_pButtonSlider1->addExtraLongMark(i, QString("%1").arg(i), true);
			}
			else
			{
				m_pButtonSlider1->addExtraShortMark(i, QString(""));
			}
		}
	}

	m_pButtonSlider1->setValue(0);

#else
	m_pButtonSlider1 = new ButtonSlider(this, Qt::Vertical);
	m_pButtonSlider1->setRange(-40, 6);
	m_pButtonSlider1->setSingleStep(1);
	m_pButtonSlider1->setPageStep(1);
	m_pButtonSlider1->setGrooveHeight(30);
	for (int i = -50; i <= 5; i++)
	{
		if ((i % 5) != 0)
		{
			m_pButtonSlider1->addExtraShortMark(i, QString(""));
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
			m_pButtonSlider1->addExtraLongMark(i, text, boldFlag);
		}
	}
	m_pButtonSlider1->addExtraLongMark(6, "+6");
	m_pButtonSlider1->setValue(0);
#endif
    
    m_pOutputSettingButton = new QPushButton(tr("Output Setting"), this);
//	m_pOutputSettingButton->setObjectName(QStringLiteral("tabButton"));
//	connect(m_pOutputSettingButton, SIGNAL(clicked()), this, SLOT(onOutputSettingClicked()));
    m_pOutputSettingButton->setCheckable(true);

    setWindowTitle(tr("QT - GUI Demo"));
    setMinimumSize(400, 300);
    resize(600, 450);
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

//	setPosition(m_pButtonSlider1, 0.1, 0.1 , 0.6, 0.1);
	setPosition(m_pButtonSlider1, 0.1, 0.1 , 0.1, 0.6);
	
}
