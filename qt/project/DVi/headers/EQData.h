#ifndef EQ_DATA_H
#define EQ_DATA_H

#include "types.h"
#include <QString>
#include <QColor>
#include <QPainter>

#define MAX_EQ_FREQUENCY 20000
#define MIN_EQ_FREQUENCY 20
#define MAX_MAGNITUDE 20

class EQWidget;

typedef enum
{
	DRAW_FROM_HORIZON,
	DRAW_FROM_BOTTOM,
	DRAW_NONE,				/* Don't draw single curve, such as User Defined FIR */
} DRAW_CURVE_MODE;

class ControlPoint
{
public:
	ControlPoint() { m_frequency = 20.0; m_gain = 0.0; }
	ControlPoint(double freq, double gain) { m_frequency = freq;  m_gain = gain; }

	double m_frequency;
	double m_gain;
} ;

/* calc curve for PEQ: Peaking / Low-Shelf / High Shelf */
void curve_calculation(int eq_type, qreal eq_gain, qreal eq_fc, qreal q_or_slope, int horizontal_number, qreal *horizontal, qreal *vertical);


class EQData
{
public:
	EQData();

	virtual DRAW_CURVE_MODE getCurveDrawMode() = 0;
	virtual EQType getEQType() = 0;
	virtual int getControlPointCount() = 0;
	virtual bool hasMainControlPoint() = 0;				/* true: has main control point, and the index of main control point is 0;  false: no main control point */
	virtual ControlPoint getControlPoint(int index) = 0;
	virtual bool isInclude(ControlPoint controlPoint) = 0;
	virtual double dbCalc(double frequency) = 0;		/* Calculate the gain according to frequency */
    virtual void dump();
	virtual void dBCalc(int discreteCount, double *freq, double *gain) = 0;
	virtual void drawEQCurve(QPainter *painter, EQWidget *pEQWidget, int pixelCount, double *pGain, QColor &fillColor) = 0;
	virtual void drawVertexText(QPainter *painter, EQWidget *pEQWidget, QString &vertexText);

	void enable(bool enableFlag);
	bool isEnabled();

protected:
	bool m_enableFlag;

};

class FIREQ: public EQData
{
public:
	virtual DRAW_CURVE_MODE getCurveDrawMode() { return DRAW_NONE; }
	virtual EQType getEQType() { return EQ_USER_DEFINED_FIR; }
	virtual int getControlPointCount() { return 0;	}
	virtual bool hasMainControlPoint() { return false; }
	virtual ControlPoint getControlPoint(int index) { return ControlPoint(1000.0, 0.0); }
	virtual bool isInclude(ControlPoint controlPoint) {  return false; }
	virtual double dbCalc(double frequency) { return -1.5; }
    virtual void dump();
	virtual void dBCalc(int discreteCount, double *freq, double *gain);
	virtual void drawEQCurve(QPainter *painter, EQWidget *pEQWidget, int pixelCount, double *pGain, QColor &fillColor);

protected:
	QString m_content;
	int m_numberTap;
} ;

class BellEQ: public EQData
{
public:
	BellEQ(double frequency, double gain, double q, bool flag = true);
	virtual DRAW_CURVE_MODE getCurveDrawMode() { return DRAW_NONE; }
	virtual EQType getEQType() { return EQ_PEAKING; }
	virtual int getControlPointCount();
	virtual bool hasMainControlPoint();
	virtual ControlPoint getControlPoint(int index);
	virtual bool isInclude(ControlPoint controlPoint) {  return false; }
	virtual double dbCalc(double frequency) { return -1.5; }
    virtual void dump();
	virtual void dBCalc(int discreteCount, double *freq, double *gain);
	virtual void drawEQCurve(QPainter *painter, EQWidget *pEQWidget, int pixelCount, double *pGain, QColor &fillColor);
	virtual void drawVertexText(QPainter *painter, EQWidget *pEQWidget, QString &vertexText);

protected:
	double getLeftFreqForControlPoint();
	double getRightFreqForControlPoint();
	
protected:	
	double m_Q;
	double m_frequency;
	double m_gain;
};

class LShelfEQ: public EQData
{
public:
	virtual DRAW_CURVE_MODE getCurveDrawMode() { return DRAW_NONE; }
	virtual EQType getEQType() { return EQ_L_SHELF; }
	virtual int getControlPointCount() { return 0;	}
	virtual bool hasMainControlPoint() { return false; }
	virtual ControlPoint getControlPoint(int index) { return ControlPoint(1000, 0); }
	virtual bool isInclude(ControlPoint controlPoint) {  return false; }
	virtual double dbCalc(double frequency) { return -1.5; }
    virtual void dump();
	virtual void dBCalc(int discreteCount, double *freq, double *gain);
	virtual void drawEQCurve(QPainter *painter, EQWidget *pEQWidget, int pixelCount, double *pGain, QColor &fillColor);

protected:	
	double m_lsFrequency;
	double m_gain;
	double m_slope;
};

