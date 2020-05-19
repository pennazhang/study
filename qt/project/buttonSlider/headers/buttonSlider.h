#ifndef BUTTONSLIDER_H
#define BUTTONSLIDER_H

#include "QSlider"
#include <QDoubleSpinBox>

typedef struct
{
	int m_value;
	QString m_strValue;
	bool m_boldFlag;
	QColor m_textColor;
	QColor m_markColor;
	int m_markLineWidth;
} EXTRA_MARK;

#define MAX_EXTRA_LONG_MARK_COUNT 80
#define MAX_EXTRA_SHORT_MARK_COUNT 80

class ButtonSlider : public QSlider
{
public:
	Q_PROPERTY(QColor textColor READ getTextColor WRITE setTextColor)
	void setTextColor(const QColor &newColor);
	QColor getTextColor() const;
	void setMarkColor(const QColor &newColor);
	QColor getMarkColor() const;
	ButtonSlider(QWidget *pParent, Qt::Orientation orientation =  Qt::Vertical);
	void setMark(int longMarkCount, int shortLongMarkRatio, QStringList &textList);
	void addExtraLongMark(int value, QString strValue, bool boldFlag = false, QColor textColor = QColor(0x10, 0x10, 0x10), QColor markColor = QColor(0x10, 0x10, 0x10), int lineWidth = 1);
	void addExtraShortMark(int value, QString strValue, bool boldFlag = false, QColor textColor = QColor(0x10, 0x10, 0x10), QColor markColor = QColor(0x10, 0x10, 0x10), int lineWidth = 1);
	void setMarkLength(int shortMarkLength = 3, int longMarkLength = 5);
	void setMarkSide(bool leftMark = false, bool rightMark = true);
	void setGrooveHeight(int grooveHeight);
	void setCenterOffset(int offset) { m_centerOffset = offset; };

protected:
	void paintEvent(QPaintEvent *event);
	void paintVerticalEvent(QPaintEvent *event);
	void paintHorizontalEvent(QPaintEvent *event);
	void keyPressEvent(QKeyEvent *event);
	int m_upperValue;
	int m_lowerValue;
	int m_longMarkCount;
	int m_centerOffset;		// >0 means move right or down from the center.
	int m_shotLongMarkRatio;  /* Short mark count divide by long mark count */
	int m_shortMarkLength, m_longMarkLength;
	bool m_leftTopMark;		// true: we need to mark at the left side of the vertical groove, or top side of the horizontal groove.
	bool m_rightBottomMark;		// true: we need to mark at the top side of the vertical groove, or bottom side of the horizontal groove.
	QStringList m_textList;

	EXTRA_MARK m_extraLongMark[MAX_EXTRA_LONG_MARK_COUNT];
	int m_extraLongMarkCount;
	EXTRA_MARK m_extraShortMark[MAX_EXTRA_SHORT_MARK_COUNT];
	int m_extraShortMarkCount;
	QColor m_textColor, m_markColor;
	int m_GrooveHeight;	// The height of the slider button, it affect the start / end line of the slider.
};


#endif
