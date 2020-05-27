#include "commonLib.h"
//#include "windows.h"
//#include <TCHAR.H> 
#include <QJsonObject>
#include <QStringList>
#include <QDebug>
#include <QJsonArray>
#include "JsonTable.h"

/*
LOG_SOCKET_MESSAGE: 
bit 0     -      Default: true,    used to log socket message.

*/
UINT32 g_debug = 1;

bool g_debugEnable = false;

void FormatString(char lpszBuffer[501], BYTE * pucMsg, int len)
{
    int count = 0, i;
    char lpszTemp[] =  "00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F 10 11 12 13 14 15 16 17 18 19 1A 1B 1C 1D 1E 1F 20 21 22 23 24 25 26 27 28 29 2A 2B 2C 2D 2E 2F 30 31 32 33 34 35 36 37 38 39 3A 3B 3C 3D 3E 3F 40 41 42 43 44 45 46 47 48 49 4A 4B 4C 4D 4E 4F 50 51 52 53 54 55 56 57 58 59 5A 5B 5C 5D 5E 5F 60 61 62 63 64 65 66 67 68 69 6A 6B 6C 6D 6E 6F 70 71 72 73 74 75 76 77 78 79 7A 7B 7C 7D 7E 7F 80 81 82 83 84 85 86 87 88 89 8A 8B 8C 8D 8E 8F 90 91 92 93 94 95 96 97 98 99 9A 9B 9C 9D 9E 9F A0 A1 A2 A3 A4 A5 A6 A7 A8 A9 AA AB AC AD AE AF B0 B1 B2 B3 B4 B5 B6 B7 B8 B9 BA BB BC BD BE BF C0 C1 C2 C3 C4 C5 C6 C7 C8 C9 CA CB CC CD CE CF D0 D1 D2 D3 D4 D5 D6 D7 D8 D9 DA DB DC DD DE DF E0 E1 E2 E3 E4 E5 E6 E7 E8 E9 EA EB EC ED EE EF F0 F1 F2 F3 F4 F5 F6 F7 F8 F9 FA FB FC FD FE FF ";

	count = len;
	if (count > 500 / 3)
	{
		count = 500 / 3;
	}

	for (i = 0; i < count; i++)
	{
		memcpy(lpszBuffer + 3 * i, lpszTemp + 3 * pucMsg[i], 3);
//			sprintf(g_lpszFormatString + 3 * i, "%02x ", (int)(pucContent[i]));
	}

	lpszBuffer[count * 3] = 0;
	return;
}

#if 0
/*!
 Load timer with current system tick value.
 \param timerTicks pointer to timer.
 */
void timerLoad(unsigned int *timerTicks) 
{
	*timerTicks = GetTickCount();
}

/*!
 Check for timer expiration with optional reload.
 \param timerTicks pointer to timer.
 \param tickCount expiration limit.
 \param reload TRUE to reload timer on expiration, else FALSE.
 \return TRUE if timer expired, else FALSE.
 */
unsigned int timerExpired(unsigned int *timerTicks, unsigned int tickCount, unsigned int reload) 
{
	unsigned int currentTick = GetTickCount();
	unsigned int lastTick = *timerTicks;

	if (currentTick - lastTick >= tickCount) 
	{
		if (reload) 
		{
			timerLoad(timerTicks);
		}
//	qDebug() << "tick = " << currentTick << " -  " << lastTick << "= " << currentTick - lastTick;
		return TRUE;
	}
	else 
	{
		return FALSE;
	}
}
#endif

BYTE calcChecksum (BYTE* pucData, int iCount)
{
    BYTE ucChecksum = 0;
    int i;

    for (i = 0; i < iCount; i++)
    {
		ucChecksum ^= pucData [i];
	}
    return ucChecksum;
}

UINT32 qt_htonl(UINT32 data)
{
	UINT32 ret;
	ret = (BYTE0(data) << 24) | (BYTE1(data) << 16) | (BYTE2(data) << 8) | BYTE3(data);

	return (ret);
}

