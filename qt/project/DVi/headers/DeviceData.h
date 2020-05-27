#ifndef DEVICE_DATA_H
#define DEVICE_DATA_H

#include "PresetData.h"

class DeviceData : public QObject
{
	Q_OBJECT

public:
	DeviceData();

	bool loadFromJSON(QString presetDBFileName);
    bool saveToJSON(QString presetDBFileName);

	void setNetworkAddress(NETWORK_SETTING networkSetting) { m_networkSetting = networkSetting; }
	NETWORK_SETTING getNetworkAddress() { return m_networkSetting; }

	void setUSBStatus(bool flag) { m_usbConnectStatus = flag; }
	bool getUSBStatus() { return m_usbConnectStatus; }

	QString dumpGlobalSettings();

	int getCurrentPresetID()  { return m_currentPresetID; }
	void setCurrentPresetID(float currentPresetID)	 { m_currentPresetID = currentPresetID; }

	float getMasterGain1() { return m_masterGain1; }
	void setMasterGain1(float masterGain1) { m_masterGain1 = masterGain1; }

	PresetData * getCurrentPresetData() { return &m_presetData[0]; }

protected:
	void loadGlobalSettings(QString &globalContent);

private:

	/* Tab 0: Overview  */
	NETWORK_SETTING m_networkSetting;
	bool m_usbConnectStatus;

	/* Read from Global Setting */
	QString m_currentPresetName;
	int		m_currentPresetID;
	float	m_masterGain1;
	PresetData m_presetData[1 + TOTAL_PRESET_COUNT];
};

extern DeviceData *getDeviceData();
extern PresetData *getCurrentPreset();

#endif