class HShelfEQ: public EQData
{
public:
	virtual DRAW_CURVE_MODE getCurveDrawMode() { return DRAW_NONE; }
	virtual EQType getEQType() { return EQ_H_SHELF; }
	virtual int getControlPointCount() { return 0;	}
	virtual bool hasMainControlPoint() { return false; }
	virtual ControlPoint getControlPoint(int index) { return ControlPoint(1000, 0); }
	virtual bool isInclude(ControlPoint controlPoint) {  return false; }
	virtual double dbCalc(double frequency) { return -1.5; }
    virtual void dump();
	virtual void dBCalc(int discreteCount, double *freq, double *gain);
	virtual void drawEQCurve(QPainter *painter, EQWidget *pEQWidget, int pixelCount, double *pGain, QColor &fillColor);

protected:	
	double m_hsfrequency;
	double m_gain;
	double m_slope;
};

class HPFEQ: public EQData
{
public:
	virtual DRAW_CURVE_MODE getCurveDrawMode() { return DRAW_NONE; }
	virtual EQType getEQType() { return EQ_HPF; }
	virtual int getControlPointCount() { return 0;	}
	virtual bool hasMainControlPoint() { return false; }
	virtual ControlPoint getControlPoint(int index) { return ControlPoint(1000, 0); }
	virtual bool isInclude(ControlPoint controlPoint) {  return false; }
	virtual double dbCalc(double frequency) { return -1.5; }
    virtual void dump();
	virtual void dBCalc(int discreteCount, double *freq, double *gain);
	virtual void drawEQCurve(QPainter *painter, EQWidget *pEQWidget, int pixelCount, double *pGain, QColor &fillColor);

protected:	
	double m_hpFrequency;
	double m_gain;
};

class LPFEQ: public EQData
{
public:
	virtual DRAW_CURVE_MODE getCurveDrawMode() { return DRAW_NONE; }
	virtual EQType getEQType() { return EQ_LPF; }
	virtual int getControlPointCount() { return 0;	}
	virtual bool hasMainControlPoint() { return false; }
	virtual ControlPoint getControlPoint(int index) { return ControlPoint(1000, 0); }
	virtual bool isInclude(ControlPoint controlPoint) {  return false; }
	virtual double dbCalc(double frequency) { return -1.5; }
    virtual void dump();
	virtual void dBCalc(int discreteCount, double *freq, double *gain);
	virtual void drawEQCurve(QPainter *painter, EQWidget *pEQWidget, int pixelCount, double *pGain, QColor &fillColor);

protected:	
	double m_lpFrequency;
	double m_gain;
};

class XOver:public EQData
{
public:
	virtual DRAW_CURVE_MODE getCurveDrawMode() { return DRAW_NONE; }
	virtual EQType getEQType() { return EQ_XOVER; }
	virtual int getControlPointCount() { return 0;	}
	virtual bool hasMainControlPoint() { return false; }
	virtual ControlPoint getControlPoint(int index) { return ControlPoint(1000, 0); }
	virtual bool isInclude(ControlPoint controlPoint) {  return false; }
	virtual double dbCalc(double frequency) { return -1.5; }
    virtual void dump();
	virtual void dBCalc(int discreteCount, double *freq, double *gain);
	virtual void drawEQCurve(QPainter *painter, EQWidget *pEQWidget, int pixelCount, double *pGain, QColor &fillColor);

protected:
	double m_lpFrequency, m_hpFrequency;
	double m_gain;
};


#define MAX_EQ_COUNT	 20
class EQGroup
{
public:
	EQGroup();
	EQGroup(int count);
	~EQGroup();
	void resetEQCount(int count);
    int getEQCount() { return m_EQCount; }

	EQData *getEQ(int index);
    void setEQ(int index, FIREQ EQ);
    void setEQ(int index, BellEQ EQ);
    void setEQ(int index, LShelfEQ EQ);
    void setEQ(int index, HShelfEQ EQ);
    void setEQ(int index, LPFEQ EQ);
    void setEQ(int index, HPFEQ EQ);
    void setEQ(int index, XOver EQ);
	bool getEnableAllFlag() { return m_enableAll; }
	void enableAll(bool flag)  {  m_enableAll = flag; }

    void dump();
protected:
	int m_EQCount;
	EQData *m_pEQDataArray[MAX_EQ_COUNT];
	bool m_enableAll;
} ;



#endif
