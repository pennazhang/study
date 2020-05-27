#include "DeviceData.h"
#include <QFile>
#include "SQLiteDB.h"
#include <QJsonObject>
#include "DeviceSocket.h"
#include "mainApplication.h"
#include "commonLib.h"
#include "simpleQtLogger.h"
#include "JsonTable.h"
#include <QFileInfo>

DeviceData g_DeviceData;

PresetData *getCurrentPreset()
{
	return (g_DeviceData.getCurrentPresetData());
}

DeviceData *getDeviceData()
{
	return (&g_DeviceData);
}

DeviceData::DeviceData()
{

}

QString DeviceData::dumpGlobalSettings()
{
	QString ret;

	JSON_KEY_STRING aDumpNode[] = 
	{
		{"Current Preset Name",		m_currentPresetName						},
		{"Current Preset ID",		QString::number(m_currentPresetID)		},
		{"Master Gain 1",			QString::number(m_masterGain1, 'f', 4)	},
	};

    for (ULONG i = 0; i < sizeof(aDumpNode) / sizeof(JSON_KEY_STRING); i++)
	{
		ret += QString("%1 = %2").arg(aDumpNode[i].m_strKeyList, 40).arg(aDumpNode[i].m_valueString);
	}
	return (ret);
}

void DeviceData::loadGlobalSettings(QString &globalContent)
{
	const JSON_STRING_TABLE stringTable[] = 
	{
		/* For TAB 0: Overview */
//		{ JSON_ENUM,			"Preset/SourceRouter/InputSource"	, &inputSource,			g_inputSourceTable, },
		{ JSON_STRING,			"Node/RPCserver/LoadedPresetName"		, &m_currentPresetName,		nullptr,		QVariant("Unknown")	},
		{ JSON_INT,				"Node/RPCserver/LoadedPresetID"			, &m_currentPresetID,		nullptr,		QVariant(0)			},
		{ JSON_FLOAT,			"Preset/PeakMeters/Channel_1_LeakRate"	, &m_masterGain1,			nullptr,		QVariant(-0.1)		},
//		{ JSON_BOOL,			"Preset/Fader/Mute"							, &m_masterOutputMuteFlag ,  nullptr, 0   },
//		{ JSON_INT,				"Preset/Fader/FadeIn"						, &fadeIn  ,  nullptr, 0   },

//		{ JSON_BOOL,			"Preset/SourceRouter/PhantomPower"		, &m_phantomEnable  ,  nullptr, 0   },
	};

	QJsonDocument loadDoc(QJsonDocument::fromJson(globalContent.toUtf8()));
	QJsonObject mainObject = loadDoc.object();
	readJsonTable(mainObject, (JSON_STRING_TABLE *)stringTable, sizeof(stringTable) / sizeof(JSON_STRING_TABLE));
	qDebug() << dumpGlobalSettings();
}

bool DeviceData::loadFromJSON(QString fileName)
{
	/* Step1: Load Global Settings */
	QString globalContent;
	if (g_pDatabase->getGlobalString(fileName, globalContent) == false)
	{
		L_ERROR("loadFromJSON failed! fileName = " + fileName);
		return (false);
	}

    if (globalContent.length() == 0)
	{
		L_ERROR(QString("global content of %1 is NULL").arg(fileName));
		return (false);
	}
	loadGlobalSettings(globalContent);

	/* Step2: Load Speaker */
	QString presetName;
	QString presetContent;

	for (int i = 0; i < 1 + TOTAL_PRESET_COUNT; i++)
	{
		if (g_pDatabase->getPreset(fileName, i, presetName, presetContent) == false)
		{
//			L_ERROR(QString("Load preset%1 failed!").arg(i));
			m_presetData[i].setInvalidFlag(false);
			continue;
		}

		if (presetContent.length() == 0)
		{
			L_ERROR(QString("current preset %1:%2 content is NULL").arg(i).arg(presetName));
			return (false);
		}
		else
		{
			qDebug() << QString("Load preset%1:%2 success").arg(i).arg(presetName).arg(presetContent);
			m_presetData[i].loadPreset(presetName, presetContent);	
		}

	}
    return true;
}


