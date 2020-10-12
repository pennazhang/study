#include "eqwidget.h"
#include <QStylePainter>
#include <QPainter>
#include <QVariant>
#include <QtCore/qmath.h>
#include <QtDebug>
#include <math.h>
#include <QMouseEvent>
#include <QApplication>
#include <QMessageBox>
#include <QRadialGradient>
#include "mainApplication.h"
#include "DeviceSocket.h"

#define ANIMATION_MAX_RADIUS 16

extern int g_defaultFontSize;

bool isEQEnable(EQType type, bool eqEnable, bool PEQEnable)
{
	if ((type == EQ_PEAKING) || (type == EQ_L_SHELF) || (type == EQ_H_SHELF))
	{
		return (eqEnable && PEQEnable);
	}
	else
	{
		return (eqEnable);
	}
}

EQWidget::EQWidget(QWidget *parent)
    : QWidget(parent)
{
	m_leftMargin = 40;
	m_topMargin = 10;
	m_rightMargin = 30;
	m_bottomMargin = 20;

	m_coordinateColor = qRgb(44, 78, 78);
	m_textColor = qRgb(0xd0, 0xd0, 0xd0);
	m_backgroundColor = qRgb(0x0, 0x0, 0x0);
	m_radiusFreqPoint = 4;
	m_sqareQLen = 8;

	m_dragStatus = DRAGSTATUS_IDLE;
	setMouseTracking(true);

	m_pAnimatationTimer = new QTimer(this);
    connect(m_pAnimatationTimer, SIGNAL(timeout()), this, SLOT(onAnimationTimer()));
	m_pAnimatationTimer->start(200);
	m_animationRadius = 2;

	m_gainMax = 15;
	m_gainStep = 3;
}

EQWidget::~EQWidget()
{

}

void EQWidget::updatePixmap1()
{
//	EQGroupData *pEQGroupData = getEQGroupData();
	m_pixmap1 = QPixmap(size());

	QPainter painter(&m_pixmap1);
	painter.initFrom(this);
	painter.fillRect(QRectF(0, 0, width(), height()), QBrush(m_backgroundColor, Qt::SolidPattern));

	painter.setPen(QPen(m_coordinateColor, 1));

	/* Draw Vertical coordinate */
	int freq[36] = {10, 20, 30, 40, 50, 60, 70, 80, 90
		,100, 200, 300, 400, 500, 600, 700, 800, 900
		,1000, 2000, 3000, 4000, 5000, 6000, 7000, 8000, 9000
		,10000, 20000, 30000, 40000, 50000, 60000, 70000, 80000, 90000};

	for (int i = 0; i < 36; i++)
	{
		if (freq[i] < MIN_EQ_FREQUENCY)
		{
			continue;
		}
		if (freq[i] > MAX_EQ_FREQUENCY)
		{
			break;
		}
#if 0
		int x1 = f2x(freq[i]);
		int y1 = m2y(-m_gainMax);
		int y2 = m2y(m_gainMax);
#endif
		painter.drawLine(f2x(freq[i]), m2y(-m_gainMax), f2x(freq[i]), m2y(m_gainMax));
	}

	/* Draw Horizontal coordinate */
	for (int i = -m_gainMax ; i <= m_gainMax; i += m_gainStep)
	{
		painter.drawLine(f2x(MIN_EQ_FREQUENCY), m2y(i), f2x(MAX_EQ_FREQUENCY), m2y(i));
	}


	/* Draw Text */
//	beginDrawText(painter);
	painter.setPen(QPen(m_textColor, 1));

	/* Draw horizontal coordinate text*/
	QString text;
	bool check;
	for (int i = 0; i < 36; i++)
	{
		check = true;
		if (freq[i] < MIN_EQ_FREQUENCY)
		{
			continue;
		}
		if (freq[i] > MAX_EQ_FREQUENCY)
		{
			break;
		}

		switch (freq[i])
		{
		case 1:
		case 2:
		case 4:
		case 7:
		case 10:
		case 20:
		case 40:
		case 70:
		case 100:
		case 200:
		case 400:
		case 700:
			text.setNum(freq[i]);
			break;
		case 1000:
		case 2000:
		case 4000:
		case 7000:
		case 10000:
		case 20000:
		case 40000:
		case 70000:
		case 100000:
			text.setNum(freq[i]/1000);
			text += QString("K");
			break;
		default:
			check = false;
		}
		if (check == true)
		{
			QRect rect(f2x(freq[i]) -20, m2y(-m_gainMax) + 5, 40, 20);
			painter.drawText(rect, Qt::AlignCenter, text);
			painter.drawLine(f2x(freq[i]), m2y(-m_gainMax), f2x(freq[i]), m2y(-m_gainMax) + 5);
		}
	}

	/* Draw Vertical coordinate text */
	for (int i = -m_gainMax ; i <= m_gainMax; i += m_gainStep)
	{
		text.setNum(i, 10);
		QRect rect(f2x(MIN_EQ_FREQUENCY) - 40, m2y(i) - 10, 25, 20); 
		painter.drawText(rect, Qt::AlignVCenter | Qt::AlignRight, text);
		painter.drawLine(f2x(MIN_EQ_FREQUENCY) - 5, m2y(i), f2x(MIN_EQ_FREQUENCY), m2y(i));
	}

#if 0
	/* Now we can draw EQ Curves */
	double pVertical[4000];
	clearFinalCurve();
	{
		/* Let's draw the unselected EQ curve */
		int currentEQIndex = getCurrentEQIndex();
		for (int i = 0; i < getEQCount(); i++)
		{
			EQData *pEQData = getEQ(i);
			if (i != currentEQIndex)
			{
				for (int verticalIndex = 0; verticalIndex < 4000; verticalIndex++)
				{
					pVertical[verticalIndex] = 0;
				}
				EQType type = pEQData->getType();
				QColor color = getColor(i);

				bool EQEnableFlag = isEQEnable(type, pEQData->getEnableFlag(), getEnableAllFlag());
				if (EQEnableFlag)
				{
					color.setAlpha(60);
				}
				else
				{
					color = getDisabledEQColor();
				}


				/* For PEQ, we draw it here */
				if ((type == EQ_PEAKING) || (type == EQ_L_SHELF) || (type == EQ_H_SHELF))
				{
					if (fabs(pEQData->getGain()) > 0.01)		/* gain != 0 */
					{
						pEQData->dBCalc(m_discreteCount, m_pDimensionX, pVertical);
						drawEQCurve(&painter, QBrush(color), pEQData, pVertical);
					}
				}

				/* For HPF, we draw it here */
				else if ((type == EQ_HPF) || (type == EQ_LPF))
				{
					pEQData->dBCalc(m_discreteCount, m_pDimensionX, pVertical);
//					drawPFCurve(&painter, QBrush(color), pEQData, pVertical);
				}

				else if (type == EQ_USER_DEFINED_FIR)
				{
					pEQData->dBCalc(m_discreteCount, m_pDimensionX, pVertical);
//					drawPFCurve(&painter, QBrush(color), pEQData, pVertical);
				}

				/* Add it to total curve */
				if (EQEnableFlag)
				{
					addToTotalCurve(pVertical);
				}
			}
		}
	}
#endif
}

