#ifndef INPUT_EQ_EDITOR_H
#define INPUT_EQ_EDITOR_H

#include <QDialog>
#include <QLabel>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QPushButton>
#include "EQWidget.h"

class InputEQEditor;

class InputEQWidget: public EQWidget
{
    Q_OBJECT
public:
	InputEQWidget(InputEQEditor *m_inputEQEditor, QWidget *parent);

	virtual int getEQCount();
	virtual int getCurrentEQIndex();
	virtual EQData *getEQ(int index);
	virtual EQData *getCurrentEQ();
	virtual bool getEnableAllFlag();
	virtual void clearFinalCurve();

	InputEQEditor *m_pInputEQEditor;
};

class InputEQEditor : public QDialog
{
    Q_OBJECT
public:
    InputEQEditor(QWidget *parent);
    int getEQCount();
	int getCurrentEQIndex()  {  return m_currentEQIndex;  }
	EQData *getCurrentEQ();
	void setCurrentEQIndex(int currentEQIndex) {  m_currentEQIndex = currentEQIndex;  }
	bool getEnableAllFlag();
	EQData *getEQ(int index);
signals:

protected slots:
//	void onClickScaleEnable();
//	void onFactorSliderChanged(int value);
//	void onFactorSpinChanged(double value);
//	void onApplyClicked();
//	void onCancelClicked();

protected:
	void resizeEvent(QResizeEvent * /* event */);
//	void checkApplyButton();

protected:
//	QCheckBox *m_pCheckBox;
//	QLabel *m_pZoomLable;
//	ButtonSlider *m_pButtonSlider;
//	QDoubleSpinBox *m_pDoubleSpinBox;
//	QPushButton *m_pApplyButton, *m_pCancelButton;
//	bool m_slotEnableFlag;

	int m_currentChannel;	/* Channel 0 or Channel 1 */
	int m_currentEQIndex;
	EQWidget *m_pEQWidget;
};

#endif