void modifyJsonValue(QJsonValue& destValue, const QString& path, const QJsonValue& newValue)
{
    const int indexOfDot = path.indexOf('/');
    const QString dotPropertyName = path.left(indexOfDot);
    const QString dotSubPath = indexOfDot > 0 ? path.mid(indexOfDot + 1) : QString();

    const int indexOfSquareBracketOpen = path.indexOf('[');
    const int indexOfSquareBracketClose = path.indexOf(']');

    const int arrayIndex = path.mid(indexOfSquareBracketOpen + 1, indexOfSquareBracketClose - indexOfSquareBracketOpen - 1).toInt();

    const QString squareBracketPropertyName = path.left(indexOfSquareBracketOpen);
    const QString squareBracketSubPath = indexOfSquareBracketClose > 0 ? (path.mid(indexOfSquareBracketClose + 1)[0] == '.' ? path.mid(indexOfSquareBracketClose + 2) : path.mid(indexOfSquareBracketClose + 1)) : QString();

    // determine what is first in path. dot or bracket
    bool useDot = true;
    if (indexOfDot >= 0) // there is a dot in path
    {
        if (indexOfSquareBracketOpen >= 0) // there is squarebracket in path
        {
            if (indexOfDot > indexOfSquareBracketOpen)
                useDot = false;
            else
                useDot = true;
        }
        else
            useDot = true;
    }
    else
    {
        if (indexOfSquareBracketOpen >= 0)
            useDot = false;
        else
            useDot = true; // acutally, id doesn't matter, both dot and square bracket don't exist
    }

    QString usedPropertyName = useDot ? dotPropertyName : squareBracketPropertyName;
    QString usedSubPath = useDot ? dotSubPath : squareBracketSubPath;

    QJsonValue subValue;
    if (destValue.isArray())
        subValue = destValue.toArray()[usedPropertyName.toInt()];
    else if (destValue.isObject())
        subValue = destValue.toObject()[usedPropertyName];
    else
        qDebug() << "oh, what should i do now with the following value?! " << destValue;

    if(usedSubPath.isEmpty())
    {
        subValue = newValue;
    }
    else
    {
        if (subValue.isArray())
        {
            QJsonArray arr = subValue.toArray();
            QJsonValue arrEntry = arr[arrayIndex];
            modifyJsonValue(arrEntry,usedSubPath,newValue);
            arr[arrayIndex] = arrEntry;
            subValue = arr;
        }
        else if (subValue.isObject())
            modifyJsonValue(subValue,usedSubPath,newValue);
        else
            subValue = newValue;
    }

    if (destValue.isArray())
    {
        QJsonArray arr = destValue.toArray();
        arr[arrayIndex] = subValue;
        destValue = arr;
    }
    else if (destValue.isObject())
    {
        QJsonObject obj = destValue.toObject();
        obj[usedPropertyName] = subValue;
        destValue = obj;
    }
    else
        destValue = newValue;
}

void modifyJsonValue(QJsonDocument& doc, const QString& path, const QJsonValue& newValue)
{
    QJsonValue val;
    if (doc.isArray())
        val = doc.array();
    else
        val = doc.object();

    modifyJsonValue(val,path,newValue);

    if (val.isArray())
        doc = QJsonDocument(val.toArray());
    else
        doc = QJsonDocument(val.toObject());
}

QString readObject(QJsonObject &json, QString keyList)
{
	/* First, we need to jump out "/" */
	QStringList list = keyList.split("/");
	QStringList effectiveList;

	for (int index = 0; index < list.length(); index++)
	{
		QString subString = list.at(index);
		if (!subString.isEmpty())
		{
			effectiveList << subString;
		}
	}

	if (effectiveList.length() == 0)
	{
		return ("");
	}
	else
	{
		return readObject(json, effectiveList, 0);
	}
}

