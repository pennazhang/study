#include <QtCore/QCoreApplication>
#include <QSettings>
#include <QString>
#include <QFileInfo>
#include <QDebug>

/**********************************
[login%20option]
max%20user%20number=10
group%20name=super admin
need%20password=false
***********************************/

#define SESSION_NAME		"login option"
#define KEY_MAX_USER_NUMBER	"max user number"
#define KEY_GROUP_NAME		"group name"
#define KEY_PASSWORD		"need password"

class LoginSetting
{
public:
	LoginSetting(const QString &groupName, int maxUserNumber, bool needPassword);
	bool readFromIni(const QString & strFileName);
	void writeToIni(const QString & strFileName);

public:
	QString m_groupName;
	int m_maxUserNumber;
	bool m_needPassword;
};

LoginSetting::LoginSetting(const QString &groupName, int maxUserNumber, bool needPassword) :
	m_groupName(groupName), m_maxUserNumber(maxUserNumber), m_needPassword(needPassword)
{
}

bool LoginSetting::readFromIni(const QString & strFileName)
{
	QFileInfo file(strFileName);
	if(file.exists() == false)
	{
		qDebug() << "File not exist!";
		return false;
	}

	QSettings settings(strFileName, QSettings::IniFormat);
 
    m_maxUserNumber = settings.value(QString("%1/%2").arg(SESSION_NAME).arg(KEY_MAX_USER_NUMBER)).toInt();
    m_groupName = settings.value(QString("%1/%2").arg(SESSION_NAME).arg(KEY_GROUP_NAME)).toString();
    m_needPassword = settings.value(QString("%1/%2").arg(SESSION_NAME).arg(KEY_PASSWORD)).toBool();
	return (true);
}

void LoginSetting::writeToIni(const QString & strFileName)
{
    QSettings settings(strFileName, QSettings::IniFormat); // 当前目录的INI文件
    settings.beginGroup(SESSION_NAME);
    settings.setValue(KEY_MAX_USER_NUMBER, m_maxUserNumber);
    settings.setValue(KEY_GROUP_NAME, m_groupName);
    settings.setValue(KEY_PASSWORD, m_needPassword);
    settings.endGroup();
	settings.sync();
}

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);

	LoginSetting mySetting("super admin", 10, false);
	mySetting.writeToIni("login_setting.ini");

	LoginSetting salesSetting("sales", 5, true);
	if (salesSetting.readFromIni("login_setting.ini") ==false)
	{
		qDebug() << "Read ini failed!\n";
		return -1;
	}

	qDebug() << "group number = " << salesSetting.m_groupName;
	qDebug() << "max user number = " << salesSetting.m_maxUserNumber;
	qDebug() << "need password = " << salesSetting.m_needPassword;
	
	return app.exec();
}