void EQWidget::updatePixmap2(QPixmap *pixmap)
{
	QPainter painter(pixmap);

	/* Now we can draw EQ Curves */
	double pVertical[4096];
	clearFinalCurve();
	{
		/* Let's draw the unselected EQ curve */
		int currentEQIndex = getCurrentEQIndex();
		for (int i = 0; i < getEQCount(); i++)
		{
			EQData *pEQData = getEQ(i);
			if (i != currentEQIndex)
			{
				for (int verticalIndex = 0; verticalIndex < 4096; verticalIndex++)
				{
					pVertical[verticalIndex] = 0;
				}
				QColor color = getColor(i);

				bool EQEnableFlag = getEnableAllFlag() && pEQData->isEnabled(); 
				if (EQEnableFlag)
				{
					color.setAlpha(60);
				}
				else
				{
					color = getDisabledEQColor();
				}

				pEQData->dBCalc(m_discreteCount, m_pDimensionX, pVertical);
				pEQData->drawEQCurve(&painter, this, m_discreteCount, pVertical, color);

				/* Add it to total curve */
				if (EQEnableFlag)
				{
					addToTotalCurve(pVertical);
				}
			}
		}
	}
#if 0
	EQData *pCurrentEQ = getCurrentEQ();
	int currentEQIndex = getCurrentEQIndex();

	double pVertical[4000];
	double pTotalCurveY[4096];	/* This is used to draw Total Curve */
	for (int i = 0; i < m_discreteCount; i++)
	{
		pTotalCurveY[i] = m_pDimensionY[i];
		pVertical[i] = 0;
	}

	EQType type = pCurrentEQ->getType();
	QColor color = getColor(currentEQIndex);
	bool EQEnableFlag = isEQEnable(type, pCurrentEQ->getEnableFlag(), getEnableAllFlag());
	if (EQEnableFlag)
	{
		color.setAlpha(200);
	}
	else
	{
		color = getDisabledEQColor();
	}

	if ((type == EQ_H_SHELF) || (type == EQ_L_SHELF) || (type == EQ_PEAKING))
	{
		if (fabs(pCurrentEQ->getGain()) >= 0.01)
		{
			/* First Let's draw the current EQ curve */
			pCurrentEQ->dBCalc(m_discreteCount, m_pDimensionX, pVertical);
			drawEQCurve(&painter, QBrush(color), pCurrentEQ, pVertical);
		}
	}
	else if ((type == EQ_HPF) || (type == EQ_LPF))
	{
		/* First Let's draw the current EQ curve */
		pCurrentEQ->dBCalc(m_discreteCount, m_pDimensionX, pVertical);
		drawPFCurve(&painter, QBrush(color), pCurrentEQ, pVertical);
	}
	else if (type == EQ_USER_DEFINED_FIR)
	{
		/* First Let's draw the current EQ curve */
		pCurrentEQ->dBCalc(m_discreteCount, m_pDimensionX, pVertical);
//		drawFIRCurve(&painter, QBrush(color), pCurrentEQ, pVertical);	
	}

	/* Then we need to recalculate the total EQ Curve */
	if (EQEnableFlag)
	{
		for (int i = 0; i < m_discreteCount; i++)
		{
			pTotalCurveY[i] += pVertical[i];
		}
	}

	/* Then, we can draw the total curve */
	color = QColor(255, 255, 255);
	drawTotalEQCurve(&painter, QPen(color, 2), pTotalCurveY);

	/* Then we can draw the draggable point for the selected EQ Curve */
	QFont font = painter.font();
	QFont newFont(font);

	newFont.setPointSize(qRound(g_defaultFontSize * 1.5));
	painter.setFont(newFont);


	/* Draw vertex text for PEQ  */
	for (int i = 0; i < getEQCount(); i++)
	{
		EQData *pEQData = getEQ(i);
		int eqType = pEQData->getType();

		if ((eqType == EQ_PEAKING) || (eqType == EQ_L_SHELF) || (eqType == EQ_H_SHELF))
		{
			int dragFreqX = f2x(pEQData->getFrequency());
			int dragFreqY = m2y(pEQData->getGain());;
			QString text;
			text.setNum(i + 1);
			if (pEQData->getGain() > 0)
			{
				if (dragFreqY - 25 > m2y(m_gainMax))
				{
					QRect rect(dragFreqX - 15, dragFreqY - 30, 30, 20);
					painter.drawText(rect, Qt::AlignCenter, text);
				}
				else
				{
					QRect rect(dragFreqX - 15, dragFreqY + 5, 30, 20);
					painter.drawText(rect, Qt::AlignCenter, text);				
				}
			}
			else
			{
				if (dragFreqY + 25 < m2y(-m_gainMax))
				{
					QRect rect(dragFreqX - 15, dragFreqY + 10, 30, 20);
					painter.drawText(rect, Qt::AlignCenter, text);
				}
				else
				{
					QRect rect(dragFreqX - 15, dragFreqY - 30, 30, 20);
					painter.drawText(rect, Qt::AlignCenter, text);
				}
			}
		}
	}

	/* Draw control point for current curve  */
	{
		QColor color;
		color.setAlpha(255);
		color.setRgb(qRgb(255, 255, 255));

		/* For PQE, we need to draw the Q control point */
		if (pCurrentEQ->getType() == EQ_PEAKING)
		{
			if (fabs(pCurrentEQ->getGain()) > 0.05)
			{
				/* draw the point for the Q */
				painter.setPen(QPen(color, 1));
				double centerX = pCurrentEQ->getFreqForQ(0);
				double centerY = pCurrentEQ->dBCalc(centerX);
				painter.drawRect(f2x(centerX) - m_sqareQLen/2, m2y(centerY) - m_sqareQLen/2, m_sqareQLen, m_sqareQLen);
				centerX = pCurrentEQ->getFreqForQ(1);
				centerY = pCurrentEQ->dBCalc(centerX);
				qDebug() << "centerX = " << centerX << ", centerY = " << centerY << ", gain = " << pCurrentEQ->getGain() << ", Q = " << pCurrentEQ->getQ();
				/* centerX =  323.607 , centerY =  nan , gain =  -2.77556e-17 , Q =  1 */
				painter.drawRect(f2x(centerX) - m_sqareQLen/2, m2y(centerY) - m_sqareQLen/2, m_sqareQLen, m_sqareQLen);
			}
		}
	}
	/* We need to restore the font */
	painter.setFont(font);
#endif
}

