#include "WaittingWidget.h"
#include "mainWidget.h"
#include <QDebug>

WaititngWidget g_waittingWidget; 

bool mouseEventFilter::eventFilter(QObject* object,QEvent* event)
{
	if (g_waittingWidget.getWaittingFlag() == true)
	{
		switch (event->type())
		{
			case QEvent::MouseButtonDblClick:
			case QEvent::MouseButtonPress:
			case QEvent::MouseButtonRelease:
			case QEvent::MouseMove:
			case QEvent::KeyPress:
			case QEvent::KeyRelease:
			case QEvent::Wheel:
			case QEvent::Leave:
			case QEvent::Enter:
			case QEvent::NonClientAreaMouseMove:
			{
				/* do your stuff here */
				return (true);
			}
	
			default:
				break;
		}
	}
	else
	{
		if (event->type() == QEvent::ToolTip)
		{
			QWidget *pWidget = (QWidget *)object;
			QString whatsThis = pWidget->whatsThis();
//			qDebug() << pWidget << whatsThis;
			if (whatsThis != g_pMainWidget->reminderInfo())
			{
				g_pMainWidget->setReminderInfo(whatsThis);
			}

//			if (!whatsThis.isEmpty())
//			{
				return (true);
//			}
		}
	}
	return QObject::eventFilter(object,event);
}

WaititngWidget::WaititngWidget()
{
	m_waittingFlag = false;
}

/* 
	You must be very careful when using this g_waitCursorFlag. If it is set to true, it won't accept any mouse event.  
	During waitting time, you can't dispatch QMessageBox or any other dialog.
*/
void WaititngWidget::startWaitting(QWidget *parent, ICON_SIZE iconSize)
{
	if (m_waittingFlag == true)
	{
		stopWaitting();
	}

	m_waittingWidget = new BackgroundWidget(QString(":ActivityIndicator.png"), parent, 20, 2, QPainter::CompositionMode_Xor);
	m_oldCursor = parent->cursor();
	m_parentWidget = parent;
	m_waittingFlag = true;
	parent->setCursor(QCursor(Qt::WaitCursor));


	int centerX = parent->width() / 2;
	int centerY = parent->height() / 2;
	int w, h;

	if (iconSize == SAMLL_ICON)
	{
		w = h = 80;
	} 
	else if (iconSize == MEDIUM_ICON)
	{
		w = h = 100;
	}
	else
	{
		w = h = 130;
	}

	m_waittingWidget->setGeometry(centerX - w / 2, centerY - h / 2, w, h);
	m_waittingWidget->show();
}

void WaititngWidget::stopWaitting()
{
	if (m_waittingFlag == true)
	{
		m_waittingFlag = false;
		m_parentWidget->setCursor(m_oldCursor);
		m_waittingWidget->hide();
		delete m_waittingWidget;
		m_waittingWidget = nullptr;
	}
}

bool WaititngWidget::getWaittingFlag()
{
	return (m_waittingFlag);
}