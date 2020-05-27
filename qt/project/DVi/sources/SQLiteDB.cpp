#include "SQLiteDB.h"
#include <QSqlQuery>
#include "simpleQtLogger.h"
#include <QDebug>
#include <qsqlerror.h>

SQLiteDB *g_pDatabase;

SQLiteDB::SQLiteDB()
{
/*
      We can't call init() before the main QApplication class, or it will show the following error:
          QSqlDatabase: QSQLITE driver not loaded
          QSqlDatabase: available drivers:
          QSqlDatabase: an instance of QCoreApplication is required for loading driver plugins

      This error only happened at Android or Linux. For Windows, it work normally.

      Please see the following webpage for more information.
            https://forum.qt.io/topic/38486/solved-qt-sql-database-driver-not-loaded-in-android
*/

    init();
}

void SQLiteDB::init(void)
{
    m_database = QSqlDatabase::addDatabase("QSQLITE");
	m_bOpened = false;
}

SQLiteDB::~SQLiteDB(void)
{
	closeDB();
}

void SQLiteDB::closeDB()
{
	if (m_bOpened == true)
	{
		m_bOpened = false;
		m_database.close();
	}
}

bool SQLiteDB::setDBFileName(QString dbFileName)
{
	if (m_bOpened == true)
	{
		if (m_oldDBFileName == dbFileName)
		{
			return (true);
		}
		else
		{
			m_database.close();
			m_bOpened = false;
		}
	}

	m_database.setDatabaseName(dbFileName);
	if (m_database.open())
	{
		m_bOpened = true;
		m_oldDBFileName = dbFileName;
		return (true);
	}
	else
	{
		m_bOpened = false;
		L_ERROR("open db failed: " + m_database.lastError().databaseText());
		return (false);
	}
}


bool SQLiteDB::setGlobalString(QString dbFileName, QString &content)
{
	if (setDBFileName(dbFileName) == false)
	{
		L_ERROR("Failed to open database:" + dbFileName);
		return (false);
	}
	else
	{
		QSqlQuery query;

        QString script = QString("UPDATE Presets SET Settings='%1' where NameID = 'globals'").arg(content);
		bool ret = query.exec(script);
		if (ret == false)
		{
			L_ERROR(QString("setGlobalString failed for %1: %2").arg(dbFileName).arg(query.lastError().databaseText()));
		}

		return (ret);
	}
}

bool SQLiteDB::getGlobalString(QString dbFileName, QString &content)
{
	if (setDBFileName(dbFileName) == false)
	{
		L_ERROR("Failed to open database:" + dbFileName);
		return (false);
	}
	else
	{
		QSqlQuery query;

		QString script = QString("select Settings from  Presets where NameID = 'globals'");
		bool ret = query.exec(script);
		if (ret == true)
		{
			//Let's export the words to XML files.
			while (query.next())
			{
				content = query.value(0).toString();
				return (true);
			}
		}
	}
	return (false);
}

bool SQLiteDB::getPreset(QString dbFileName, int presetIndex, QString &presetName, QString &presetContent)
{
	if (setDBFileName(dbFileName) == false)
	{
		L_ERROR("Failed to open database:" + dbFileName);
		return (false);
	}
	else
	{
		QSqlQuery query;

		QString script = QString("select PresetName, Settings from  Presets where NameID = '%1'").arg(presetIndex);
		bool ret = query.exec(script);
		if (ret == true)
		{
			//Let's export the words to XML files.
			while (query.next())
			{
				presetName = query.value(0).toString();
				presetContent = query.value(1).toString();
				return (true);
			}
		}
	}
	return (false);
}
