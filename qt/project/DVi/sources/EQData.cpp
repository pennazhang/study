#include "EQData.h"
#include "simpleQtLogger.h"
#include <QDebug>
#include "commonLib.h"
#include "FormatOutput.h"
#include "EQWidget.h"
#include "plotBiquad.h"

/* calc curve for PEQ: Peaking / Low-Shelf / High Shelf */
void curve_calculation(int eq_type, double eq_gain, double eq_fc, double q_or_slope, int horizontal_number, double *horizontal, double *vertical)
{
	g_plotBiquad.setParameter(true, eq_type, eq_fc, eq_gain, q_or_slope);
	g_plotBiquad.plotCurve(horizontal, vertical, horizontal_number);
}

EQData::EQData()
{
	m_enableFlag = false;
}

void EQData::enable(bool validFlag)
{
	m_enableFlag = validFlag;
}

bool EQData::isEnabled()
{
	return (m_enableFlag);
}

void EQData::dump()
{
	FormatOutput::dump(QString("EnableFlag = %1").arg(QString::number(m_enableFlag)));
}

void EQData::drawVertexText(QPainter *painter, EQWidget *pEQWidget, QString &vertexText)
{

}


void FIREQ::dump()
{
    FormatOutput::dump("EQ Type = FIR");
    EQData::dump();
    FormatOutput::dump(QString("Tap = %1").arg(m_numberTap));
    FormatOutput::dump(QString("content = %1").arg(m_content));
}

void FIREQ::drawEQCurve(QPainter *painter, EQWidget *pEQWidget, int pixelCount, double *pGain, QColor &fillColor)
{

}

void FIREQ::dBCalc(int discreteCount, double *freq, double *gain)
{
	for (int i = 0; i < discreteCount; i++)
	{
		gain[i] = -3;
	}
}

BellEQ::BellEQ(double frequency, double gain, double q, bool enableFlag)
{
	m_frequency = frequency;
	m_gain = gain;
	m_Q = q;
	m_enableFlag = enableFlag;
}

void BellEQ::dump()
{
    FormatOutput::dump("EQ Type = Bell");
    EQData::dump();
    FormatOutput::dump(QString("Frequency = %1").arg(m_frequency));
    FormatOutput::dump(QString("Gain = %1").arg(m_gain));
    FormatOutput::dump(QString("Q = %1").arg(m_Q));
}

void BellEQ::drawEQCurve(QPainter *pPainter, EQWidget *pEQWidget, int pixelCount, double *pGain, QColor &fillColor)
{
	int startIndex = 0, endIndex = pixelCount;
	int startX = pEQWidget->f2x(MIN_EQ_FREQUENCY);
	int m0 = pEQWidget->m2y(0); 
	int i;
	QPainterPath path;

	if ((m_gain < 0.01) && (m_gain > -0.01))
	{
		return;
	}

	/* First Let's find the startPoint and endPoint */
	if (m_gain > 0)
	{
		for (i = 1; i < pixelCount; i++)
		{
			if (pEQWidget->m2y(pGain[i]) < m0)
			{
				break;
			}
		}
		startIndex = i;

		for (i = pixelCount - 1; i> 0; i--)
		{
			if (pEQWidget->m2y(pGain[i]) < m0)
			{
				break;
			}		
		}
		endIndex = i;

		if (startIndex >= endIndex)
		{
			return;
		}
		path.moveTo(startX + startIndex, m0);
		for (i = startIndex; i <= endIndex; i++)
		{
			path.lineTo(startX + i, pEQWidget->m2y(pGain[i]));
		}
		path.lineTo(startX + endIndex, m0);
		path.closeSubpath();		
	}
	else
	{
		for (i = 1; i < pixelCount; i++)
		{
			if (pEQWidget->m2y(pGain[i]) > m0)
			{
				break;
			}
		}
		startIndex = i;

		for (i = pixelCount - 1; i> 0; i--)
		{
			if (pEQWidget->m2y(pGain[i]) > m0)
			{
				break;
			}		
		}
		endIndex = i;	

		if (startIndex >= endIndex)
		{
			return;
		}
		path.moveTo(startX + startIndex, m0);
		for (i = startIndex; i <= endIndex; i++)
		{
			path.lineTo(startX + i, pEQWidget->m2y(pGain[i]));
		}
		path.lineTo(startX + endIndex, m0);
		path.closeSubpath();	
	}
	pPainter->fillPath(path, QBrush(fillColor));
}

void BellEQ::dBCalc(int pointNum, double *freq, double *gain)
{
	curve_calculation(EQ_PEAKING, m_gain, m_frequency, m_Q, pointNum, freq, gain);
}

