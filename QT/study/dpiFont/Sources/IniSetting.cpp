#include "IniSetting.h"
#include <QFileInfo>
#include <QMessageBox>

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
	QString strValue = value("ScaleSetting/ScaleFlag", QString("NULL")).toString(); 
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

	setValue("ScaleSetting/ScaleFlag", scaleString);
}

bool IniSetting::getScaleFactor(float &scaleFactor)
{
	QString strValue = value("ScaleSetting/ScaleFactor", QString("0")).toString(); 
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
	setValue("ScaleSetting/ScaleFactor", QString::number(scaleFactor, 'f', 4));
}