void EQWidget::updatePixmap3(QPixmap *pixmap)
{
	QPainter painter(pixmap);
}

void EQWidget::updatePixmap4(QPixmap *pixmap)
{
	QPainter painter(pixmap);

	EQData *pCurrentEQ = getCurrentEQ();
	int currentEQIndex = getCurrentEQIndex();

	double pVertical[4096];
	double pTotalCurveY[4096];	/* This is used to draw Total Curve */
	for (int i = 0; i < m_discreteCount; i++)
	{
		pTotalCurveY[i] = m_pDimensionY[i];
		pVertical[i] = 0;
	}

	QColor color = getColor(currentEQIndex);

	bool EQEnableFlag = getEnableAllFlag() && pCurrentEQ->isEnabled(); 
	if (EQEnableFlag)
	{
		color.setAlpha(200);
	}
	else
	{
		color = getDisabledEQColor();
	}

	pCurrentEQ->dBCalc(m_discreteCount, m_pDimensionX, pVertical);
	pCurrentEQ->drawEQCurve(&painter, this, m_discreteCount, pVertical, color);

	/* Add it to total curve */
	if (EQEnableFlag)
	{
		for (int i = 0; i < m_discreteCount; i++)
		{
			pTotalCurveY[i] += pVertical[i];
		}
	}
	
	/* Then, we can draw the total curve */
	QPen pen(QColor(Qt::white), 2);
	drawTotalEQCurve(&painter, pen, pTotalCurveY);

	/* draw the non-main control point */
	int controlPoint = pCurrentEQ->getControlPointCount();
	int startControlPoint = 0;
	if (pCurrentEQ->hasMainControlPoint())
	{
		startControlPoint = 1;
	}
	for (int i = startControlPoint; i < controlPoint; i++)
	{
		ControlPoint controlPoint = pCurrentEQ->getControlPoint(i);

		painter.setPen(QPen(color, 1));
		painter.drawRect(f2x(controlPoint.m_frequency) - m_sqareQLen/2, m2y(controlPoint.m_gain) - m_sqareQLen/2, m_sqareQLen, m_sqareQLen);
	}

	/* Then we can draw the draggable point for the selected EQ Curve */
	painter.setPen(QPen(QColor(255, 255, 255), 1));
	QFont font = painter.font();
	QFont newFont(font);
	newFont.setPointSize(qRound(g_defaultFontSize * 1.5));
	painter.setFont(newFont);

	/* draw vertex Text for EQ Curve */
	for (int i = 0; i < getEQCount(); i++)
	{
		QString vertexText = getVertexText(i);
		if (vertexText.isEmpty())
		{
			continue;
		}

		EQData *pEQData = getEQ(i);
		
		if (!pEQData->hasMainControlPoint())
		{
			continue;
		}

//		ControlPoint controlPoint = pCurrentEQ->getControlPoint(0);
		pEQData->drawVertexText(&painter, this, vertexText);
	}
}

void EQWidget::updatePixmap5(QPixmap *pixmap)
{
	QPainter painter(pixmap);
}