int BellEQ::getControlPointCount()
{
	if ((m_gain < 0.01) && (m_gain > -0.01))
	{
		return (1);
	}
	else
	{
		return (3);
	}
}

double BellEQ::getLeftFreqForControlPoint()
{
	double ratio = (sqrt((qreal)(4 * m_Q * m_Q + 1)) - 1 ) / ( 2 * m_Q);
	return (m_frequency * ratio);			
}

double BellEQ::getRightFreqForControlPoint()
{
	double ratio = (sqrt((qreal)(4 * m_Q * m_Q + 1)) - 1 ) / ( 2 * m_Q);
	return (m_frequency / ratio);
}

bool BellEQ::hasMainControlPoint()
{
	return (true);
}

void BellEQ::drawVertexText(QPainter *painter, EQWidget *pEQWidget, QString &vertexText)
{
	ControlPoint point = getControlPoint(0);
	
	QSize vertexSize = pEQWidget->getVertexSize();
	double maxGain = pEQWidget->getMaxGain();

	int dragFreqX = pEQWidget->f2x(point.m_frequency);
	int dragFreqY = pEQWidget->m2y(point.m_gain);

	QRect textRect;
	if (m_gain >= 0)
	{
		if (dragFreqY - vertexSize.height() > pEQWidget->m2y(maxGain))
		{
			textRect = QRect(dragFreqX - vertexSize.width() / 2, dragFreqY - vertexSize.height(), vertexSize.width(), vertexSize.height());
		}
		else
		{
			textRect = QRect(dragFreqX - vertexSize.width() / 2, dragFreqY, vertexSize.width(), vertexSize.height());
		}
	}
	else
	{
		if (dragFreqY + vertexSize.height() < pEQWidget->m2y(-maxGain))
		{
			textRect = QRect(dragFreqX - vertexSize.width() / 2, dragFreqY, vertexSize.width(), vertexSize.height());
		}
		else
		{
			textRect = QRect(dragFreqX - vertexSize.width() / 2, dragFreqY - vertexSize.height(), vertexSize.width(), vertexSize.height());
		}
	}

	painter->drawRect(textRect);
	painter->drawText(textRect, Qt::AlignCenter, vertexText);
}

ControlPoint BellEQ::getControlPoint(int index) 
{ 
	double freq;
	double gain;

	if (index == 0)
	{
		/* Main Control Point for gain */	
		freq = m_frequency;

	}
	else
	{
		/* Control Point for Q */
		if (index == 1)
		{
			freq = getLeftFreqForControlPoint();
		}
		else if (index == 2)
		{
			freq = getRightFreqForControlPoint();
		}
		else
		{
			L_ERROR(QString("BellEQ::getControlPoint error for index = %d").arg(index));
			freq = m_frequency;
		}
	}
	curve_calculation(EQ_PEAKING, m_gain, m_frequency, m_Q, 1, &freq, &gain);

	return (ControlPoint(freq, gain));
}

void LShelfEQ::dump()
{
    FormatOutput::dump("EQ Type = LShelf");
    EQData::dump();
    FormatOutput::dump(QString("Frequency = %1").arg(m_lsFrequency));
    FormatOutput::dump(QString("Gain = %1").arg(m_gain));
    FormatOutput::dump(QString("Slope = %1").arg(m_slope));
}

void LShelfEQ::drawEQCurve(QPainter *painter, EQWidget *pEQWidget, int pixelCount, double *pGain, QColor &fillColor)
{

}

void LShelfEQ::dBCalc(int discreteCount, double *freq, double *gain)
{
	for (int i = 0; i < discreteCount; i++)
	{
		gain[i] = -3;
	}
}

void HShelfEQ::dump()
{
    FormatOutput::dump("EQ Type = HShelf");
    EQData::dump();
    FormatOutput::dump(QString("Frequency = %1").arg(m_hsfrequency));
    FormatOutput::dump(QString("Gain = %1").arg(m_gain));
    FormatOutput::dump(QString("Slope = %1").arg(m_slope));
}

void HShelfEQ::drawEQCurve(QPainter *painter, EQWidget *pEQWidget, int pixelCount, double *pGain, QColor &fillColor)
{

}

void HShelfEQ::dBCalc(int discreteCount, double *freq, double *gain)
{
	for (int i = 0; i < discreteCount; i++)
	{
		gain[i] = -3;
	}
}

void HPFEQ::dump()
{
    FormatOutput::dump("EQ Type = High Path Filter");
    EQData::dump();
    FormatOutput::dump(QString("Frequency = %1").arg(m_hpFrequency));
    FormatOutput::dump(QString("Gain = %1").arg(m_gain));
}

void HPFEQ::drawEQCurve(QPainter *painter, EQWidget *pEQWidget, int pixelCount, double *pGain, QColor &fillColor)
{
	
}

