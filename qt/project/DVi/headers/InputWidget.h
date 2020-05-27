#ifndef INPUT_WIDGET_H
#define INPUT_WIDGET_H

#include <QWidget>
#include <QLabel>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QPushButton>
#include "ButtonSlider.h"
#include "backgroundWidget.h"
#include <QTimer>

class InputWidget : public BackgroundWidget
{
    Q_OBJECT
public:
    InputWidget(QString backgroundImage, QWidget *parent);

signals:

protected slots:
	void onTimer_1s();

protected:
	void resizeEvent(QResizeEvent * /* event */);
	void paintEvent(QPaintEvent *event);

protected:
	BackgroundWidget *m_pClip1Led, *m_pClip2Led;
	QTimer *m_pTimer;
};


#endif