void EQWidget::drawTotalEQCurve(QPainter *pPainter, QPen &pen, double *pVerticleY)
{

	QPainterPath path;
	
	pPainter->setRenderHint(QPainter::HighQualityAntialiasing);

	pPainter->setPen(pen);
	{
		int startX = f2x(MIN_EQ_FREQUENCY);
		int staryY = m2y(pVerticleY[0]);
	
		path.moveTo(startX + 1, staryY);
	
		for (int i = 1; i < m_discreteCount; i++)
		{
			int x = startX + i;
			int y = m2y(pVerticleY[i]);

			if ((pVerticleY[i] <= m_gainMax) && (pVerticleY[i] >= -m_gainMax) && (pVerticleY[i - 1] <= m_gainMax) && (pVerticleY[i - 1] >= -m_gainMax))
			{
				path.lineTo(x, y);
			}
			else
			{
				path.moveTo(x, y);
			}
		}
	}

	pPainter->drawPath(path);
}


/*
	All the graphical elements in this EQWidget can be divided into 3 types:
	1. the background elements, which include the coords line, coords label, and unselected curves (the parameters of the unselected curves won't change). 
	2. the foreground elements, which include the current curves, the total curve and the parameters box.
	3. Animation Vertex for the Gain of the selected curves.

	for the graphical element type 1, it will be drawn in updatePixmap1; for this situation, we need to set m_updateType = 1;
	for the graphical element type 2, it will be drawn in updatePixmap2; for this situation, we need to set m_updateType = 2;
	for the graphical element type 3, it will be drawn in updatePixmap3; for this situation, we need to set m_updateType = 3;
*/
void EQWidget::paintEvent(QPaintEvent * /* event */)
{
//	LOG << "paintEvent: type = %d" << m_updateType;
	int updateType = m_updateType;
	m_updateType = 0;

	if (updateType <= 1)
	{
		/* Draw coordinate */
		updatePixmap1();
	}

	if (updateType <= 2)
	{
		/* Draw inactive EQ Curves */
		m_pixmap2 = m_pixmap1;
		updatePixmap2(&m_pixmap2);
	}

	if (updateType <= 3)
	{
		/* Draw Frequency Boxes */
		m_pixmap3 = m_pixmap2;
		updatePixmap3(&m_pixmap3);
	}

	if (updateType <= 4)
	{
		/* Draw current EQ Curve and total EQ Curve */
		m_pixmap4 = m_pixmap3;
		updatePixmap4(&m_pixmap4);
	}

	/* Draw Current EQ Curve's main control point */
	m_pixmap5 = m_pixmap4;
	updatePixmap5(&m_pixmap5);

	QStylePainter painter(this);
	painter.drawPixmap(0, 0, m_pixmap5);

#if 0
	/* Let's draw animantion of gain vertex for selected curve */
	EQData *pCurrentEQ = getCurrentEQ();
	int currentEQIndex = getCurrentEQIndex();
	EQType type = pCurrentEQ->getType();
	if (type != EQ_USER_DEFINED_FIR)
	{
		/* Animation the Vertex for the Gain */

		QColor color(qRgba(255, 255, 255, 255));
		int dragFreqX = f2x(pCurrentEQ->getFrequency());
		int dragFreqY;
		EQType type = pCurrentEQ->getType();
		if ((type == EQ_PEAKING) || (type == EQ_L_SHELF) || (type == EQ_H_SHELF))
		{
			dragFreqY = m2y(pCurrentEQ->getGain());
		}
		else
		{
			dragFreqY = m2y(0);		
		}

		QBrush oldBrush = painter.brush();
		drawAnimationVertex(&painter, dragFreqX, dragFreqY, getColor(currentEQIndex));
		painter.setPen(QPen(color, 2));
		painter.setBrush(oldBrush);
	}
	updateType = 0;
#endif
}

void EQWidget::drawAnimationVertex(QPainter *pPainter, int dragFreqX, int dragFreqY, QColor color)
{
#if 0
	if ((m_dragStatus == DRAGSTATUS_PRESSED) && (m_dragPoint == DRAG_POINT_GAIN))
	{
		return;
	}
#endif
	QColor color1(color);
	color1.setAlpha(0);

	QColor penColor = QColor(255, 255, 255);
	penColor.setAlpha(128);
	QRadialGradient radialGradient(dragFreqX,dragFreqY,m_animationRadius,dragFreqX,dragFreqY);  
	radialGradient.setColorAt(0, QColor(255, 255, 255));  
	radialGradient.setColorAt(0.4, color1);  
	radialGradient.setColorAt(1.0, QColor(255, 255, 0, 128));  
	radialGradient.setSpread(QGradient::PadSpread);  
	QBrush oldBrush = pPainter->brush();
	pPainter->setBrush(QBrush(radialGradient));  
	pPainter->setPen(QPen(penColor, 2));
	pPainter->drawEllipse(QPoint(dragFreqX, dragFreqY), m_animationRadius, m_animationRadius);
//	qDebug() << "drawAnimationVertex called with m_animationRadius = " << m_animationRadius;
}


/* x is the position related to the Widget */
double EQWidget::x2f(int x)
{
	double result = (qLn(MAX_EQ_FREQUENCY) - qLn(MIN_EQ_FREQUENCY)) * (double)(x - m_leftMargin) / (width() - m_leftMargin - m_rightMargin);
	result = exp(result + qLn(MIN_EQ_FREQUENCY));

	return (result);
}

