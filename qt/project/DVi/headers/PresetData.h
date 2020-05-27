#ifndef PRESET_DATA_H
#define PRESET_DATA_H

#include <QWidget>
#include <QSettings>
#include <QHostAddress>
#include <QJsonDocument>
#include <QJsonObject>
//#include "OutputChannel.h"
#include "types.h"
#include <QTime>
#include <QDate>
#include "EQData.h"
#include <QString>

#define MAX_EQ_FREQUENCY 20000
#define MIN_EQ_FREQUENCY 20
#define MAX_MAGNITUDE 20

#define TOTAL_PRESET_COUNT  100

typedef enum
{
	SIGNAL_OFF = 0,
	SIGNAL_100Hz,
	SIGNAL_1kHz,
	SIGNAL_10kHz,
	SIGNAL_PINK_NOISE,
} SIGNAL_MODE;

typedef enum
{
	RTA_FULL = 0,
	RTA_X_CURVE
} RTA_MODE;

QString formatFreq(qreal freq);
QString formatGain(qreal gain);

typedef struct
{
	bool m_dhcpEnable;
	QHostAddress m_ipAddress, m_networkMask, m_ipGateway;
} NETWORK_SETTING;


typedef struct
{
	bool		m_enableFlag;
	bool		m_autoFlag;
	float		m_threshold;		/* unit 0.1 dB */
	float		m_attackTimeInMs;	/* unit 0.05ms  */
	float		m_holdTimeInMs;		/* unit 0.05 ms */
	int			m_release;			/* unit 10 dB/s */
} LIMITER;

class PresetData : public QObject
{
	Q_OBJECT
public:
	PresetData();
	void loadPreset(QString &presetName, QString &presetContent);
    void dump();

	void setInvalidFlag(bool validFlag);
	bool isInvalid();

    EQGroup *getInputEQGroup() { return m_inPEQGroup; }

protected:
	QString m_presetName;
	bool m_validFlag;

	bool m_outputBridgeMode;		/* true:  Bridge Mode,   false: Steoro Mode */
	int	m_inputSource[2];			/* input mode for channel 1 and channel 2 */
	double m_gain[2];				/* Gain for channel 1 and channel 2 */
	bool	m_muteFlag[2];			/* Mute Flag for channel 1 and channel 2 */
	double m_delayInMS[2];			/* delay in ms for channel 1 and channel 2 */
	bool	m_polarityNormalFlag[2];/* Polarity flag for channel 1 and channel 2. true: Normal;  false: Inverse */
	FIREQ m_fir[2];					/* FIR for channel 1 and channel 2 */
	EQGroup m_inPEQGroup[2];		/* 8 PEQs for channel 1 and channel 2 */
	EQGroup m_outPEQGroup[2];		/* 8 PEQs for channel 1 and channel 2 */
	bool m_inPEQEnableAll[2];		/* PEQ Enable all for channel1 and channel 2 */
	LIMITER m_limiter[2];			/* Limiter for channel 1 and channel 2 */
	XOver m_xover[2];				/* XOver for channel 1 and channel 2 */
};


#endif
