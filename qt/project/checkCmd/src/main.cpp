#include <QTextStream>
#include <QThread>
#include <QProcess>
#include <QString>
#include <QFileInfo>
#include <QSettings>
#include <QFile>     
#include <QtXml>  
#include <QDomDocument> 
#include "main.h"

QTextStream cout(stdout);
QTextStream cin(stdin);
MainData g_mainData;

#define INI_FILE_NAME	"checkCmd.ini"
#define SESSION_NAME	"settings"
#define KEY_COMMAND_FILE	"Command File"
#define KEY_CURRENT_NODE	"LAST CHOICE"
#define DEFAULT_COMMAND_FILE "/git/study/cmd_index.xml"


MainData::MainData()
{
	initXMLFilePath();
}

void MainData::setNextStep(int step)
{
	if (step == 0)
	{
		if (m_currentNodeId.count() != 0)
		{
			m_currentNodeId.removeLast();
		}
	}
	else
	{
		m_currentNodeId.append(step);
	}
}

void MainData::loadCurrentNodeId()
{
	QSettings iniSetting(INI_FILE_NAME, QSettings::IniFormat);
	QString strNode = iniSetting.value(QString("%1/%2").arg(SESSION_NAME).arg(KEY_CURRENT_NODE)).toString();
	QStringList nodeList = strNode.split(" ", QString::SkipEmptyParts);
//	for (int i = 0; i < nodeList.count(); i++)
//	{
//		cout << i << " " << nodeList.at(i).trimmed() << endl;
//	}
	int count = nodeList.at(0).trimmed().toInt();

	m_currentNodeId.clear();
//	cout << count << endl;
//	cout << nodeList.count() << "  " << strNode << endl;
	if (count != nodeList.count() - 1)
	{
		cout << "Invalid Node List: " << strNode << endl;
		saveCurrentNodeId();
		return;
	}

	for (int i = 0; i < count; i++)
	{
		m_currentNodeId.append(nodeList.at(i + 1).trimmed().toInt());
	}
}

void MainData::saveCurrentNodeId()
{
	QSettings iniSetting(INI_FILE_NAME, QSettings::IniFormat); // 当前目录的INI文件
	iniSetting.beginGroup(SESSION_NAME);
	QString currentNode;
	currentNode.sprintf("%d ", m_currentNodeId.count());
	for (int i = 0; i <  m_currentNodeId.count(); i++)
	{
		QString strNode;
		strNode.sprintf("%d ", m_currentNodeId.at(i));
		currentNode += strNode;
	}

	iniSetting.setValue(KEY_CURRENT_NODE, currentNode);
    iniSetting.endGroup();
	iniSetting.sync();
}

void MainData::saveLastChoice()
{
	saveCurrentNodeId();
}

void MainData::initXMLFilePath()
{
	QFileInfo file(INI_FILE_NAME);
	if(file.exists()==false)
	{
		QSettings iniSetting(INI_FILE_NAME, QSettings::IniFormat); // 当前目录的INI文件
		iniSetting.beginGroup(SESSION_NAME);
		iniSetting.setValue(KEY_COMMAND_FILE, DEFAULT_COMMAND_FILE);
		iniSetting.setValue(KEY_CURRENT_NODE, "0");
	    iniSetting.endGroup();
		iniSetting.sync();
	}

	QSettings iniSetting(INI_FILE_NAME, QSettings::IniFormat);
    m_strXMLFilePath = iniSetting.value(QString("%1/%2").arg(SESSION_NAME).arg(KEY_COMMAND_FILE)).toString();
	loadCurrentNodeId();
}

const QStringList & MainData::getCurrentNodeName() 
{
	return m_currentNodeName;
}