double EQWidget::y2m(int y)
{
	double result = (height() - m_topMargin - m_bottomMargin) / 2 - (y - m_topMargin);
	result = result  * 2 * m_gainMax / (height() - m_topMargin - m_bottomMargin);

	return (result);
}


/* x is the position related to the Coordinate */
int EQWidget::f2x(double freq)
{
	double result = (qLn(freq) - qLn(MIN_EQ_FREQUENCY)) * (width() - m_leftMargin - m_rightMargin) / (qLn(MAX_EQ_FREQUENCY) - qLn(MIN_EQ_FREQUENCY)) + m_leftMargin;
	return (qRound(result));
}

int EQWidget::m2y(double mag)
{
	double result = (double)(height() - m_topMargin - m_bottomMargin) / 2 + (double)m_topMargin - ((double)(height() - m_topMargin - m_bottomMargin) * mag) / (2 * m_gainMax);
	return (qRound(result));
}

void EQWidget::clearFinalCurve()
{
	for (int i = 0; i <= m_discreteCount; i++)
	{
		m_pDimensionY[i] = 0;
	}
}

void EQWidget::addToTotalCurve(double *pVertical)
{
	for (int i = 0; i < m_discreteCount; i++)
	{
		m_pDimensionY[i] += pVertical[i];
	}
}

void EQWidget::resetDimensionSize()
{
	m_discreteCount = f2x(MAX_EQ_FREQUENCY) - f2x(MIN_EQ_FREQUENCY) + 1;
	if (m_discreteCount > 4096)
	{
		m_discreteCount = 4096;
	}
	
	double stepp = pow(1000.0,1.0 / m_discreteCount);

	m_pDimensionX[0] = MIN_EQ_FREQUENCY;
	for(int i = 1; i < m_discreteCount; i++)
	{
		m_pDimensionX[i] = stepp * m_pDimensionX[i - 1];
	}
}

void EQWidget::resizeEvent(QResizeEvent *)
{
	m_leftMargin = width() * 0.04;	//40
	m_topMargin = height() * 0.04;	//10;
	m_rightMargin = width() * 0.03;	//30;
	m_bottomMargin = height() * 0.15;	//20;

	resetDimensionSize();
	m_updateType = 0;

	m_vertexWidth = width() * 0.04;
	m_vertexHeight = height() * 0.06;

#if 0
	m_pBypassButton->setGeometry(m_leftMargin, buttonY, buttonWidth, buttonHeight);
	m_pDeleteAllButton->setGeometry(m_leftMargin + (buttonWidth + buttonInterval), buttonY, buttonWidth, buttonHeight);

	if (getTabType() == TAB_TYPE_MIC)
	{
		buttonWidth = 135;
	}

	m_pLREQLinkButton->setGeometry(width() - m_rightMargin - 2 * (buttonWidth + buttonInterval) - buttonWidth - 70, buttonY, buttonWidth + 70, buttonHeight);
	m_pLCHButton->setGeometry(width() - m_rightMargin - 1 * (buttonWidth + buttonInterval) - buttonWidth, buttonY, buttonWidth, buttonHeight);
	m_pRCHButton->setGeometry(width() - m_rightMargin - 0 * (buttonWidth + buttonInterval) - buttonWidth, buttonY, buttonWidth, buttonHeight);
#endif
}

void EQWidget::update(int updateType)
{
	if ((updateType < 1) || (updateType > 3))
	{
		return;
	}

	if (m_updateType == 0)
	{
		m_updateType = updateType; 
	}
	else if (m_updateType > updateType)
	{
		m_updateType = updateType;
	}

	QWidget::update();
}


void EQWidget::onAnimationTimer()
{
#if 0
	/* Let's update the EQ focus point */
	EQData *pCurrentEQ = getCurrentEQ();

	m_animationRadius++;
	if (m_animationRadius > ANIMATION_MAX_RADIUS / 2)
	{
		m_animationRadius = 2;
	}

	int dragFreqX = f2x(pCurrentEQ->getFrequency());
	int dragFreqY;
	
	EQType type = pCurrentEQ->getType();

	if (type == EQ_USER_DEFINED_FIR)
	{
		return;
	}

	if ((type == EQ_PEAKING) || (type == EQ_L_SHELF) || (type == EQ_H_SHELF))
	{
		dragFreqY = m2y(pCurrentEQ->getGain());
	}
	else
	{
		dragFreqY = m2y(0);
	}

//	if ((m_updateType != 0) || ((m_dragStatus == DRAGSTATUS_PRESSED) && (m_dragPoint == DRAG_POINT_GAIN)))
	if (m_updateType != 0)
	{
		return;
	}
	else
	{
		m_updateType = 3;
		repaint(dragFreqX - ANIMATION_MAX_RADIUS / 2 - 1, dragFreqY - ANIMATION_MAX_RADIUS / 2 - 1, ANIMATION_MAX_RADIUS + 2, ANIMATION_MAX_RADIUS + 2);
	}
#endif
}

void EQWidget::mousePressEvent(QMouseEvent *event)
{
	if (event->buttons() == Qt::LeftButton)
	{
		dispatchDragEvent(DRAGEVENT_MOUSEPRESSED, event);
	}
	else if (event->buttons() == Qt::RightButton)
	{
		
	}
	else
	{
	}
}

