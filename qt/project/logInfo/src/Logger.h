#ifndef QT_LOGGER_H
#define QT_LOGGER_H

#include <QTimer>
#include <QTextStream>

typedef enum {
  LOG_LEVEL_EMERGENCY = 0,
  LOG_LEVEL_ALERT,    
  LOG_LEVEL_CRITICAL, 
  LOG_LEVEL_ERROR,     
  LOG_LEVEL_WARNING,    
  LOG_LEVEL_NOTICE,  
  LOG_LEVEL_INFO,  
  LOG_LEVEL_DEBUG,
} LOG_LEVEL;

class Logger : public QObject
{
    Q_OBJECT
public:	
	Logger(QObject *parent = NULL);
	~Logger();

	static Logger* getInstance();
	void log(const QString& text, LOG_LEVEL logLevel, const QString& functionName, const char* fileName, unsigned int lineNumber);
	bool isLogLevelEnable(LOG_LEVEL logLevel);

public slots:
//	void onTimer_CmdLoop();
	
protected:
	QString m_strLogFormat;
	QStringList m_logList;
};

#define LOG_ERROR(text)	LOG(LOG_LEVEL_ERROR, text)

#define LOG(logLevel, text)		\
	do \
	{ \
		if (g_pLogger->isLogLevelEnable(logLevel)) \
		{ \
			Logger::getInstance()->log(text, LOG_LEVEL_ERROR, __FUNCTION__, __FILE__, __LINE__); \
		} \
	} while (0)


extern Logger *g_pLogger;
extern QTextStream g_stdOut;

#endif