NODE_TYPE MainData::getCurrentNode(QStringList & childList, QString &strCommand)
{
	NODE_TYPE ret;

    if(m_strXMLFilePath.isEmpty())
	{
		cout << "file_name is NULL" << endl;
		return NODE_NONE;  
	}

    QFile file(m_strXMLFilePath);  
    if(!file.open(QFile::ReadOnly | QFile::Text))
    {  
//       QMessageBox::information(NULL, QString("title"), QString("open error!"));
		cout << "Failed to open file:" << m_strXMLFilePath << endl;
		return NODE_NONE;  
    }
  
    QDomDocument document;  
    QString error;  
    int row = 0, column = 0;  
	if(!document.setContent(&file, false, &error, &row, &column))
    {  
//       QMessageBox::information(NULL, QString("title"), QString("parse file failed at line row and column") + QString::number(row, 10) + QString(",") + QString::number(column, 10));
		cout << "Failed to setContent, error = " << error << ", row = " << row << endl;
		file.close();
		return NODE_NONE;  
    }  
	
	file.close();
	if(document.isNull())
    {  
 //      QMessageBox::information(NULL, QString("title"), QString("document is null!"));
		cout << "document is NULL" << endl;
		 return NODE_NONE;  
    }  
  
    QDomElement root = document.documentElement();  
	m_currentNodeName.clear();

   //root_tag_name为persons
    QString root_tag_name = root.tagName();
	if (root_tag_name != "CATALOG")
	{
		cout << "Invalid tag for root node: " << root_tag_name << endl;
		return NODE_NONE;
	}

	QDomElement element = root;
	for (int i = 0; i < m_currentNodeId.count(); i++)
	{
		QDomNodeList list = element.childNodes();
		QDomNode dom_node = list.item(m_currentNodeId.at(i) - 1);
		element = dom_node.toElement();
		m_currentNodeName << element.attribute("title");
	}

    QString tag_name = element.tagName();

	if ((tag_name == "CATALOG") || (tag_name == "GROUP"))
	{
		ret = NODE_GROUP;
		QDomNodeList child_list = element.childNodes();
		childList.clear();
		for (int i = 0; i < child_list.count(); i++)
		{
			QDomNode child_dom_node = child_list.item(i);
			QDomElement child_element = child_dom_node.toElement();
			QString childName = child_element.attribute("title");
			childList.append(childName);
		}
	}
	else if (tag_name == "ITEM")
	{
		ret = NODE_ITEM;
		QDomElement child_element = element.firstChildElement();
		if (child_element.tagName() != "script")
		{
			cout << "Invalid tag Name: " << child_element.tagName();
			return NODE_NONE;
		}
		strCommand = element.text();
	}
	else
	{
		cout << "Invalid tag name" << tag_name << endl;
		return NODE_NONE;
	}

	return (ret);
}

	
int main(int argc, char **argv)
{
	QString strCmd;
    QCoreApplication app(argc, argv);

	for (;;)
	{
		QStringList childList;
		QString strCommand;
		QString strChoice;
		int nChoice;

		cout << "---------------------------------------------------------------------" << endl;
		NODE_TYPE nodeType = g_mainData.getCurrentNode(childList, strCommand);
		QStringList currentNodeName = g_mainData.getCurrentNodeName();
		switch (nodeType)
		{
			case NODE_NONE:
			{
				QString strCommandFilePath = g_mainData.getXMLFilePath() ;
				cout << QString("Faild to read file: %1, Please check the configure file: %2!").arg(strCommandFilePath).arg(INI_FILE_NAME) << endl;
				return (-1);
			}

			case NODE_GROUP:
			{
//				cout << "Current position: " << g_mainData.getCurrentNodeName() << endl;
				cout << "/root =>" << endl;
				for (int i = 0; i < currentNodeName.count(); i++)
				{
					cout << "    ";
					for (int j = 0; j < i; j++)
					{
						cout << "    ";
					}
					cout << currentNodeName.at(i) << " =>" << endl;
				}
				for (int i = 0; i < childList.count(); i++)
				{
					for (int j = 0; j <= currentNodeName.count(); j++)
					{
						cout << "    ";
					}
					cout << QString("%1. -- %2").arg(i + 1).arg(childList.at(i)) << endl;
				}

				for (int j = 0; j <= currentNodeName.count(); j++)
				{
					cout << "    ";
				}
				cout << "b. -- Back to previous level" << endl;

				for (int j = 0; j <= currentNodeName.count(); j++)
				{
					cout << "    ";
				}
				cout << "x. -- Exit" << endl << endl;

				for (int j = 0; j <= currentNodeName.count(); j++)
				{
					cout << "    ";
				}
				cout << "    Enter you choice: ";
				cout.flush();
				cin >> strChoice;
				cout << endl;
				if (strChoice.length() == 0)
				{
					break;
				}

				if ((strChoice.at(0) == 'x') || (strChoice.at(0) == 'X'))
				{
					exit(0);
				}
				else if ((strChoice.at(0) == 'b') || (strChoice.at(0) == 'B'))
				{
					/* Back to previous menu */
					g_mainData.setNextStep(0);
				}
				nChoice = strChoice.at(0).digitValue();
				if ((nChoice > 0) && (nChoice <=  childList.count()))
				{
					g_mainData.setNextStep(nChoice);
				}
				break;
			}
			case NODE_ITEM:
			{
				cout << "/root =>" << endl;
				for (int i = 0; i < currentNodeName.count(); i++)
				{
					cout << "    ";
					for (int j = 0; j < i; j++)
					{
						cout << "    ";
					}
					cout << currentNodeName.at(i) << " =>" << endl;
				}

				cout << endl << strCommand << endl << endl;

				for (int j = 0; j <= currentNodeName.count(); j++)
				{
					cout << "    ";
				}
				cout << "r. -- Run Command" << endl;

				for (int j = 0; j <= currentNodeName.count(); j++)
				{
					cout << "    ";
				}
				cout << "b. -- Back to previous level" << endl;

				for (int j = 0; j <= currentNodeName.count(); j++)
				{
					cout << "    ";
				}
				cout << "x. -- Exit" << endl << endl;

				for (int j = 0; j <= currentNodeName.count(); j++)
				{
					cout << "    ";
				}

				cout << "Enter you choice: ";
				cout.flush();
				cin >> strChoice;
				cout << endl;
				if ((strChoice.at(0) == 'b') || (strChoice.at(0) == 'B'))
				{
					// Back to previous menu.
					g_mainData.setNextStep(0);
				}
				else if ((strChoice.at(0) == 'r') || (strChoice.at(0) == 'R'))
				{
					g_mainData.saveLastChoice();
					// Run your command here...
#if 1
//#ifdef _LINUX_
					QFile f("nextCmd.sh");
			        if(!f.open(QIODevice::WriteOnly | QIODevice::Text))  
					{  
			            cout << "Open failed." << endl;  
						return -1;  
					}  
    
			        QTextStream txtOutput(&f);  
					txtOutput << "#!/bin/bash" << endl;
					txtOutput << "set -e" << endl << endl;
					txtOutput << strCommand << endl;
					f.close();
					exit(0);
#endif					
				}
				else if ((strChoice.at(0) == 'x') || (strChoice.at(0) == 'X'))
				{
					g_mainData.saveLastChoice();
					exit(0);
				}
			}

		}
	}

	return 0;
}