void EQWidget::wheelEvent(QWheelEvent *event)
{
#if 0
	int numDegrees = event->delta() / 8;
	int numSteps = numDegrees / 15;

	if (event->orientation() == Qt::Vertical) 
	{
		setFocus();
		EQData *pCurrentEQ = getCurrentEQ();
		{
			double gain = pCurrentEQ->getGain();
			gain += (0.1 * numSteps);
			if (gain >= m_gainMax)
			{
				gain = m_gainMax;
			}
			if (gain <= -m_gainMax)
			{
				gain = -m_gainMax;
			}

			pCurrentEQ->setGain(gain);
			update(2);
			emit currentEQChanged();
		}
	}
#endif
}

void EQWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
	if (event->buttons() == Qt::LeftButton)
	{
		dispatchDragEvent(DRAGEVENT_MOUSEDBLCLICKED, event);
	}
	else if (event->buttons() == Qt::RightButton)
	{
	
	}
	else
	{
	}
}

void EQWidget::mouseMoveEvent(QMouseEvent *event)
{
	static int oldX = 0, oldY = 0;
	if ((event->x() == oldX) && (event->y() == oldY))
	{
		return;
	}
	oldX = event->x();
	oldY = event->y();
	dispatchDragEvent(DRAGEVENT_MOUSEMOVE, event);
}

void EQWidget::mouseReleaseEvent(QMouseEvent *event)
{
	dispatchDragEvent(DRAGEVENT_MOUSERELEASED, event);
}

void EQWidget::keyPressEvent(QKeyEvent *event)
{
#if 0
	EQData *pCurrentEQ = getCurrentEQ();
	int currentPEQIndex = getCurrentEQIndex();

    switch (event->key())
    {
        case Qt::Key_Up:
//			if (currentEQIndex == currentEQButtonIndex)
			{
				double gain = pCurrentEQ->getGain();
				gain += 0.1;
				if (gain >= m_gainMax)
				{
					gain = m_gainMax;
				}

//				onEQGainChanged(currentEQIndex, gain);
				pCurrentEQ->setGain(gain);
			}
			update(2);
			emit currentEQChanged();
			break;

        case Qt::Key_Down:
//			if (currentEQIndex == currentEQButtonIndex)
			{
				double gain = pCurrentEQ->getGain();
				gain -= 0.1;
				if (gain <= -m_gainMax)
				{
					gain = -m_gainMax;
				}

//				onEQGainChanged(currentEQIndex, gain);
				pCurrentEQ->setGain(gain);
			}
			update(2);
			emit currentEQChanged();
			break;
		case Qt::Key_Left:
//			if (currentEQIndex == currentEQButtonIndex)
			{
				double freq = pCurrentEQ->getFrequency();
				int x = f2x(freq);
				freq = x2f(x - 1);
				if (freq <= MIN_EQ_FREQUENCY)
				{
					freq = MIN_EQ_FREQUENCY;
				}

				pCurrentEQ->setFrequency(freq);
			}

			update(2);
			emit currentEQChanged();
			break;

		case Qt::Key_Right:
//			if (currentEQIndex == currentEQButtonIndex)
			{
				double freq = pCurrentEQ->getFrequency();
				int x = f2x(freq);
				freq = x2f(x + 1);
				if (freq >= MAX_EQ_FREQUENCY)
				{
					freq = MAX_EQ_FREQUENCY;
				}

				pCurrentEQ->setFrequency(freq);
			}
			
			update(2);
			emit currentEQChanged();
			break;

		case Qt::Key_Tab:
		case Qt::Key_QuoteLeft: // Key for '~'
//			for (;;)
			{
				if (currentPEQIndex == 0)
				{
					currentPEQIndex = 2;
				}
				else if (currentPEQIndex == 4)
				{
					currentPEQIndex = 1;
				}
				else if (currentPEQIndex == 1)
				{
					currentPEQIndex = 0;
				}
				else
				{
					currentPEQIndex++;
				}
			}

			emit currentEQIndexChanged(currentPEQIndex);
			update(1);
			break;

/*
		case Qt::Key_Delete:
			if (currentEQIndex == currentEQButtonIndex)
			{
				onDeleteClicked();
			}
			break;
*/
        default:
//			qDebug() << "Press: " << event->key();
            QWidget::keyPressEvent(event);
    }
	event->accept();
#endif
}