bool DeviceData::saveToJSON(QString fileName) 
{
	QString localPresetFileName = g_pApp->getTempPresetDBFileName();
	if (QFileInfo::exists(localPresetFileName) == false)
	{
		g_pDatabase->closeDB();				/* We must close the Database before copy file */
		QFile::remove(localPresetFileName);
		QFile::copy(QString(":/binaries/FactoryPresets.db"), localPresetFileName);
	}
	QFile::setPermissions(localPresetFileName, QFileDevice::WriteOwner | QFileDevice::WriteUser | QFileDevice::WriteGroup | QFileDevice::WriteOther | QFileDevice::ReadOwner | QFileDevice::ReadUser | QFileDevice::ReadGroup | QFileDevice::ReadOther);

	QString content;
	if (g_pDatabase->getGlobalString(localPresetFileName, content) == false)
    if (content.length() == 0)
	{
		L_ERROR(QString("Failed to load global setting from: %1").arg(localPresetFileName));
		return (false);
	}

	QJsonDocument loadDoc(QJsonDocument::fromJson(content.toUtf8()));
	QJsonObject mainObject = loadDoc.object();

	QString debugKey = QString("Preset/SourceRouter/MicChannelAssign");
	qDebug() << readObject(mainObject, debugKey);

	JSON_KEY_STRING stringTable[] = 
	{
		{ "Node/RPCserver/LoadedPresetName",		m_currentPresetName							},
		{ "Node/RPCserver/LoadedPresetID",			QString::number(m_currentPresetID)			},
		{ "Preset/PeakMeters/Channel_1_LeakRate",	QString::number(m_masterGain1, 'f', 3)		},

#if 0
		/* For TAB 0: Overview */
		{ "Preset/SourceRouter/InputSource"	, getTableString(g_inputSourceTable, m_inputSource) },
		{ "Node/.rootSVs/Theater"			, m_strTheaterName},
		{ "Node/.rootSVs/Hall"				, m_strHallName},
		{ "Node/.rootSVs/Server"			, m_serverName},
		{ "Node/.rootSVs/Projector"			, m_projectorName},
		{ "Node/.rootSVs/SpeakerSettingL"	, m_speakerName[0]},
		{ "Node/.rootSVs/SpeakerSettingR"	, m_speakerName[1]},
		{ "Node/.rootSVs/SpeakerSettingC"	, m_speakerName[2]},
		{ "Node/.rootSVs/SpeakerSettingLFE"	, m_speakerName[3]},
		{ "Node/.rootSVs/SpeakerSettingLS"	, m_speakerName[4]},
		{ "Node/.rootSVs/SpeakerSettingRS"	, m_speakerName[5]},
		{ "Node/.rootSVs/SpeakerSettingBLS"	, m_speakerName[6]},
		{ "Node/.rootSVs/SpeakerSettingBRS"	, m_speakerName[7]},
		{ "Node/.rootSVs/Amplifier_L"		, m_amplifierName[0]},
		{ "Node/.rootSVs/Amplifier_R"		, m_amplifierName[1]},
		{ "Node/.rootSVs/Amplifier_C"		, m_amplifierName[2]},
		{ "Node/.rootSVs/Amplifier_LFE"		, m_amplifierName[3]},
		{ "Node/.rootSVs/Amplifier_LS"		, m_amplifierName[4]},
		{ "Node/.rootSVs/Amplifier_RS"		, m_amplifierName[5]},
		{ "Node/.rootSVs/Amplifier_BLS"		, m_amplifierName[6]},
		{ "Node/.rootSVs/Amplifier_BRS"		, m_amplifierName[7]},

		/* For TAB 1: Basic Setting */
		{ "Preset/MasterVol/GainPreset"		, QString::number(masterVolumeToDB(getMasterVolumePreset()), 'f', 1) + QString("dB")},
//		{ "Preset/MasterVol/Gain"			, QString::number(masterVolumeToDB(getMasterVolume()), 'f', 1) + QString("dB")},
		{ "Preset/Fader/Mute"				, getTableString(g_boolTable, m_masterOutputMuteFlag)},
		{ "Preset/Fader/FadeIn"				, QString::number(qRound(m_fadeIn * 1000), 10) },
		{ "Preset/Fader/FadeOut"			, QString::number(qRound(m_fadeOut * 1000), 10) },
		{ "Node/.rootSVs/AudioInputType"	, getTableString(g_AudioOutputTable, (int)m_speakerNumber)},		
		{ "Node/.rootSVs/FaultMask"			, QString::number(m_detectionMask, 10) },
		{ "Node/.rootSVs/DistanceL"			, QString::number(m_hallLength, 'f', 1) },
		{ "Node/.rootSVs/DistanceW"			, QString::number(m_hallWidth, 'f', 1) },

		{ "Preset/InDelay/Channel_5_Amount"			, QString("%1ms/%2ft/%3m").arg(m_surroundDelay).arg(1.127 * m_surroundDelay).arg(0.343 * m_surroundDelay) },
		{ "Preset/InDelay/Channel_6_Amount"			, QString("%1ms/%2ft/%3m").arg(m_surroundDelay).arg(1.127 * m_surroundDelay).arg(0.343 * m_surroundDelay) },
		{ "Preset/InDelay/Channel_7_Amount"			, QString("%1ms/%2ft/%3m").arg(m_surroundDelay).arg(1.127 * m_surroundDelay).arg(0.343 * m_surroundDelay) },
		{ "Preset/InDelay/Channel_8_Amount"			, QString("%1ms/%2ft/%3m").arg(m_surroundDelay).arg(1.127 * m_surroundDelay).arg(0.343 * m_surroundDelay) },

		/* For TAB2: Input Setting */
		{ "Preset/SourceRouter/DigitalGain"	, QString::number(getDigitalVolume(), 'f', 1) + QString("dB")},
		{ "Preset/SourceRouter/AnalogGain"	, QString::number(getAnalogVolume(), 'f', 1) + QString("dB")},	
		{ "Preset/SourceRouter/MicGain"		, QString::number(getMicVolume(), 'f', 1) + QString("dB")},	
		{ "Preset/SourceRouter/MusicGain"	, QString::number(getMusicVolume(), 'f', 1) + QString("dB")},	

		{ "Preset/SourceRouter/DigitalMute"	, getTableString(g_boolTable, m_digitalMute)},
		{ "Preset/SourceRouter/AnalogMute"	, getTableString(g_boolTable, m_analogMute)},
		{ "Preset/SourceRouter/MicMute"		, getTableString(g_boolTable, m_micMute)},
		{ "Preset/SourceRouter/MusicMute"	, getTableString(g_boolTable, m_musicMute)},

		{ "Preset/SourceRouter/DigitalDelay", getTableString(g_boolTable, m_digitalDelayEnable)},
		{ "Preset/SourceRouter/AnalogDelay"	, getTableString(g_boolTable, m_analogDelayEnable)},
		{ "Preset/SourceRouter/MicDelay"	, getTableString(g_boolTable, m_micDelayEnable)},
		{ "Preset/SourceRouter/MusicDelay"	, getTableString(g_boolTable, m_musicDelayEnable)},

		{ "Preset/SourceRouter/DigitalDelayAmount", QString::number(m_digitalDelay, 10) + "ms/" + QString::number(m_digitalDelay * 1.127, 'f', 2) + "ft/" +  QString::number(m_digitalDelay * 0.3432, 'f', 2) + "m" },
		{ "Preset/SourceRouter/AnalogDelayAmount",	QString::number(m_analogDelay, 10) + "ms/" + QString::number(m_analogDelay * 1.127, 'f', 2) + "ft/" +  QString::number(m_analogDelay * 0.3432, 'f', 2) + "m" },
		{ "Preset/SourceRouter/MicDelayAmount",		QString::number(m_micDelay, 10) + "ms/" + QString::number(m_micDelay * 1.127, 'f', 2) + "ft/" +  QString::number(m_micDelay * 0.3432, 'f', 2) + "m" },
		{ "Preset/SourceRouter/MusicDelayAmount",	QString::number(m_musicDelay, 10) + "ms/" + QString::number(m_musicDelay * 1.127, 'f', 2) + "ft/" +  QString::number(m_musicDelay * 0.3432, 'f', 2) + "m" },

		{ "Preset/SourceRouter/MicChannelAssign",	getTableString(g_micChannelTable, m_micChannel)},
		{ "Preset/SourceRouter/MusicChannelAssign", QString::number(m_musicChannel, 10) }, 
		{ "Preset/SourceRouter/PhantomPower",		getTableString(g_boolTable, m_phantomEnable)},
		{ "Preset/SourceRouter/DigitalChannel",		QString("%1 %2 %3 %4 %5 %6 %7 %8").arg(m_DigitalChannel[0]).arg(m_DigitalChannel[1]).arg(m_DigitalChannel[2]).arg(m_DigitalChannel[3]).arg(m_DigitalChannel[4]).arg(m_DigitalChannel[5]).arg(m_DigitalChannel[6]).arg(m_DigitalChannel[7]) },
		{ "Preset/SourceRouter/AnalogChannel",		QString("%1 %2 %3 %4 %5 %6 %7 %8").arg(m_AnalogChannel[0]).arg(m_AnalogChannel[1]).arg(m_AnalogChannel[2]).arg(m_AnalogChannel[3]).arg(m_AnalogChannel[4]).arg(m_AnalogChannel[5]).arg(m_AnalogChannel[6]).arg(m_AnalogChannel[7]) },

		/* For TAB3: Output Setting - Rool Level */
		{ "Preset/InputGains/Channel_1_Mute",	getTableString(g_boolTable, m_pOutputChannel[0].m_roomLevelMute)},
		{ "Preset/InputGains/Channel_2_Mute",	getTableString(g_boolTable, m_pOutputChannel[1].m_roomLevelMute)},
		{ "Preset/InputGains/Channel_3_Mute",	getTableString(g_boolTable, m_pOutputChannel[2].m_roomLevelMute)},
		{ "Preset/InputGains/Channel_4_Mute",	getTableString(g_boolTable, m_pOutputChannel[3].m_roomLevelMute)},
		{ "Preset/InputGains/Channel_5_Mute",	getTableString(g_boolTable, m_pOutputChannel[4].m_roomLevelMute)},
		{ "Preset/InputGains/Channel_6_Mute",	getTableString(g_boolTable, m_pOutputChannel[5].m_roomLevelMute)},
		{ "Preset/InputGains/Channel_7_Mute",	getTableString(g_boolTable, m_pOutputChannel[6].m_roomLevelMute)},
		{ "Preset/InputGains/Channel_8_Mute",	getTableString(g_boolTable, m_pOutputChannel[7].m_roomLevelMute)},

		{ "Preset/InputGains/Channel_1_Gain",	QString::number(m_pOutputChannel[0].m_roomLevelGain, 'f', 1) + QString("dB")},
		{ "Preset/InputGains/Channel_2_Gain",	QString::number(m_pOutputChannel[1].m_roomLevelGain, 'f', 1) + QString("dB")},
		{ "Preset/InputGains/Channel_3_Gain",	QString::number(m_pOutputChannel[2].m_roomLevelGain, 'f', 1) + QString("dB")},
		{ "Preset/InputGains/Channel_4_Gain",	QString::number(m_pOutputChannel[3].m_roomLevelGain, 'f', 1) + QString("dB")},
		{ "Preset/InputGains/Channel_5_Gain",	QString::number(m_pOutputChannel[4].m_roomLevelGain, 'f', 1) + QString("dB")},
		{ "Preset/InputGains/Channel_6_Gain",	QString::number(m_pOutputChannel[5].m_roomLevelGain, 'f', 1) + QString("dB")},
		{ "Preset/InputGains/Channel_7_Gain",	QString::number(m_pOutputChannel[6].m_roomLevelGain, 'f', 1) + QString("dB")},
		{ "Preset/InputGains/Channel_8_Gain",	QString::number(m_pOutputChannel[7].m_roomLevelGain, 'f', 1) + QString("dB")},

		{ "Preset/InputGains/Channel_4_Polarity",	getTableString(g_LFEInverseTable, m_swConfiguration.m_inverseFlag)},	
		{ "Preset/InPeq/Channel_4_Band_1_Q",	QString::number(m_pOutputChannel[3].m_lShelf.getQ(), 'f', 1)},
		{ "Preset/InPeq/Channel_4_Band_2_Q",	QString::number(m_pOutputChannel[3].m_hShelf.getQ(), 'f', 1)},
		{ "Preset/SourceRouter/SignalCenterMode",	getTableString(g_boolTable, m_swConfiguration.m_centerGenFlag)},	
#endif
	};

    for (ULONG i = 0; i < sizeof(stringTable) / sizeof(JSON_KEY_STRING); i++)
	{
		modifyJsonValue(loadDoc, stringTable[i].m_strKeyList, stringTable[i].m_valueString);
	}

#if 0
	/* Let's load GEQ Here */
	{
		QStringList GEQspeakerList;
		GEQspeakerList << "InGeq_L" << "InGeq_R" << "InGeq_C" << "InGeq_LFE" << "InGeq_LS" << "InGeq_RS" << "InGeq_BLS" << "InGeq_BRS";

		QStringList freqList;
		freqList	<< "40 Hz" << "50 Hz" << "63 Hz" 
					<< "80 Hz" << "100 Hz" << "125 Hz" 
					<< "160 Hz" << "200 Hz" << "250 Hz"
					<< "315 Hz" << "400 Hz" << "500 Hz" 
					<< "630 Hz"	<< "800 Hz" << "1.0 kHz" 
					<< "1.25 kHz" << "1.6 kHz" << "2.0 kHz" 
					<< "2.5 kHz" << "3.15 kHz" << "4.0 kHz" 
					<< "5.0 kHz" << "6.3 kHz" << "8.0 kHz"
					<< "10.0 kHz" << "12.5 kHz" << "16.0 kHz";

		/* We will get float value here... */
		for (int speakerIndex = 0; speakerIndex < 8; speakerIndex++)
		{
			QString JSONKey;
			qDebug() << "***************************";

			if (speakerIndex == 3)
			{
				continue;
			}

			/* Get GEQ Value */
			for (int freqIndex = 0; freqIndex < 27; freqIndex++)
			{
				JSONKey = QString("Preset/%1/%2").arg(GEQspeakerList.at(speakerIndex)).arg(freqList.at(freqIndex));
				qDebug() << JSONKey;
				modifyJsonValue(loadDoc, JSONKey, QString::number(getGEQ(speakerIndex, freqIndex), 'f', 1) + QString("dB"));
			}

			/* Get PEQ- Low Shelf Filter Value */
			EQData *pEQ = m_pOutputChannel[speakerIndex].getLShelf();

			JSONKey = QString("Preset/InPeq/Channel_%1_Band_1_Frequency").arg(speakerIndex + 1);
			modifyJsonValue(loadDoc, JSONKey, formatFreq(pEQ->getFrequency()));

			JSONKey = QString("Preset/InPeq/Channel_%1_Band_1_Gain").arg(speakerIndex + 1);
			modifyJsonValue(loadDoc, JSONKey, formatGain(pEQ->getGain()));
		
			JSONKey = QString("Preset/InPeq/Channel_%1_Band_1_Slope").arg(speakerIndex + 1);
			modifyJsonValue(loadDoc, JSONKey, QString::number(pEQ->getSlope(), 'f', 2));

			/* Get PEQ- High Shelf Filter Value */
			pEQ = m_pOutputChannel[speakerIndex].getHShelf();
		
			JSONKey = QString("Preset/InPeq/Channel_%1_Band_2_Frequency").arg(speakerIndex + 1);
			modifyJsonValue(loadDoc, JSONKey, formatFreq(pEQ->getFrequency()));

			JSONKey = QString("Preset/InPeq/Channel_%1_Band_2_Gain").arg(speakerIndex + 1);
			modifyJsonValue(loadDoc, JSONKey, formatGain(pEQ->getGain()));
		
			JSONKey = QString("Preset/InPeq/Channel_%1_Band_2_Slope").arg(speakerIndex + 1);
			modifyJsonValue(loadDoc, JSONKey, QString::number(pEQ->getSlope(), 'f', 2));
		}
	}
#endif

	mainObject = loadDoc.object();
	QByteArray byte_array = QJsonDocument(mainObject).toJson();
    QString newContent(byte_array);
    bool ret = g_pDatabase->setGlobalString(localPresetFileName, newContent);
	if (ret == true)
	{
		qDebug() << QString(byte_array);
		QFile::remove(fileName);
		QFile::copy(localPresetFileName, fileName);
	}

	return (ret);
}
