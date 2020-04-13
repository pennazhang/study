#include <MainWidget.h>
#include <QVariant>
#include <QMessageBox>
#include <QFileDialog>
#include "IniSetting.h"
#include "MainApplication.h"
#include <QScreen>

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

MainWidget::MainWidget(QWidget *parent)
    : QWidget(parent)
{
	/* Create Label */
	QScreen *pScreen = g_pApp->primaryScreen ();
	QRect mm =pScreen->availableGeometry() ;
	int screen_width = mm.width();
	int screen_height = mm.height();
	float currentDPI = pScreen->logicalDotsPerInch();

	QFont font("Tahoma");
	font.setPointSize(8);
	g_pApp->setFont(font);

	m_pLabel_default = new QLabel(this);
	m_pLabel_default->setText(QString("(Tahoma_8_Point): AutoScaleFlag = %1, AutoScaleFactor = %2\nDPI = %3, screen_width = %4, screen_height = %5").arg(g_pIniSetting->m_scaleFlag).arg(g_pIniSetting->m_scaleFactor) .arg(currentDPI).arg(screen_width).arg(screen_height));
	m_pLabel_default->setWordWrap(true);

	m_pLabel_10px = new QLabel(tr("Black_Font_10px"), this);
    m_pLabel_10px->setObjectName("Black_Font_10px");
	m_pLabel_10px->setAlignment(Qt::AlignCenter);
	m_pLabel_10px->setWordWrap(true);

	m_pLabel_12px = new QLabel(tr("Black_Font_12px"), this);
    m_pLabel_12px->setObjectName("Black_Font_12px");
	m_pLabel_12px->setAlignment(Qt::AlignCenter);
	m_pLabel_12px->setWordWrap(true);

	m_pLabel_14px = new QLabel(tr("Black_Font_14px"), this);
    m_pLabel_14px->setObjectName("Black_Font_14px");
	m_pLabel_14px->setAlignment(Qt::AlignCenter);
	m_pLabel_14px->setWordWrap(true);

	m_pLabel_10pt = new QLabel(tr("Black_Font_10pt"), this);
    m_pLabel_10pt->setObjectName("Black_Font_10pt");
	m_pLabel_10pt->setAlignment(Qt::AlignCenter);
	m_pLabel_10pt->setWordWrap(true);

	m_pLabel_12pt = new QLabel(tr("Black_Font_12pt"), this);
    m_pLabel_12pt->setObjectName("Black_Font_12pt");
	m_pLabel_12pt->setAlignment(Qt::AlignCenter);
	m_pLabel_12pt->setWordWrap(true);

	m_pLabel_14pt = new QLabel(tr("Black_Font_14pt"), this);
    m_pLabel_14pt->setObjectName("Black_Font_14pt");
	m_pLabel_14pt->setAlignment(Qt::AlignCenter);
	m_pLabel_14pt->setWordWrap(true);

    resize(500, 400);
}

MainWidget::~MainWidget()
{

}

void MainWidget::resizeEvent(QResizeEvent * /* event */)
{
	setPosition(m_pLabel_default, 0.1, 0.1, 0.8, 0.2);
	setPosition(m_pLabel_10px, 0.1, 0.3, 0.4, 0.2);
	setPosition(m_pLabel_12px, 0.1, 0.5, 0.4, 0.2);
	setPosition(m_pLabel_14px, 0.1, 0.7, 0.4, 0.2);
	setPosition(m_pLabel_10pt, 0.5, 0.3, 0.4, 0.2);
	setPosition(m_pLabel_12pt, 0.5, 0.5, 0.4, 0.2);
	setPosition(m_pLabel_14pt, 0.5, 0.7, 0.4, 0.2);
}
