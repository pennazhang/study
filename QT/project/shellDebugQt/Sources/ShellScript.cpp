#include <QWaitCondition>
#include "MainWidget.h"
#include "ShellScript.h"
#include <QDebug>
#include "ShellSocket.h"
#include "MainApplication.h"

ShellScript* g_pShellScript = nullptr;

void outputString(const QString & string)
{
	if (g_pActiveShell == nullptr)
	{
		qDebug() << string;
	}
	else
	{
		g_pActiveShell->sendString(string + "\r\n");
	}
}

ShellScript::ShellScript()
{
	QStringList funcList;

	m_pScriptEngine = new QScriptEngine(this);
    QScriptValue scriptBrowser1 = m_pScriptEngine->newQObject(this);
    QScriptValue global1 = m_pScriptEngine->globalObject();
    global1.setProperty("shell", scriptBrowser1);

	funcList.clear();
	funcList << 
		"void sleep(int ms);" <<
		"void showParam(QString str, int value);" <<
		"void showParam(QString str, qreal value);";
	addToHelpInfo("shell", funcList);

    QScriptValue scriptBrowser2 = m_pScriptEngine->newQObject(g_pMainWidget);
    QScriptValue global2 = m_pScriptEngine->globalObject();
    global2.setProperty("main", scriptBrowser2);
	funcList.clear();
	funcList << 
		"void setSize(int width, int height);" <<
		"void showSize();";
	addToHelpInfo("main", funcList);
}

bool ShellScript::runCmd(QString strCmd, int& ret, QString &failReason)
{
    QScriptValue result = m_pScriptEngine->evaluate(strCmd);

    if (result.isError())
    {
        qDebug() << "Run script " << strCmd <<"Failed, reason = " << result.toString();
		failReason = result.toString();
		return (false);
    }
	else
	{
		ret = result.toInt32();
		return (true);
	}
}

void ShellScript::addToHelpInfo(const QString & objectName, const QStringList & funcList)
{
	outputString(objectName);
	for (int i = 0; i < funcList.count(); i++)
	{
		outputString(QString("\t") + funcList[i]);
	}

	ObjectInfo *pObjectInfo = new ObjectInfo;
	pObjectInfo->m_objectName = objectName;
	pObjectInfo->m_functionList = funcList;
	m_helpInfo.append(pObjectInfo);
}

void ShellScript::lkup(const QString & objectName, const QString&funcName)
{
	for (int i = 0; i < m_helpInfo.size(); ++i) 
	{
	    ObjectInfo *pObjectInfo = m_helpInfo.at(i);
		if (!objectName.isEmpty())
		{
			if (pObjectInfo->m_objectName.indexOf(objectName, 0, Qt::CaseInsensitive) == -1)
			{
				continue;
			}
		}
		outputString("----------------------------------------------------------------------");
		outputString(pObjectInfo->m_objectName);

		// We have found the objectName.
		for (int j = 0; j < pObjectInfo->m_functionList.count(); j++)
		{
			if (!funcName.isEmpty())
			{
				if (pObjectInfo->m_functionList[j].indexOf(funcName, 0, Qt::CaseInsensitive) == -1)
				{
					continue;
				}
			}
			outputString(QString("\t") + pObjectInfo->m_functionList[j]);
		}
	}	
}


void ShellScript::sleep(int second)
{
	if (second > 10)
	{
		second = 10;
	}

	for (int i = 0; i < second; i++)
	{
		QString strValue;
		strValue.sprintf("%d...", i);
		outputString(strValue);
		g_pApp->mSleep(1000);
	}

	outputString("\n");
}

void ShellScript::showParam(QString str, int value)
{
	QString strValue;
	strValue.setNum(value);
	QString strResult;
	strResult = QString("param1 is: ") + str + ("\r\n") + QString("param2 is: ") + strValue + QString("\r\n");
	outputString(strResult);
}

void ShellScript::showParam(QString str, qreal value)
{
	QString strValue;
	strValue.setNum(value);
	QString strResult;
	strResult = QString("param1 is: ") + str + ("\r\n") + QString("param2 is: ") + strValue + QString("\r\n");
	outputString(strResult);
}

ShellScript::~ShellScript()
{
	qDebug() << "quit shellScript...\n";
	
	for (int i = 0; i < m_helpInfo.size(); ++i) 
	{
	    ObjectInfo *pObjectInfo = m_helpInfo.at(i);
		delete pObjectInfo;
	}
	m_helpInfo.clear();
}