void HPFEQ::dBCalc(int discreteCount, double *freq, double *gain)
{
	for (int i = 0; i < discreteCount; i++)
	{
		gain[i] = -3;
	}
}

void LPFEQ::dump()
{
    FormatOutput::dump("EQ Type = Low Path Filter");
    EQData::dump();
    FormatOutput::dump(QString("Frequency = %1").arg(m_lpFrequency));
    FormatOutput::dump(QString("Gain = %1").arg(m_gain));
}

void LPFEQ::drawEQCurve(QPainter *painter, EQWidget *pEQWidget, int pixelCount, double *pGain, QColor &fillColor)
{

}

void LPFEQ::dBCalc(int discreteCount, double *freq, double *gain)
{
	for (int i = 0; i < discreteCount; i++)
	{
		gain[i] = -3;
	}
}

void XOver::dump()
{
    FormatOutput::dump("EQ Type = XOver");
    EQData::dump();
    FormatOutput::dump(QString("Low Path Frequency = %1").arg(m_lpFrequency));
    FormatOutput::dump(QString("High Path Frequency = %1").arg(m_hpFrequency));
    FormatOutput::dump(QString("Gain = %1").arg(m_gain));
}

void XOver::drawEQCurve(QPainter *painter, EQWidget *pEQWidget, int pixelCount, double *pGain, QColor &fillColor)
{

}

void XOver::dBCalc(int discreteCount, double *freq, double *gain)
{
	for (int i = 0; i < discreteCount; i++)
	{
		gain[i] = -3;
	}
}

EQGroup::EQGroup()
{
	m_EQCount = 0;
}

EQGroup::EQGroup(int count)
{
	int i;

	m_EQCount = count;
	for (i = 0; i < m_EQCount; i++)
	{
		m_pEQDataArray[i] = nullptr;
	}
}

EQGroup::~EQGroup()
{
	resetEQCount(0);
}

void EQGroup::resetEQCount(int count)
{
	int i;
	for (i = 0; i < m_EQCount; i++)
	{
		EQData *pEQData = m_pEQDataArray[i];
		delete pEQData;
		m_pEQDataArray[i] = nullptr;
	}

	m_EQCount = count;
	for (i = 0; i < m_EQCount; i++)
	{
		m_pEQDataArray[i] = nullptr;
	}
}

EQData *EQGroup::getEQ(int index)
{
	if ((m_EQCount > index) && (index >= 0))
	{
		return m_pEQDataArray[index];
	}
	else
	{
		return nullptr;
	}
}

void EQGroup::setEQ(int index, FIREQ EQ)
{
	FIREQ *pNewEQ;
	if (m_pEQDataArray[index] == nullptr) 
	{
		pNewEQ = new FIREQ(EQ);
		m_pEQDataArray[index] = pNewEQ; 
	}
	else
	{
		if (m_pEQDataArray[index]->getEQType() == EQ_USER_DEFINED_FIR)
		{
			pNewEQ = (FIREQ *)m_pEQDataArray[index];
			*pNewEQ = EQ;
		}
		else
		{
			delete m_pEQDataArray[index];
			pNewEQ = new FIREQ(EQ);
			m_pEQDataArray[index] = pNewEQ; 
		}
	}
}

void EQGroup::setEQ(int index, BellEQ EQ)
{
	if (index >= m_EQCount)
	{
		L_ERROR(QString("Internal Error: Invalid index:%1 while total EQCount = %2").arg(index).arg(m_EQCount));
		return;
	}

	BellEQ *pNewEQ;
	if (m_pEQDataArray[index] == nullptr) 
	{
		pNewEQ = new BellEQ(EQ);
		m_pEQDataArray[index] = pNewEQ; 
	}
	else
	{
		if (m_pEQDataArray[index]->getEQType() == EQ_USER_DEFINED_FIR)
		{
			pNewEQ = (BellEQ *)m_pEQDataArray[index];
			*pNewEQ = EQ;
		}
		else
		{
			delete m_pEQDataArray[index];
			pNewEQ = new BellEQ(EQ);
			m_pEQDataArray[index] = pNewEQ; 
		}
	}
}

void EQGroup::setEQ(int index, LShelfEQ EQ)
{
	if (index >= m_EQCount)
	{
		L_ERROR(QString("Internal Error: Invalid index:%1 while total EQCount = %2").arg(index).arg(m_EQCount));
		return;
	}

	LShelfEQ *pNewEQ;
	if (m_pEQDataArray[index] == nullptr) 
	{
		pNewEQ = new LShelfEQ(EQ);
		m_pEQDataArray[index] = pNewEQ; 
	}
	else
	{
		if (m_pEQDataArray[index]->getEQType() == EQ_USER_DEFINED_FIR)
		{
			pNewEQ = (LShelfEQ *)m_pEQDataArray[index];
			*pNewEQ = EQ;
		}
		else
		{
			delete m_pEQDataArray[index];
			pNewEQ = new LShelfEQ(EQ);
			m_pEQDataArray[index] = pNewEQ; 
		}
	}
}

