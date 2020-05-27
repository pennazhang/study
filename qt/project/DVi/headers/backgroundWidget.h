#ifndef BACKGROUND_WIDGET_H
#define BACKGROUND_WIDGET_H
#include <QFrame>
#include <qtimer.h>
#include <QPixmap>
#include <QPainter>

class BackgroundWidget : public QFrame
{
    Q_OBJECT
public:
	BackgroundWidget(QString backgroundImage, QWidget *parent, int ms = 0, int stepAngle = 0, QPainter::CompositionMode mode = QPainter::CompositionMode_Source);
	void rotateEnable(bool flag);
	void setBackgroundImage(QString backgroundImage);

	int pixelWidth() { return m_background.width(); }
	int pixelHeight() { return m_background.height(); }


protected slots:
	void onTimerOut();

protected:
	void paintEvent(QPaintEvent *event);
	QPainter::CompositionMode m_mode;

	QPixmap m_background;
	QTimer *m_pTimer;

	int m_rotateAngle;
	int m_stepAngle;
	bool m_rotateFlag;
};

#endif
