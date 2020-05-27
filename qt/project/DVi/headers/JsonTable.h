#ifndef JSON_TABLE_H
#define JSON_TABLE_H

#include <QString>
#include <QJsonObject>
#include <QVariant>

typedef struct
{
	int		m_value;
	QString	m_explain;
} EXPLAIN_TABLE;

typedef struct
{
	int			m_stringID;
	QString		m_JSONKey;
	void		*m_pValue;
	EXPLAIN_TABLE	*m_pExplainTable;
	QVariant	m_defaultValue;
} JSON_STRING_TABLE;

typedef struct 
{
	EXPLAIN_TABLE * m_pTable;
	int	m_tableCount;
} TABLE_ITEM;

extern EXPLAIN_TABLE g_onOffTable[], g_boolTable[], g_inputSourceTable[], g_myTable[];



extern int getTableValue(EXPLAIN_TABLE *table, QString string, bool *pResult = nullptr);

extern QString getTableString(EXPLAIN_TABLE *table, int value);

extern int getTableCount(EXPLAIN_TABLE *table);

void readJsonTable(QJsonObject &mainObject, JSON_STRING_TABLE *pStringTable, int tableSize);

#endif

