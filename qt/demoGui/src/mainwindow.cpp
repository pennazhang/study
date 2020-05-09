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

	setPosition(m_pOutputSettingButton, (width() - buttonWidth) / 2 , 0.45, buttonWidth, 0.1);
}
