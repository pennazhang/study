#ifndef INI_SETTING_H
#define INI_SETTING_H

#include <QSettings>

class IniSetting : public QSettings
{
public:
	IniSetting(QString strFileName);

	void readAll();
	void writeAll();

	int getPID();
	void setPID(int pid);

protected:
	bool getScaleFlag(bool &ScaleFlag);
	void setScaleFlag(bool scaleFlag);

	bool getScaleFactor(float &scaleFactor);
	void setScaleFactor(float scaleFactor);

public:
	bool m_scaleFlag;
	float m_scaleFactor;
	QString m_strFileName;
};

extern IniSetting *g_pIniSetting;

#endif
