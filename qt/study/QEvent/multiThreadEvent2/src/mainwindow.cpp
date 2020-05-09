#include "mainwindow.h"
#include <QApplication>
#include <QDebug>
#include "main.h"

QEvent::Type CustomerEvent::m_EventType = QEvent::Type(EVENT_CUSTOMER_EVENT);


CustomerEvent::CustomerEvent(QString valueString) : QEvent(eventType())
{
    m_String = valueString;
}

QEvent::Type CustomerEvent::eventType()
{
    // register Event Type
    if (m_EventType == QEvent::None)
	{
        m_EventType = (QEvent::Type)QEvent::registerEventType();
	}

    return m_EventType;
}

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
//	int *a = new int[10];
    m_pPostEventButton = new QPushButton(tr("Post Event"), this);
	connect(m_pPostEventButton, SIGNAL(clicked()), this, SLOT(onClickedPostEvent()));

    m_pSendEventButton = new QPushButton(tr("Send Event"), this);
	connect(m_pSendEventButton, SIGNAL(clicked()), this, SLOT(onClickedSendEvent()));

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
    int buttonHeight = height() / 10;

    setPosition(m_pPostEventButton, (width() - buttonWidth) * 2 / 3, (height() - buttonHeight) / 2, buttonWidth, buttonHeight);
    setPosition(m_pSendEventButton, (width() - buttonWidth) / 3, (height() - buttonHeight) / 2, buttonWidth, buttonHeight);
}

void MainWindow::onClickedPostEvent()
{
    CustomerEvent *pCustomerEvent = new CustomerEvent("PostCustomerEvent");
    QCoreApplication::postEvent(g_pReceiverObject, pCustomerEvent);
    qDebug() << "PostCustomerEvent called... ";
}

void MainWindow::onClickedSendEvent()
{
    QEvent *pCustomerEvent = new QEvent((QEvent::Type)EVENT_CUSTOMER_EVENT_1);
    bool result = QCoreApplication::sendEvent(g_pReceiverObject, pCustomerEvent);
    qDebug() << "The Dispose Result Is " << result;
    delete pCustomerEvent;
}

bool MainWindow::event(QEvent *pEvent)
{
    switch(pEvent->type())
    {
		case EVENT_CUSTOMER_EVENT:
			{
				CustomerEvent *customerEvent = dynamic_cast<CustomerEvent*>(pEvent);
				qDebug() << "Captured CUSTOMER_EVENT: " << customerEvent->getValueString();
				return true;
			}

		case EVENT_CUSTOMER_EVENT_1:
			qDebug() << "Captured EVENT_CUSTOMER_EVENT_1: ";
			return true;	
	}
    return QWidget::event(pEvent);
}