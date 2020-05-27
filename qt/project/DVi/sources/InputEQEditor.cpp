#include "InputEQEditor.h"
#include "commonLib.h"
#include "DeviceData.h"


InputEQWidget::InputEQWidget(InputEQEditor *pSpeakerEditor, QWidget *parent) : EQWidget(parent)
{
	m_pInputEQEditor = pSpeakerEditor;
}

int InputEQWidget::getEQCount()
{
    return m_pInputEQEditor->getEQCount();
}

int InputEQWidget::getCurrentEQIndex()
{
	return m_pInputEQEditor->getCurrentEQIndex();
}

EQData * InputEQWidget::getEQ(int index)
{
	return m_pInputEQEditor->getEQ(index);
}

EQData * InputEQWidget::getCurrentEQ()
{
	return m_pInputEQEditor->getCurrentEQ();
}

bool InputEQWidget::getEnableAllFlag()
{
	return m_pInputEQEditor->getEnableAllFlag();
}

void InputEQWidget::clearFinalCurve()
{
	EQWidget::clearFinalCurve();

#if 0
	qreal pVertical[4000];
	SpeakerData *pSpeakerData = m_pSpeakerEditor->getSpeakerData();
	if (pSpeakerData->m_speakerType == SPEAKER_TYPE_SUBWOOFER)
	{
		EQData EQ;
		EQ.setType(EQ_LPF);
		EQ.setFrequency(283);
		EQ.setPFType(PF_IIR);
		EQ.setPFIIRType(PF_IIR_BW24);
		EQ.enable(true);

		EQ.dBCalc(m_discreteCount, m_pDimensionX, pVertical);
		addToTotalCurve(pVertical);
	}
#endif
}

InputEQEditor::InputEQEditor(QWidget *parent)	
	: QDialog(parent)
{
	m_pEQWidget = new InputEQWidget(this, this);

	setMinimumSize(900, 700);
    resize(900, 700);

	m_currentChannel = 0;
	m_currentEQIndex = 0;
}

void InputEQEditor::resizeEvent(QResizeEvent * /*event*/)
{
	::setGeometry(m_pEQWidget, 0.015, 0.02, 0.97, 0.58);
}

int InputEQEditor::getEQCount()
{
    PresetData *pCurrentPreset = getCurrentPreset();

    EQGroup *pInputEQGroup = pCurrentPreset->getInputEQGroup();
    return (pInputEQGroup[m_currentChannel].getEQCount());

}

EQData *InputEQEditor::getCurrentEQ()
{
    PresetData *pCurrentPreset = getCurrentPreset();

    EQGroup *pInputEQGroup = pCurrentPreset->getInputEQGroup();
	return (pInputEQGroup[m_currentChannel].getEQ(m_currentEQIndex));
}

bool InputEQEditor::getEnableAllFlag()
{
    PresetData *pCurrentPreset = getCurrentPreset();

    EQGroup *pInputEQGroup = pCurrentPreset->getInputEQGroup();
    return (pInputEQGroup[m_currentChannel].getEnableAllFlag());
}

EQData *InputEQEditor::getEQ(int index)
{
    PresetData *pCurrentPreset = getCurrentPreset();

    EQGroup *pInputEQGroup = pCurrentPreset->getInputEQGroup();
	return (pInputEQGroup[m_currentChannel].getEQ(index));
}