void EQGroup::setEQ(int index, HShelfEQ EQ)
{
	if (index >= m_EQCount)
	{
		L_ERROR(QString("Internal Error: Invalid index:%1 while total EQCount = %2").arg(index).arg(m_EQCount));
		return;
	}

	HShelfEQ *pNewEQ;
	if (m_pEQDataArray[index] == nullptr) 
	{
		pNewEQ = new HShelfEQ(EQ);
		m_pEQDataArray[index] = pNewEQ; 
	}
	else
	{
		if (m_pEQDataArray[index]->getEQType() == EQ_USER_DEFINED_FIR)
		{
			pNewEQ = (HShelfEQ *)m_pEQDataArray[index];
			*pNewEQ = EQ;
		}
		else
		{
			delete m_pEQDataArray[index];
			pNewEQ = new HShelfEQ(EQ);
			m_pEQDataArray[index] = pNewEQ; 
		}
	}
}

void EQGroup::setEQ(int index, LPFEQ EQ)
{
	if (index >= m_EQCount)
	{
		L_ERROR(QString("Internal Error: Invalid index:%1 while total EQCount = %2").arg(index).arg(m_EQCount));
		return;
	}

	LPFEQ *pNewEQ;
	if (m_pEQDataArray[index] == nullptr) 
	{
		pNewEQ = new LPFEQ(EQ);
		m_pEQDataArray[index] = pNewEQ; 
	}
	else
	{
		if (m_pEQDataArray[index]->getEQType() == EQ_USER_DEFINED_FIR)
		{
			pNewEQ = (LPFEQ *)m_pEQDataArray[index];
			*pNewEQ = EQ;
		}
		else
		{
			delete m_pEQDataArray[index];
			pNewEQ = new LPFEQ(EQ);
			m_pEQDataArray[index] = pNewEQ; 
		}
	}
}

void EQGroup::setEQ(int index, HPFEQ EQ)
{
	if (index >= m_EQCount)
	{
		L_ERROR(QString("Internal Error: Invalid index:%1 while total EQCount = %2").arg(index).arg(m_EQCount));
		return;
	}

	HPFEQ *pNewEQ;
	if (m_pEQDataArray[index] == nullptr) 
	{
		pNewEQ = new HPFEQ(EQ);
		m_pEQDataArray[index] = pNewEQ; 
	}
	else
	{
		if (m_pEQDataArray[index]->getEQType() == EQ_USER_DEFINED_FIR)
		{
			pNewEQ = (HPFEQ *)m_pEQDataArray[index];
			*pNewEQ = EQ;
		}
		else
		{
			delete m_pEQDataArray[index];
			pNewEQ = new HPFEQ(EQ);
			m_pEQDataArray[index] = pNewEQ; 
		}
	}
}

void EQGroup::setEQ(int index, XOver EQ)
{
	if (index >= m_EQCount)
	{
		L_ERROR(QString("Internal Error: Invalid index:%1 while total EQCount = %2").arg(index).arg(m_EQCount));
		return;
	}

	XOver *pNewEQ;
	if (m_pEQDataArray[index] == nullptr) 
	{
		pNewEQ = new XOver(EQ);
		m_pEQDataArray[index] = pNewEQ; 
	}
	else
	{
		if (m_pEQDataArray[index]->getEQType() == EQ_USER_DEFINED_FIR)
		{
			pNewEQ = (XOver *)m_pEQDataArray[index];
			*pNewEQ = EQ;
		}
		else
		{
			delete m_pEQDataArray[index];
			pNewEQ = new XOver(EQ);
			m_pEQDataArray[index] = pNewEQ; 
		}
	}
}

void EQGroup::dump()
{
	JSON_KEY_STRING aDumpNode[] = 
	{
		{"Enable Flag",				QString::number(m_enableAll)	},
		{"EQ Count",				QString::number(m_EQCount)	},
	};

    for (ULONG i = 0; i < sizeof(aDumpNode) / sizeof(JSON_KEY_STRING); i++)
	{
		FormatOutput::dump(QString("%1 = %2").arg(aDumpNode[i].m_strKeyList).arg(aDumpNode[i].m_valueString));
	}

	for (int i = 0; i < m_EQCount; i++)
	{
		FormatOutput::dump(QString("EQ Index:%1").arg(i));
		FormatOutput::incDumpLevel();
        m_pEQDataArray[i]->dump();
		FormatOutput::decDumpLevel();
	}
}
