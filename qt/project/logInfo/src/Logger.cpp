#include <QDateTime>
#include <QNetworkConfigurationManager>
#include <qnetworkaccessmanager.h>
#include <QUrl>
#include <QNetworkRequest>
#include <QDirIterator>
#include <QDebug>
#include <QThread>
#include <QString>
#include <QtCore/qmath.h> 
#include "Logger.h"

Logger *g_pLogger = NULL;
//const QString DEFAULT_LOG_FORMAT_FULL          = "<TS> [<TID>] [<LL>] <TEXT> (<FUNC>@<FILE>:<LINE>)";
const QString DEFAULT_LOG_FORMATL = "<TS> : <TEXT>"; 
const QString CONSOLE_COLOR_ANSI_ESC_CODE[] = 
{
	"\033[0;33m",      // background -     / foreground yellow
	"\033[0;40;33;1m", // background black / foreground yellow
	"\033[0;31m",      // background -     / foreground red
	"\033[0;36m",      // background -     / foreground cyan
	"\033[0;40;36;1m", // background black / foreground cyan
	"\033[0;40;37;1m", // background black / foreground white
	"\033[0;47;35;1m", // background white / foreground magenta
	"\033[0;32m",      // background -     / foreground green
};

const QString LOG_LEVEL_STRING[] = 
{
	"LOG_LEVEL_EMERGENCY",
	"LOG_LEVEL_ALERT" ,    
	"LOG_LEVEL_CRITICAL" , 
	"LOG_LEVEL_ERROR" ,     
	"LOG_LEVEL_WARNING" ,    
	"LOG_LEVEL_NOTICE" ,  
	"LOG_LEVEL_INFO" ,  
	"LOG_LEVEL_DEBUG" ,
};

QString timeStamp()
{
	// time-stamp
	QDateTime dateTime = QDateTime::currentDateTime(); // or better use QDateTime::currentDateTimeUtc() instead
	return dateTime.toString("yyyy-MM-dd hh:mm:ss.zzz");
}

QString threadId()
{
	return QString("%1").arg((unsigned long int)QThread::currentThreadId(),16,16,QLatin1Char('0')); // field-with for 64bit
}

Logger::Logger(QObject *parent)
	: QObject(parent)
{
//	m_strLogFormat = "<TS> [<TID>] [<LL>] <TEXT> (<FUNC>@<FILE>:<LINE>)";
	m_strLogFormat = DEFAULT_LOG_FORMATL;
}

Logger::~Logger()
{

}
	
Logger* Logger::getInstance()
{
	if (g_pLogger == NULL)
	{
		g_pLogger = new Logger();
	}
	
	return (g_pLogger);
}

bool Logger::isLogLevelEnable(LOG_LEVEL logLevel)
{
	return (true);
}


void Logger::log(const QString& text, LOG_LEVEL logLevel, const QString& functionName, const char* fileName, unsigned int lineNumber)
{
	QString strlogFormat = m_strLogFormat;
	if (strlogFormat.indexOf("<COLOR>") != -1)
	{
		strlogFormat.replace("<COLOR>", CONSOLE_COLOR_ANSI_ESC_CODE[logLevel]);
	}
	if (strlogFormat.indexOf("<TS>") != -1)
	{
		QString ts = timeStamp();
		strlogFormat.replace("<TS>", ts);
	}
	if (strlogFormat.indexOf("<TID>") != -1)
	{
		QString tid = threadId();
		strlogFormat.replace("<TID>", tid);
	}
	if (strlogFormat.indexOf("<LL>") != -1)
	{
		strlogFormat.replace("<LL>", LOG_LEVEL_STRING[logLevel]);
	}
	if (strlogFormat.indexOf("<TEXT>") != -1)
	{
		strlogFormat.replace("<TEXT>", text);
	}
	if (strlogFormat.indexOf("<FUNC>") != -1)
	{
		strlogFormat.replace("<FUNC>", functionName);
	}
	if (strlogFormat.indexOf("<FILE>") != -1)
	{
		strlogFormat.replace("<FILE>", fileName);
	}
	if (strlogFormat.indexOf("<LINE>") != -1)
	{
		strlogFormat.replace("<LINE>", QString("%1").arg(lineNumber));
	}

	
	m_logList << strLogFormat;
}

