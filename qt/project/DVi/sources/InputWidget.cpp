#include "InputWidget.h"
#include "IniSetting.h"
#include <QMessageBox>
#include <QDebug>
#include "commonLib.h"

InputWidget::InputWidget(QString backgroundImage, QWidget *parent)
	: BackgroundWidget(backgroundImage, parent)
{
	m_pClip1Led = new BackgroundWidget(QString(":light_green.png"), this);
	m_pClip2Led = new BackgroundWidget(QString(":light_green.png"), this);

	m_pTimer = new QTimer(this); 
	connect(m_pTimer, SIGNAL(timeout()), this, SLOT(onTimer_1s()));
	m_pTimer->start(1000);
}

void InputWidget::resizeEvent(QResizeEvent * /* event */)
{
	double ledLeft = 0.135;
	double ledTop = 0.02;
	double ledWidth = 0.14;
	double ledHeight = 0.062;
	::setGeometry(m_pClip1Led, ledLeft, ledTop, ledWidth, ledHeight);
	::setGeometry(m_pClip2Led, 1.02 - ledLeft - ledWidth, ledTop, ledWidth, ledHeight);
}

void InputWidget::paintEvent(QPaintEvent *event)
{
	BackgroundWidget::paintEvent(event);
}

void InputWidget::onTimer_1s()
{
	static bool flag = false;
	if (flag)
	{
		m_pClip1Led->setBackgroundImage(":light_green.png");
		m_pClip2Led->setBackgroundImage(":light_off.png");
	}
	else
	{
		m_pClip1Led->setBackgroundImage(":light_off.png");
		m_pClip2Led->setBackgroundImage(":light_green.png");
	}
	flag = !flag;
}
