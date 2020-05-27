#include "JsonTable.h"
#include "simpleQtLogger.h"
#include "commonLib.h"
#include "types.h"
#include <qdebug.h>

EXPLAIN_TABLE g_onOffTable[] = 
{
		{ 0,	"Off" },
		{ 1,	"On" },
};

EXPLAIN_TABLE g_boolTable[] = 
{
		{ 0,	"Off" },
		{ 1,	"On" },
};

EXPLAIN_TABLE g_inputSourceTable[] = 
{
	{ 4, "Digital Cinema" },
	{ 3, "MIC" },
	{ 2, "Analog Cinema" },
	{ 1, "Music" },
};

TABLE_ITEM g_table[] = 
{
	{	g_onOffTable, sizeof(g_onOffTable)/sizeof(EXPLAIN_TABLE) },
	{	g_boolTable, sizeof(g_boolTable)/sizeof(EXPLAIN_TABLE) },
	{	g_inputSourceTable, sizeof(g_inputSourceTable)/sizeof(EXPLAIN_TABLE) },
};


int getTableValue(EXPLAIN_TABLE *table, QString explain, bool *pResult)
{
	int count = -1, i;
	
	count = getTableCount(table);
	if (count <= 0)
	{
		if (pResult != nullptr)
		{
			*pResult = false;
		}
		return (-1);
	}

	for (i = 0; i < count; i++)
	{
		if (table[i].m_explain == explain)
		{
			break;
		}
	}

	if (i < count)
	{
		if (pResult != nullptr)
		{
			*pResult = true;
		}
		return table[i].m_value;
	}
	else
	{
		L_ERROR("Can't find table string:\"" + explain + "\" in table:" + QString::number((ulong)table, 16));
		if (pResult != nullptr)
		{
			*pResult = false;
		}
		return (-1);
	}
}

QString getTableString(EXPLAIN_TABLE *table, int value)
{
	int count = -1, i;
	
	count = getTableCount(table);
	if (count <= 0)
	{
		return ("");
	}

	for (i = 0; i < count; i++)
	{
		if (table[i].m_value == value)
		{
			return table[i].m_explain;
		}
	}

	L_ERROR("Failed to get string where table = " + QString::number((ulong)table, 16) + ", value = " + QString::number(value));
	return ("");
}

int getTableCount(EXPLAIN_TABLE *table)
{
    int count = -1;
    ULONG i;
	for (i = 0; i < sizeof(g_table) / sizeof(TABLE_ITEM); i++)
	{
		if (table == g_table[i].m_pTable)
		{
			count = g_table[i].m_tableCount;
			break;
		}
	}
	if (count == -1)
	{
		L_ERROR(QString("Can't find table:") + QString::number((ulong)table, 16));
	}
	return (count);
}

void readJsonTable(QJsonObject &mainObject, JSON_STRING_TABLE *pStringTable, int tableSize)
{
	QString valueString;
	for (int i = 0; i < tableSize; i++)
	{
		JSON_STRING_TABLE *pItem = pStringTable + i;
		valueString = readObject(mainObject, pItem->m_JSONKey);

		switch (pItem->m_stringID)
		{
		case JSON_STRING:
			{
				QString *pString = (QString *)(pItem->m_pValue);
				if (!valueString.isEmpty())
				{
					*pString = valueString;
				}
				else
				{
					L_ERROR("Failed to read Node: " + pItem->m_JSONKey);
					*pString = pItem->m_defaultValue.toString();
				}
			}
			break;

		case JSON_INT:
			{
				int *pInt = (int *)(pItem->m_pValue);
				bool result = false;

				if (!valueString.isEmpty())
				{
					qDebug() << valueString;
					*pInt = readInt(valueString, result);
				}

				if (result == false)
				{
					L_ERROR("Failed to read Node: " + pItem->m_JSONKey);
					*pInt = pItem->m_defaultValue.toInt();
				}
			}
			break;

		case JSON_FLOAT:
			{
				float *pFloat = (float *)(pItem->m_pValue);
				bool result = false;

				if (!valueString.isEmpty())
				{
					qDebug() << valueString;
					*pFloat = readFloat(valueString, result);
				}

				if (result == false)
				{
					L_ERROR("Failed to read Node: " + pItem->m_JSONKey);
					*pFloat = pItem->m_defaultValue.toFloat();
				}
			}
			break;
		}
	}
}
