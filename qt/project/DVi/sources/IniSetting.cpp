#include "IniSetting.h"
#include <QFileInfo>

#define INI_SCALE_FACTOR_SEGMENT	"ScaleSetting/ScaleFactor"
#define INI_SCALE_FLAG_SEGMENT		"ScaleSetting/ScaleFlag"
#define INI_UID_SEGMENT				"Global/UUID"

IniSetting::IniSetting(QString strFileName)
	: QSettings(strFileName, QSettings::IniFormat)
{
	setIniCodec("UTF-8");
	m_strFileName = strFileName;

	m_scaleFlag = false;
	m_scaleFactor = 1.0;
}

void IniSetting::readAll()
{
	if (QFileInfo::exists(m_strFileName) == true)
	{
		if (getScaleFlag(m_scaleFlag) == false)
		{
			m_scaleFlag = false;
		}

		if (getScaleFactor(m_scaleFactor) == false)
		{
			m_scaleFactor = 1.0;
		}
	}
	else
	{
		m_scaleFlag = false;
		m_scaleFactor = 1.0;
	}
	
}

void IniSetting::writeAll()
{
	setScaleFlag(m_scaleFlag);
	setScaleFactor(m_scaleFactor);
}

bool IniSetting::getScaleFlag(bool &autoScaleFlag)
{
	QString strValue = value(INI_SCALE_FLAG_SEGMENT, QString("NULL")).toString(); 
	bool returnVal = false;

	if (strValue == "true")
	{
		autoScaleFlag = true;
		returnVal = true;
	}
	else if (strValue == "false")
	{
		autoScaleFlag = false;
		returnVal = true;
	}

	return (returnVal);
}

void IniSetting::setScaleFlag(bool scaleFlag)
{
	QString scaleString;
	if (scaleFlag)
	{
		scaleString = "true";
	}
	else
	{
		scaleString = "false";
	}

	setValue(INI_SCALE_FLAG_SEGMENT, scaleString);
}

bool IniSetting::getScaleFactor(float &scaleFactor)
{
	QString strValue = value(INI_SCALE_FACTOR_SEGMENT, QString("0")).toString(); 
	bool returnVal = false;

	if (strValue != "0")
	{
		returnVal = true;
		scaleFactor = strValue.toFloat();
	}

	return (returnVal);
}

void IniSetting::setScaleFactor(float scaleFactor)
{
	setValue(INI_SCALE_FACTOR_SEGMENT, QString::number(scaleFactor, 'f', 4));
}

int IniSetting::getPID()
{
	QString strValue = value(INI_UID_SEGMENT, QString("0")).toString(); 

	return (strValue.toInt());
}

void IniSetting::setPID(int pid)
{
	setValue(INI_UID_SEGMENT, QString::number(pid));
}