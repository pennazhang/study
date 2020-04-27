#ifndef _MAIN_H
#define _MAIN_H

#include <QString>
#include <QList>

typedef enum 
{
	NODE_NONE = 0,
	NODE_ITEM = 1,
	NODE_GROUP = 2,
} NODE_TYPE;

class MainData
{
public:
	MainData();
	void initXMLFilePath();
	const QString & getXMLFilePath() { return m_strXMLFilePath; }
	NODE_TYPE getCurrentNode(QStringList & childList, QString &strCommand);
	const QStringList& getCurrentNodeName();
	void setNextStep(int step);
	void loadCurrentNodeId();
	void saveCurrentNodeId();
	void saveLastChoice();

protected:
	
    QList<int> m_currentNodeId;
	QStringList m_currentNodeName;
	QString m_strXMLFilePath;
};

#endif
