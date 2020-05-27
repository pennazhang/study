#include "ButtonSlider.h"
#include <QStylePainter>
#include <QKeyEvent>

ButtonSlider::ButtonSlider(QWidget *pParent, Qt::Orientation orientation)
    : QSlider(orientation, pParent)
{
	m_upperValue = 100;
	m_lowerValue = 0;
	m_longMarkCount = 0;
	m_shotLongMarkRatio = 0;
	m_textList<< "0 CM" << "25 CM" << "50 CM" << "75 CM" << "100 CM";
	if (orientation == Qt::Vertical)
	{
	    setObjectName(QStringLiteral("ButtonVerticalSlider"));
	}
	else
	{
	    setObjectName(QStringLiteral("ButtonHorizontalSlider"));
	}

	m_extraLongMarkCount = 0;
	m_extraShortMarkCount = 0;

	m_leftMark = false;
	m_rightMark = true;

	m_shortMarkLength = 3; 
	m_longMarkLength = 5;
	m_GrooveHeight = 20;

	m_markColor = QColor(0x10, 0x10, 0x10);
	m_textColor = QColor(0x10, 0x10, 0x10);
}

void ButtonSlider::keyPressEvent(QKeyEvent *event)
{
    switch (event->key())
    {
        case Qt::Key_Left:
		case Qt::Key_Right:
			if (orientation() == Qt::Vertical)
			{
				event->ignore();
				return;
			}
	}
	QSlider::keyPressEvent(event);
}

void ButtonSlider::setMarkLength(int shortMarkLength, int longMarkLength)
{
	m_shortMarkLength = shortMarkLength; 
	m_longMarkLength = longMarkLength;
}

void ButtonSlider::setMarkSide(bool leftMark, bool rightMark)
{
	m_leftMark = leftMark;
	m_rightMark = rightMark;
}

/*
	longMarkCount: the count of Long Mark, Normally 0 means no long mark, 2 means the long mark located at begin and end.
	shortLongMarkRatio: how many short area between the 2 neighbour long marks.
*/
void ButtonSlider::setMark(int longMarkCount, int shortLongMarkRatio, QStringList &textList)
{
	m_longMarkCount = longMarkCount;
	m_shotLongMarkRatio = shortLongMarkRatio;
	m_textList = textList;
}

void ButtonSlider::addExtraLongMark(int value, QString strValue, bool boldFlag, QColor textColor, QColor markColor, int lineWidth)
{
	Q_ASSERT(m_extraLongMarkCount < MAX_EXTRA_LONG_MARK_COUNT);
	m_extraLongMark[m_extraLongMarkCount].m_value = value;
	m_extraLongMark[m_extraLongMarkCount].m_strValue = strValue;
	m_extraLongMark[m_extraLongMarkCount].m_boldFlag = boldFlag;
	m_extraLongMark[m_extraLongMarkCount].m_textColor = textColor;
	m_extraLongMark[m_extraLongMarkCount].m_markColor = markColor;
	m_extraLongMark[m_extraLongMarkCount].m_markLineWidth = lineWidth;

	m_extraLongMarkCount++;
}

void ButtonSlider::addExtraShortMark(int value, QString strValue, bool boldFlag, QColor textColor, QColor markColor, int lineWidth)
{
	Q_ASSERT(m_extraShortMarkCount < MAX_EXTRA_SHORT_MARK_COUNT);
	m_extraShortMark[m_extraShortMarkCount].m_value = value;
	m_extraShortMark[m_extraShortMarkCount].m_strValue = strValue;
	m_extraShortMark[m_extraShortMarkCount].m_boldFlag = boldFlag;
	m_extraShortMark[m_extraShortMarkCount].m_textColor = textColor;
	m_extraShortMark[m_extraShortMarkCount].m_markColor = markColor;
	m_extraShortMark[m_extraShortMarkCount].m_markLineWidth = lineWidth;

	m_extraShortMarkCount++;
}


void ButtonSlider::paintEvent(QPaintEvent *event)
{
	if (orientation() == Qt::Vertical)
	{
		paintVerticalEvent(event);
	}
	else
	{
		paintHorizontalEvent(event);
	}
	QSlider::paintEvent(event);
}

