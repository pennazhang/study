#ifndef EQWIDGET_H
#define EQWIDGET_H

#include <QWidget>
#include <QPixmap>
#include <QPushButton>
#include <QLabel>
#include <MarkButton.h>
#include <QLineEdit>
#include <QTimer>
#include "EQData.h"

typedef enum
{
	DRAGSTATUS_IDLE,
	DRAGSTATUS_PRESSED,
}DragStatus;

typedef enum
{
	DRAGEVENT_MOUSEMOVE,
	DRAGEVENT_MOUSEPRESSED,
	DRAGEVENT_MOUSEDBLCLICKED,
	DRAGEVENT_MOUSERELEASED,
}DragEvent;

typedef enum
{
	DRAG_POINT_GAIN,
	DRAG_POINT_LEFT_Q,
	DRAG_POINT_RIGHT_Q,
	DRAG_POINT_LPF_HPF,
}DragPoint;


class EQWidget : public QWidget
{
    Q_OBJECT

public:
	void setGain(int maxGain, int gainStep) { m_gainMax = maxGain; m_gainStep = gainStep; } 
	EQWidget(QWidget *parent = 0);
    ~EQWidget();
	int f2x(double x);
	int m2y(double x);
	double x2f(int x);
	double y2m(int y);
	virtual void clearFinalCurve();
	void update(int updateType);	/* 1: Update Completely;  2: Only update current Curve */

	virtual int getEQCount() = 0;
	virtual int getCurrentEQIndex() = 0;
	virtual EQData *getEQ(int index) = 0;
	virtual EQData *getCurrentEQ() = 0;
	virtual bool getEnableAllFlag() = 0;
	virtual double getMaxGain() { return m_gainMax; }
	QSize getVertexSize() { return QSize(m_vertexWidth, m_vertexHeight); }
	QString	getVertexText(int index) { return QString::number(index); }

protected:

	/* Draw coordinate */
	void updatePixmap1();

	/* Draw inactive EQ Curves */
	void updatePixmap2(QPixmap *pixmap);

	/* Draw Frequency Boxes */
	void updatePixmap3(QPixmap *pixmap);

	/* Draw current EQ Curve, total EQ Curve and control point */
	void updatePixmap4(QPixmap *pixmap);

	/* Draw current EQ's Main Control point */
	void updatePixmap5(QPixmap *pixmap);

	void paintEvent(QPaintEvent *event);
	void resizeEvent(QResizeEvent * event);
	void resetDimensionSize();
	void addToTotalCurve(double *pVertical);
	static QColor getDisabledEQColor() { return QColor(255, 255, 255, 27);; }
	void drawTotalEQCurve(QPainter *pPainter, QPen &pen, double *pVerticleY);
	void drawAnimationVertex(QPainter *painter, int dragFreqX, int dragFreqY, QColor color);
	void mousePressEvent(QMouseEvent *event);
	void mouseDoubleClickEvent(QMouseEvent *);
	void mouseReleaseEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void dispatchDragEvent(DragEvent dragEvent, QMouseEvent *event);
	void keyPressEvent(QKeyEvent *event);
	void wheelEvent(QWheelEvent *event); 
	static QColor getColor(int currentEQIndex);

	void drawEQ(QPainter * pPainter,  EQData *pEQData, QColor color, bool currentFlag, double *pVertical);

signals:
	void currentEQIndexChanged(int i);
	void currentEQChanged();
	void setPFFreq(double freq);

protected slots:
	void onAnimationTimer();

protected:
	QColor m_coordinateColor;  //Green color for coordinate.
	QColor m_textColor;	 //white number for frequency: 20, 40, 70, 100, 200, 400, 700, 1K, 2K, 4K ...
	QColor m_backgroundColor;

	int m_discreteCount;
	double m_pDimensionX[4096];	/* This is used to draw Final Curve */
	double m_pDimensionY[4096];	/* sum of all the unactive curve, exclude the active curve */

	QPixmap m_pixmap1, m_pixmap2, m_pixmap3, m_pixmap4, m_pixmap5;
/*
	All the graphical elements in this EQWidget can be divided into 3 types:
	1. the background elements, which include the coords line, coords label, and unselected curves (the parameters of the unselected curves won't change). 
	2. the foreground elements, which include the current curves, the total curve and the parameters box.
	3. Animation Vertex for the Gain of the selected curves.

	for the graphical element type 1, it will be drawn in updatePixmap1; for this situation, we need to set m_updateType = 1;
	for the graphical element type 2, it will be drawn in updatePixmap2; for this situation, we need to set m_updateType = 2;
	for the graphical element type 3, it will be drawn in updatePixmap3; for this situation, we need to set m_updateType = 3;
*/
	int m_updateType;	//1: update Type1 - 3; 2: update Type2 - 3; 3: update Type3; 0: need to update Type 1 - 3;
	int m_leftMargin, m_topMargin, m_rightMargin, m_bottomMargin;
	int m_radiusFreqPoint; //the radius for drag point of Freq
	int m_sqareQLen;		//the square length for the drag point of Q

	QTimer *m_pAnimatationTimer;
	int m_animationRadius;

	DragStatus m_dragStatus;
	DragPoint m_dragPoint;

	int m_gainMax, m_gainStep;
	int m_vertexWidth, m_vertexHeight;
};


#endif
