
#ifndef _COMMON_LIB_H_
#define _COMMON_LIB_H_

#include "types.h"
#include <QString>
#include <QJsonObject>
#include <QJsonDocument>
#include <QWidget>

#define REGULAR_EXPRESSION_INTEGER	"(-?\\d+)"
#define REGULAR_EXPRESSION_FLOAT	"(-?\\d+(\\.\\d+)?)"

#define BYTE0(value) ((value) & 0xFF)
#define BYTE1(value) (((value) >> 8) & 0xFF)
#define BYTE2(value) (((value) >> 16) & 0xFF)
#define BYTE3(value) (((value) >> 24) & 0xFF)

#define BIT(value, bit) (((value) >> (bit)) & 1)

/* All the functions here MUST support multi-thread !!! */

void FormatString(char lpszBuffer[501], BYTE * pucMsg, int len);

#if 0
void timerLoad(unsigned int *timerTicks);

unsigned int timerExpired(unsigned int *timerTicks, unsigned int tickCount, unsigned int reload);
#endif

BYTE calcChecksum (BYTE* pucData, int iCount);

QString readIPAddr(QString valueString, bool &result);
UINT32 qt_htonl(UINT32 data);

int readInt(QString valueString, bool &result);

float readFloat(QString valueString, bool &result);


extern QString readObject(QJsonObject &json, QString keyList);

extern QString readObject(QJsonObject &json, QStringList& list, int startIndex);

extern bool modifyObject(QJsonObject &json, QString keyString, QString &valueString);

//extern bool modifyObject(QJsonObject &json, QStringList& list, int startIndex, QString& valueString);
extern void modifyJsonValue(QJsonDocument& doc, const QString& path, const QJsonValue& newValue);

/* The following is for debug */
// =============================================================
void msSleep(int ms);

#define LOG_SOCKET_MESSAGE  0		/* bit 0 is used for the switch to trace SOCKET message */
extern UINT32 g_debug;
extern bool g_debugEnable;		/* Total debug switch */

void setBit(UINT32 &value, int bit);
void clearBit(UINT32 &value, int bit);

// =============================================================


void test();

// =============================================================

typedef	struct 
{
	QString m_strKeyList;
	QString m_valueString;
}JSON_KEY_STRING;

class POSITION 
{
public:
	POSITION(double percent, int min = 0, int max = 0)
	{
		m_percent = percent;
		m_min = min;
		m_max = max;
	}

	double m_percent;
	int m_min, m_max;
};

void setGeometry(QWidget *m_pWidget, POSITION left, POSITION top, POSITION width, POSITION height);


#endif