void ButtonSlider::paintHorizontalEvent(QPaintEvent * /* event */)
{
	int left = 14;
	int right = 20;
	int top = height()/2; 
	QPainter painter(this);
	painter.setPen(QPen(QColor(10, 10, 10), 2));

//	painter.drawRect(0, 0, width(), height());

	painter.drawLine(left, top, width() - right, top);

	int longMarkTop = top - 16;
	int shortMarkTop = top - 14;
	int markBottom = top - 10;

	int textWidth = 60;
	int textHeight = longMarkTop;
	
	painter.setPen(QPen(QColor(10, 10, 10), 1));

	if ((m_longMarkCount > 1) && (m_shotLongMarkRatio >= 1))
	{
		int unitCount = (m_longMarkCount - 1) * m_shotLongMarkRatio;
		for (int i = 0; i <= unitCount ; i++)
		{
			int x = left + (width() - left - right) * i / unitCount;
			if (( i % m_shotLongMarkRatio) == 0)
			{
				painter.drawLine(x, longMarkTop, x, markBottom);
				QString text =  m_textList[i / m_shotLongMarkRatio];
				painter.drawText(x - textWidth / 2, 0, textWidth, textHeight, Qt::AlignVCenter | Qt::AlignHCenter, text);
//				painter.drawRect(x - textWidth / 2, 0, textWidth, textHeight);
			}
			else
			{
				painter.drawLine(x, shortMarkTop, x, markBottom);
			}
		}
	}

	QFont oldFont = painter.font();
	QFont newFont = oldFont;
	newFont.setBold(true);
	newFont.setPixelSize(12);

	for (int i = 0; i < m_extraLongMarkCount; i++)
	{
		int value = m_extraLongMark[i].m_value;
		if ((value >= minimum()) && (value <= maximum()))
		{
			int x = left + (width() - left - right) * (m_extraLongMark[i].m_value - minimum()) / (maximum() - minimum());
			painter.drawLine(x, longMarkTop, x, markBottom);
			if (m_extraLongMark[i].m_boldFlag == true)
			{
				painter.setFont(newFont);
			}
			painter.drawText(x - textWidth / 2, 0, textWidth, textHeight, Qt::AlignVCenter | Qt::AlignHCenter, m_extraLongMark[i].m_strValue);
			if (m_extraLongMark[i].m_boldFlag == true)
			{
				painter.setFont(oldFont);
			}
		}
	}


	for (int i = 0; i < m_extraShortMarkCount; i++)
	{
		int value = m_extraShortMark[i].m_value;
		if ((value >= minimum()) && (value <= maximum()))
		{
			int x = left + (width() - left - right) * (m_extraShortMark[i].m_value - minimum()) / (maximum() - minimum());
			painter.drawLine(x, shortMarkTop, x, markBottom);
			if (m_extraShortMark[i].m_boldFlag == true)
			{
				painter.setFont(newFont);			
			}
			painter.drawText(x - textWidth / 2, 0, textWidth, textHeight, Qt::AlignVCenter | Qt::AlignHCenter, m_extraShortMark[i].m_strValue);
			if (m_extraShortMark[i].m_boldFlag == true)
			{
				painter.setFont(oldFont);
			}
		}
	}
}


void ButtonSlider::setMarkColor(const QColor &newColor)
{
	m_markColor = newColor;
}

QColor ButtonSlider::getMarkColor() const 
{ 
	return m_markColor; 
}


void ButtonSlider::setTextColor(const QColor &newColor)
{
	m_textColor = newColor;
}

QColor ButtonSlider::getTextColor() const 
{ 
	return m_textColor; 
}

void ButtonSlider::setGrooveHeight(int grooveHeight)
{
	m_GrooveHeight = grooveHeight;
//	m_GrooveHeight = 30;
}