QString readObject(QJsonObject &json, QStringList& list, int startIndex)
{
	QString key = list.at(startIndex);
//	qDebug() << key << startIndex;
	if (startIndex == list.length() - 1)
	{
		return (json[key].toString());
	}
	else
	{
		QJsonObject nested = json[key].toObject();
		return (readObject(nested, list, startIndex + 1));	
	}
}

int readInt(QString valueString, bool &result)
{
	int value = 0;
	QRegExp rx_int;
	rx_int.setPattern(REGULAR_EXPRESSION_INTEGER); 
	if (rx_int.indexIn(valueString) != -1) 
	{
		value = rx_int.cap(1).toInt();
		result = true;
	}
	else
	{
		result = false;
	}
	return (value);
}

float readFloat(QString valueString, bool &result)
{
	float value = (float)0;
	QRegExp rx_float;
	rx_float.setPattern(REGULAR_EXPRESSION_FLOAT); 
	if (rx_float.indexIn(valueString) != -1) 
	{
		value = rx_float.cap(1).toFloat();
		result = true;
	}
	else
	{
		result = false;
	}

	return (value);
}

QString readIPAddr(QString valueString, bool &result)
{
	QString ret;
	QString pattern("((2([0-4]\\d|5[0-5])|[0-1]?\\d\\d?)\\.){3}(2([0-4]\\d|5[0-5])|[0-1]?\\d\\d?)");
	QRegExp rx(pattern);
	if (rx.indexIn(valueString) != -1)
	{
		result = true;
		ret = rx.cap(0);
	}
	else
	{
		result = false;
	}
	return (ret);
}

void setBit(UINT32 &value, int bit)
{
	value = value | (1 << bit);
}

void clearBit(UINT32 &value, int bit)
{
	UINT32 bitnot = ~(1 << bit);
	value = value & bitnot;
}


void setGeometry(QWidget *pWidget, POSITION left, POSITION top, POSITION width, POSITION height)
{
	QWidget *parentWidget = pWidget->parentWidget();
	int parentWidth = parentWidget->width();
	int parentHeight = parentWidget->height();

	/* Left Position*/
	int leftPos = parentWidth * left.m_percent;
	if (left.m_min > 0)
	{
		if (left.m_min > leftPos)
		{
			leftPos = left.m_min;
		}
	}
	if (left.m_max > 0)
	{
		if (left.m_max < leftPos)
		{
			leftPos = left.m_max;
		}
	}

	/* Top Position*/
	int topPos = parentHeight * top.m_percent;
	if (top.m_min > 0)
	{
		if (top.m_min > topPos)
		{
			topPos = top.m_min;
		}
	}
	if (top.m_max > 0)
	{
		if (top.m_max < topPos)
		{
			topPos = top.m_max;
		}
	}

	/* Right Position*/
	int widthPos = parentWidth * width.m_percent;
	if (width.m_min > 0)
	{
		if (width.m_min > widthPos)
		{
			widthPos = width.m_min;
		}
	}
	if (width.m_max > 0)
	{
		if (width.m_max < widthPos)
		{
			widthPos = width.m_max;
		}
	}


	/* bottom Position*/
	int heightPos = parentHeight * height.m_percent;
	if (height.m_min > 0)
	{
		if (height.m_min > heightPos)
		{
			heightPos = height.m_min;
		}
	}
	if (height.m_max > 0)
	{
		if (height.m_max < heightPos)
		{
			heightPos = height.m_max;
		}
	}

	pWidget->setGeometry(leftPos, topPos, widthPos, heightPos);
}

void test()
{
	int index = getTableValue(g_onOffTable, "on");
	qDebug() << "test Index = " << index;

	QString explain = getTableString(g_inputSourceTable, 4);
	if (explain.isEmpty())
	{
		qDebug() << "error: explain is empty";
	}
	else
	{
		qDebug() << "explain = " << explain;
	}
}