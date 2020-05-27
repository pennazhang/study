#ifndef WAITTING_Widget_H
#define WAITTING_Widget_H

#include <QPixmap>
#include <QDialog>
#include <QEvent>
#include <QPainter>
#include "BackgroundWidget.h"

/* It is very dangerous to use mouseEventFilter. When you stop a mouse event, you will stop all the GUI application such as QMessageBox or some abnormal dispatch dialog  */
class mouseEventFilter: public QObject
{
public:
	mouseEventFilter():QObject() { }
	~mouseEventFilter() { }
	bool eventFilter(QObject* object,QEvent* event);
};

typedef enum
{
	SAMLL_ICON = 0,
	MEDIUM_ICON,
	BIG_ICON
} ICON_SIZE;

class WaititngWidget
{
public:
	WaititngWidget();
	void startWaitting(QWidget *parent, ICON_SIZE iconSize = MEDIUM_ICON);
	void stopWaitting();
	bool getWaittingFlag();

protected:
	BackgroundWidget *m_waittingWidget;
	bool m_waittingFlag;
	QWidget *m_parentWidget;
	QCursor m_oldCursor;
};

extern WaititngWidget g_waittingWidget; 

#endif