void ButtonSlider::paintVerticalEvent(QPaintEvent * /* event */)
{
	int left = width() / 2 - 1;
	int top = m_GrooveHeight / 2;
	int bottom = m_GrooveHeight / 2 + 1;

	int markLeft = left + 5;
	int longMarkLength = m_longMarkLength;
	int shortMarkLength = m_shortMarkLength;
	int textLeft = 2;

	QPainter painter(this);
//	if (width() / 25 > 1)
	{
		painter.setPen(QPen(m_markColor, 4));
		painter.drawLine(left, 3, left, height() - 3);
//		m_markColor.setAlpha(100);
		painter.setPen(QPen(QColor(255, 255, 255), 1));
		painter.drawLine(left + 1, 1, left + 1, height() - 1);
	}

	painter.setPen(QPen(m_markColor, 1));
//	painter.drawRect(0, 0, width() - 1, height() - 1);

	int unitCount;
	if (m_longMarkCount >= 2)
	{
		unitCount = (m_longMarkCount - 1) * m_shotLongMarkRatio;
	}
	else
	{
		unitCount = m_shotLongMarkRatio;
	}
	for (int i = 0; i <= unitCount ; i++)
	{
		int y = height() - bottom - (height() - top - bottom) * i / unitCount - 1;
		if ((( i % m_shotLongMarkRatio) == 0) && (m_longMarkCount >= 2))
		{
			painter.setPen(QPen(m_markColor, 1));
			if (m_rightMark  == true)
			{
				painter.drawLine(markLeft, y, markLeft + longMarkLength, y);
			}
			if (m_leftMark  == true)
			{
				painter.drawLine(left - 5 - longMarkLength, y, left - 5, y); 
			}
			QString text =  m_textList[i / m_shotLongMarkRatio];
			painter.setPen(QPen(m_textColor, 1));
			painter.drawText(textLeft + markLeft + longMarkLength + 2, y - 11, 150, 20, Qt::AlignVCenter | Qt::AlignLeft, text);
//			painter.drawRect(textLeft + markLeft + longMarkLength + 2, y - 10, 150, 20);
		}
		else
		{
			painter.setPen(QPen(m_markColor, 1));
			if (m_rightMark == true)
			{
				painter.drawLine(markLeft + 1, y, markLeft + shortMarkLength, y);
			}
			if (m_leftMark  == true)
			{
				painter.drawLine(left - 6 - shortMarkLength, y, left - 6, y); 			
			}
		}
	}

#if 0
	QFont font = painter.font();
	font.setBold(true);
	font.setPixelSize(12);
	painter.setFont(font);
#endif

	QFont oldFont = painter.font();
	QFont newFont = oldFont;
	newFont.setBold(true);
	newFont.setPixelSize(12);

	for (int i = 0; i < m_extraLongMarkCount; i++)
	{
		int value = m_extraLongMark[i].m_value;
		if ((value >= minimum()) && (value <= maximum()))
		{
			int y = height() - bottom - (height() - top - bottom) * (value - minimum()) / (maximum() - minimum()) - 1;

			painter.setPen(QPen(m_extraLongMark[i].m_markColor, m_extraLongMark[i].m_markLineWidth));
			if (m_rightMark  == true)
			{
				painter.drawLine(markLeft, y, markLeft + longMarkLength, y);
			}
			if (m_leftMark  == true)
			{
				painter.drawLine(left - 5 - longMarkLength, y, left - 5, y); 
			}


			painter.setPen(QPen(m_extraLongMark[i].m_textColor, 1));
			if (m_extraLongMark[i].m_boldFlag == true)
			{
				painter.setFont(newFont);
			}
			painter.drawText(textLeft + markLeft + longMarkLength + 2, y - 11, 150, 20, Qt::AlignVCenter | Qt::AlignLeft, m_extraLongMark[i].m_strValue);
			if (m_extraLongMark[i].m_boldFlag == true)
			{
				painter.setFont(oldFont);
			}
		}
	}


	for (int i = 0; i < m_extraShortMarkCount; i++)
	{
		int value = m_extraShortMark[i].m_value;
		if ((value >= minimum()) && (value <= maximum()))
		{
			painter.setPen(QPen(m_extraShortMark[i].m_markColor, m_extraShortMark[i].m_markLineWidth));
			int y = height() - bottom - (height() - top - bottom) * (value - minimum()) / (maximum() - minimum()) - 1;
			if (m_rightMark == true)
			{
				painter.drawLine(markLeft + 1, y, markLeft + shortMarkLength, y);
			}
			if (m_leftMark  == true)
			{
				painter.drawLine(left - 6 - shortMarkLength, y, left - 6, y); 			
			}

			painter.setPen(QPen(m_extraShortMark[i].m_textColor, 1));
			if (m_extraShortMark[i].m_boldFlag == true)
			{
				painter.setFont(newFont);			
			}
			painter.drawText(textLeft + markLeft + shortMarkLength + 2, y - 11, 150, 20, Qt::AlignVCenter | Qt::AlignLeft, m_extraShortMark[i].m_strValue);
			if (m_extraShortMark[i].m_boldFlag == true)
			{
				painter.setFont(oldFont);
			}

		}
	}
}
