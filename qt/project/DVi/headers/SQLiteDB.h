#ifndef SQLITE_DB_H
#define SQLITE_DB_H

#include <QSqlDatabase>
#include <QString>
#include "types.h"


class SQLiteDB
{
public:
	SQLiteDB(void);
    void init(void);
	~SQLiteDB(void);

	bool getPreset(QString dbFileName, int presetIndex, QString &presetName, QString &presetContent);
	bool getGlobalString(QString dbFileName, QString &content);
	bool setGlobalString(QString dbFileName, QString &content);
	void closeDB();


protected:
	bool setDBFileName(QString dbFileName);

protected:	
	QString m_oldDBFileName;
	bool m_bOpened;
	QSqlDatabase m_database;
};

extern SQLiteDB *g_pDatabase;


#endif