void EQWidget::dispatchDragEvent(DragEvent dragEvent, QMouseEvent *event)
{
#if 0
	static int centerX = 0, centerY = 0, mousePointX = 0, mousePointY = 0;

	EQData *pCurrentEQ = getCurrentEQ();
	int currentPEQIndex = getCurrentEQIndex();

	switch (m_dragStatus)
	{
	case DRAGSTATUS_IDLE:

		switch (dragEvent)
		{
#if 1
			case DRAGEVENT_MOUSEDBLCLICKED:
			{

				/* if the user double click the current curve, then we need to disable/enable the EQ Curve */
				int xPos = event->x();
				int yPos = event->y();
				double freq = x2f(xPos);
				double mag = y2m(yPos);
				if (pCurrentEQ->isInclude(freq, mag) == true)
				{
					bool bFlag = pCurrentEQ->getEnableFlag();
					pCurrentEQ->enable(!bFlag);
//					onEQCurveEnableChanged(currentEQButtonIndex, bFlag);
					update(2);
					emit currentEQChanged();
					break;
				}

				break;
			}
#endif
#if 1  // Mark = 1.1
			case DRAGEVENT_MOUSEMOVE:
			{
				/* First Let's check the mouse is in the draggable Gain & Frequency circle*/
				int freqX = f2x(pCurrentEQ->getFrequency());
				int freqY = m2y(pCurrentEQ->getGain());
				if ((pCurrentEQ->getType() == EQ_LPF) || (pCurrentEQ->getType() == EQ_HPF))
				{
					freqY = m2y(0);
				}

				int radius = (freqX - event->x()) * (freqX - event->x()) + (freqY - event->y()) * (freqY - event->y());

				if (radius <= (3 * m_radiusFreqPoint) * (3 * m_radiusFreqPoint))
				{
					setCursor(QCursor(Qt::SizeAllCursor));
					break;
				}

				if (pCurrentEQ->getType() == EQ_PEAKING)
				{
					/* Then let's check if the mouse is in the left draggable Q square */
					double leftPointFreq = pCurrentEQ->getFreqForQ(0);
					int leftPointX = f2x(leftPointFreq);
					int leftPointY = m2y(pCurrentEQ->dBCalc(leftPointFreq));
					if ((event->x() >= (leftPointX - m_sqareQLen)) && (event->x() <= (leftPointX + m_sqareQLen))
						&& (event->y() >= (leftPointY - m_sqareQLen)) && (event->y() <= (leftPointY + m_sqareQLen)))
					{
						setCursor(QCursor(Qt::SizeAllCursor));
						break;					
					}

					/* Then let's check if the mouse is in the right draggable Q square */
					double rightPointFreq = pCurrentEQ->getFreqForQ(1);
					int rightPointX = f2x(rightPointFreq);
					int rightPointY = m2y(pCurrentEQ->dBCalc(rightPointFreq));
					if ((event->x() >= (rightPointX - m_sqareQLen)) && (event->x() <= (rightPointX + m_sqareQLen))
						&& (event->y() >= (rightPointY - m_sqareQLen)) && (event->y() <= (rightPointY + m_sqareQLen)))
					{
						setCursor(QCursor(Qt::SizeAllCursor));
						break;					
					}
				}
				setCursor(QCursor(Qt::ArrowCursor));
				break;
			}
#endif // Mark = 1.1
			case DRAGEVENT_MOUSEPRESSED:
			{
#if 1  // Mark = 0
				this->setFocus();

				/* First of all, let's check if the mouse is in the draggable point area */
//				for (index = 0; index < getEQCount(); index++)
				{
					/* First let's judge if the draggable left Q Point is selected */
					if (pCurrentEQ->getType() == EQ_PEAKING)
					{
						double leftPointFreq = pCurrentEQ->getFreqForQ(0);
						int leftPointX = f2x(leftPointFreq);
						int leftPointY = m2y(pCurrentEQ->dBCalc(leftPointFreq));
						if ((event->x() >= (leftPointX - m_sqareQLen)) && (event->x() <= (leftPointX + m_sqareQLen))
							&& (event->y() >= (leftPointY - m_sqareQLen)) && (event->y() <= (leftPointY + m_sqareQLen)))
						{
							centerX = leftPointX;
							centerY = leftPointY;
							mousePointX = event->x();
							mousePointY = event->y();
							setMouseTracking(false);
							m_dragStatus = DRAGSTATUS_PRESSED;
							m_dragPoint = DRAG_POINT_LEFT_Q;
							return;
						}

						double rightPointFreq = pCurrentEQ->getFreqForQ(1);
						int rightPointX = f2x(rightPointFreq);
						int rightPointY = m2y(pCurrentEQ->dBCalc(rightPointFreq));
						if ((event->x() >= (rightPointX - m_sqareQLen)) && (event->x() <= (rightPointX + m_sqareQLen))
							&& (event->y() >= (rightPointY - m_sqareQLen)) && (event->y() <= (rightPointY + m_sqareQLen)))
						{
							centerX = rightPointX;
							centerY = rightPointY;
							mousePointX = event->x();
							mousePointY = event->y();
							setMouseTracking(false);
							m_dragStatus = DRAGSTATUS_PRESSED;
							m_dragPoint = DRAG_POINT_RIGHT_Q;
							return;
						}
					}

					/* Then Let's judge if the draggable Gain Point of the current OutputChannel is being dragged */
					int freqX = f2x(pCurrentEQ->getFrequency());
					int freqY = m2y(pCurrentEQ->getGain());
					if ((pCurrentEQ->getType() == EQ_HPF) || (pCurrentEQ->getType() == EQ_LPF))
					{
						freqY = m2y(0);
					}

					int radius = (freqX - event->x()) * (freqX - event->x()) + (freqY - event->y()) * (freqY - event->y());

					if (radius <= m_radiusFreqPoint * m_radiusFreqPoint * 9)
					{
						centerX = freqX;
						centerY = freqY;
						mousePointX = event->x();
						mousePointY = event->y();
						setMouseTracking(false);
						m_dragStatus = DRAGSTATUS_PRESSED;
						m_dragPoint = DRAG_POINT_GAIN;
						return;
					}
				}

				int xPos = event->x();
				int yPos = event->y();
				double freq = x2f(xPos);
				double mag = y2m(yPos);

#if 0
				if (pCurrentEQ->isInclude(freq, mag) == true)
				{
					return;
				}
#endif


				/* We need to judge if another EQ Curve is selected by clicking the vertex Text */
				{
//					bool selectedFlag = false;
					for (int i = 0; i < getEQCount(); i++)
					{
						QRect rect;

						EQData *pEQData = getEQ(i);
						int eqType = pEQData->getType();

						if ((eqType == EQ_PEAKING) || (eqType == EQ_L_SHELF) || (eqType == EQ_H_SHELF))
						{
							int dragFreqX = f2x(pEQData->getFrequency());
							int dragFreqY = m2y(pEQData->getGain());;
							QString text;
							text.setNum(i + 1);
							if (pEQData->getGain() > 0)
							{
								if (dragFreqY - 25 > m2y(m_gainMax))
								{
									rect = QRect(dragFreqX - 15, dragFreqY - 30, 30, 20);
								}
								else
								{
									rect = QRect(dragFreqX - 15, dragFreqY + 5, 30, 20);
								}
							}
							else
							{
								if (dragFreqY + 25 < m2y(-m_gainMax))
								{
									rect = QRect(dragFreqX - 15, dragFreqY + 10, 30, 20);
								}
								else
								{
									rect = QRect(dragFreqX - 15, dragFreqY - 30, 30, 20);
								}
							}

							if (rect.contains(xPos, yPos) && (currentPEQIndex != i))
							{
								emit currentEQIndexChanged(i);
								update(1);
								return;
							}
						}
					}
				}

				/* We need to judge if another EQ curve is selected by clicking the points in the curve */
				for (int i = 0; i < getEQCount(); i++)
				{
					if (i != currentPEQIndex)
					{
						EQData *pEQData = getEQ(i);
						if ((pEQData->isInclude(freq, mag) == true) &&  (currentPEQIndex != i))
						{
							emit currentEQIndexChanged(i);
							update(1);
							return;
						}
					}
				}


#endif  // Mark = 0
				break;
			}

			default:
				break;

		}

		break;
#if 1  //Mark = 1
	case DRAGSTATUS_PRESSED:
	{
		switch (dragEvent)
		{
			case DRAGEVENT_MOUSEMOVE:
			{
				int newCenterX = centerX - mousePointX + event->x();
				int newCenterY = centerY - mousePointY + event->y();
				
				switch (m_dragPoint)
				{
					case DRAG_POINT_GAIN:
					{
						if ((pCurrentEQ->getType() == EQ_HPF) || (pCurrentEQ->getType() == EQ_LPF))
						{
							double freq = x2f(newCenterX);
							emit setPFFreq(freq);							
						}
						else
						{
							// Gain for PEQ
							double freq = x2f(newCenterX);
							double gain = y2m(newCenterY);
							if (freq < MIN_EQ_FREQUENCY)
							{
								freq = MIN_EQ_FREQUENCY;
							}
							if (freq > MAX_EQ_FREQUENCY)
							{
								freq = MAX_EQ_FREQUENCY;
							}

							if (gain < -m_gainMax)
							{
								gain = -m_gainMax;
							}
							if (gain > m_gainMax)
							{
								gain = m_gainMax;
							}

							pCurrentEQ->setFrequency(freq);
							pCurrentEQ->setGain(gain);
						}

						update(2);
						emit currentEQChanged();

						break;
					}
					case DRAG_POINT_LEFT_Q:
					{
						double q;
						if (newCenterX >= f2x(pCurrentEQ->getFrequency()))
						{
							q = MAX_Q;
						}
						else
						{
							double leftFreq = x2f(newCenterX);
							if (leftFreq < MIN_EQ_FREQUENCY)
							{
								leftFreq = MIN_EQ_FREQUENCY;
							}
							q = pCurrentEQ->calcLeftQ(leftFreq);
							if (q < MIN_Q)
							{
								q = MIN_Q;
							}
							else if (q > MAX_Q)
							{
								q = MAX_Q;
							}
						}
						pCurrentEQ->setQ(q);
//						onEQCurveQChanged(currentEQButtonIndex, q);
						update(2);
						emit currentEQChanged();
//						updateQBoxforFilterWidget();
						break;
					}
					case DRAG_POINT_RIGHT_Q:
					{
						double q;
						if (newCenterX <= f2x(pCurrentEQ->getFrequency()))
						{
							q = MAX_Q;
						}
						else
						{
							double rightFreq = x2f(newCenterX);
							if (rightFreq > MAX_EQ_FREQUENCY)
							{
								rightFreq = MAX_EQ_FREQUENCY;
							}
							q = pCurrentEQ->calcRightQ(rightFreq);
//							qDebug() << "RightQ = " << rightFreq << ", " << q ;
							if (q < MIN_Q)
							{
								q = MIN_Q;
							}
							else if (q > MAX_Q)
							{
								q = MAX_Q;
							}
						}
						pCurrentEQ->setQ(q);
//						onEQCurveQChanged(currentEQButtonIndex, q);
						update(2);
						emit currentEQChanged();
						break;
					}
					default:
						Q_ASSERT(false);
				}

				update(2);
				break;
			}
			case DRAGEVENT_MOUSERELEASED:
			{
				dispatchDragEvent(DRAGEVENT_MOUSEMOVE, event);
				setMouseTracking(true);
				m_dragStatus = DRAGSTATUS_IDLE;
				QApplication::postEvent(this, new QMouseEvent(*event));
				break;
			}
			default:
				break;
		}
		break;	
	}
#endif  // Mark = 1
	default:
		break;
	}
#endif
}

QColor EQWidget::getColor(int currentEQIndex)
{
	QColor color[17] = {
		QColor(244, 122, 122), QColor(24, 224, 24), QColor(224, 24, 24), 
		QColor(24, 224, 224), QColor(224, 224, 24), QColor(224, 24, 224), 
		QColor(24, 124, 224), QColor(24, 224, 124), QColor(124, 24, 224), 
		QColor(124, 224, 24), QColor(224, 24, 124), QColor(224, 124, 24), 
		QColor(124, 124, 224), QColor(124, 224, 124), QColor(224, 124, 124),
		QColor(124, 244, 244), QColor(24, 24, 224)}; 

	int count = sizeof(color) / sizeof(QColor);
	int index = currentEQIndex % count;

	color[index].setAlpha(60);

	return (color[index]);
}
