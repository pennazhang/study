#include <MainWidget.h>
#include <QVariant>
#include <QMessageBox>
#include <QFileDialog>
#include "ShellSocket.h"

void setPosition(QWidget *pWidget, qreal x, qreal y, qreal w, qreal h)
{
	QWidget *pParentWidget = pWidget->parentWidget();

	int parentWidth = pParentWidget->width();
	int parentHeight = pParentWidget->height();

	int xp, yp, wp, hp;
	if ((x > -1) && (x < 1))
	{
		xp = x * parentWidth;
	}
	else
	{
		xp = x;
	}

	if ((y > -1) && (y < 1))
	{
		yp = y * parentHeight;
	}
	else
	{
		yp = y;
	}

	if ((w > -1) && (w < 1))
	{
		wp = w * parentWidth;
	}
	else
	{
		wp = w;
	}

	if ((h > -1) && (h < 1))
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
	m_pLabel = new QLabel(tr("Music Level"), this);
	m_pLabel->setAlignment(Qt::AlignCenter);
//    m_pLabel->setObjectName(QStringLiteral("WidgetLabel"));
	m_pLabel->adjustSize(); 
	m_pLabel->setWordWrap(true);

    m_pLineEdit = new QLineEdit(this);
	m_pLineEdit->setText("User AFE 1");
    setMinimumSize(200, 100);
    resize(300, 200);
}

MainWidget::~MainWidget()
{

}

void MainWidget::resizeEvent(QResizeEvent * /* event */)
{
	setPosition(m_pLabel, 0.1, 0.4, 0.2, 0.2);
	setPosition(m_pLineEdit, 0.33, 0.4, 0.4, 0.2);
}

void MainWidget::setSize(int width, int height)
{
	resize(width, height);
}

void MainWidget::showSize()
{
	outputString(QString("width = %1, height = %2").arg(width()).arg(height()));
}
