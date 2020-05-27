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
#include "FormatOutput.h"

PresetData::PresetData()
{
	m_validFlag = false;

	for (int channelIndex = 0; channelIndex < 2; channelIndex++)
	{
		m_inPEQGroup[channelIndex].enableAll(true);
		m_inPEQGroup[channelIndex].resetEQCount(5);

		m_inPEQGroup[channelIndex].setEQ(0, BellEQ(30.0, -0.5, 2.0));
		m_inPEQGroup[channelIndex].setEQ(1, BellEQ(100.0, -1.5, 2.0));
		m_inPEQGroup[channelIndex].setEQ(2, BellEQ(300.0, -2.5, 2.0));
		m_inPEQGroup[channelIndex].setEQ(3, BellEQ(1000.0, -3.5, 2.0));
		m_inPEQGroup[channelIndex].setEQ(4, BellEQ(3000.0, -4.5, 2.0));
	}
}

void PresetData::loadPreset(QString &presetName, QString &presetContent)
{
	m_validFlag = true;

	const JSON_STRING_TABLE stringTable[] = 
	{
		/* For TAB 0: Overview */
//		{ JSON_ENUM,			"Preset/SourceRouter/InputSource"	, &inputSource,			g_inputSourceTable, },
//		{ JSON_STRING,			"Node/RPCserver/LoadedPresetName"		, &m_currentPresetName,		nullptr,		QVariant("Unknown")	},
		{ JSON_INT,				"Node/RPCserver/LoadedPresetID"			, &m_inputSource,		nullptr,		QVariant(0)			},
//		{ JSON_FLOAT,			"Preset/PeakMeters/Channel_1_LeakRate"	, &m_masterGain1,			nullptr,		QVariant(-0.1)		},
//		{ JSON_BOOL,			"Preset/Fader/Mute"							, &m_masterOutputMuteFlag ,  nullptr, 0   },
//		{ JSON_INT,				"Preset/Fader/FadeIn"						, &fadeIn  ,  nullptr, 0   },

//		{ JSON_BOOL,			"Preset/SourceRouter/PhantomPower"		, &m_phantomEnable  ,  nullptr, 0   },
	};

	QJsonDocument loadDoc(QJsonDocument::fromJson(presetContent.toUtf8()));
	QJsonObject mainObject = loadDoc.object();
	readJsonTable(mainObject, (JSON_STRING_TABLE *)stringTable, sizeof(stringTable) / sizeof(JSON_STRING_TABLE));
	m_presetName = presetName;

    FormatOutput::setTextStream(nullptr);
    this->dump();
}

void PresetData::dump()
{
	if (m_validFlag == false)
	{
		FormatOutput::dump("flag = Invalid");
		return;
	}

	JSON_KEY_STRING aDumpNode[] = 
	{
		{"Name",				m_presetName	},
	};

    for (ULONG i = 0; i < sizeof(aDumpNode) / sizeof(JSON_KEY_STRING); i++)
	{
		FormatOutput::dump(QString("%1 = %2").arg(aDumpNode[i].m_strKeyList).arg(aDumpNode[i].m_valueString));
	}

	/* Input PEQ for Channel 1 */
	FormatOutput::dump("Input PEQ for Channel 1");
	FormatOutput::incDumpLevel();
    m_inPEQGroup[0].dump();
	FormatOutput::decDumpLevel();
}

void PresetData::setInvalidFlag(bool validFlag) 
{
	m_validFlag = validFlag;
}

bool PresetData::isInvalid() 
{
	return (m_validFlag);
}

QString formatFreq(qreal freq)
{
	if (freq < 1000)
	{
		return (QString("%1Hz").arg((int)freq));
	}
	else
	{
		return (QString::number(freq / 1000, 'f', 2) + QString("kHz"));
	}
}

QString formatGain(qreal gain)
{
	return (QString::number(gain, 'f', 1) + QString("dB"));
}
