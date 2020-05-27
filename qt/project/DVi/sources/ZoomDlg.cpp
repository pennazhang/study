#include "ZoomDlg.h"
#include "IniSetting.h"
#include <QMessageBox>
#include <QDebug>
#include "commonLib.h"

ZoomDlg::ZoomDlg(QWidget *parent)	
	: QDialog(parent)
{
	m_pCheckBox = new QCheckBox(tr("Enable Scale"), this);
	m_pCheckBox->setChecked(g_pIniSetting->m_scaleFlag);
	connect(m_pCheckBox, SIGNAL(clicked()), this, SLOT(onClickScaleEnable()));

	m_pZoomLable = new QLabel(tr("Scale Factor:"), this);
	m_pButtonSlider = new ButtonSlider(this, Qt::Horizontal);
	m_pButtonSlider->setTextColor(QColor(0x10, 0x10, 0x10));
	m_pButtonSlider->setMarkColor(QColor(0x10, 0x10, 0x10));
	QStringList analogStringList;
	analogStringList << "0.8" << "0.9"<<"1.0" << "1.1" << "1.2";
	m_pButtonSlider->setMark(5, 10, analogStringList);
	m_pButtonSlider->setMarkLength(5, 7);
	m_pButtonSlider->setMarkSide(true, false);
	m_pButtonSlider->setRange(80, 120);
	m_pButtonSlider->setGrooveHeight(30);
	m_pButtonSlider->setSingleStep(1);
	m_pButtonSlider->setPageStep(1);
	m_pButtonSlider->setValue(100);
	connect(m_pButtonSlider, SIGNAL(valueChanged(int)), this, SLOT(onFactorSliderChanged(int)));

	m_pDoubleSpinBox = new QDoubleSpinBox(this);
	m_pDoubleSpinBox->setRange(0.8, 1.2);
	m_pDoubleSpinBox->setSingleStep(0.01);
	m_pDoubleSpinBox->setDecimals(2);
	m_pDoubleSpinBox->setSuffix("");
	m_pDoubleSpinBox->setValue(1.0);
	connect(m_pDoubleSpinBox, SIGNAL(valueChanged(double)), this, SLOT(onFactorSpinChanged(double)));

	m_pApplyButton = new QPushButton(tr("Apply"), this);
	connect(m_pApplyButton, SIGNAL(clicked()), this, SLOT(onApplyClicked()));	

	m_pCancelButton = new QPushButton(tr("Cancel"), this);
	connect(m_pCancelButton, SIGNAL(clicked()), this, SLOT(onCancelClicked()));	

	m_slotEnableFlag = true;

	setFixedSize(338, 254);
}

void ZoomDlg::resizeEvent(QResizeEvent * /*event*/)
{
	int leftMargin = 50;
	m_pCheckBox->setGeometry(leftMargin, 40, 131, 25);
	m_pZoomLable->setGeometry(leftMargin, 80, 100, 20);
	m_pDoubleSpinBox->setGeometry(151, 80, 81, 22);
	m_pButtonSlider->setGeometry(leftMargin, 120, 241, 60);
	m_pApplyButton->setGeometry(60, 180, 75, 25);
	m_pCancelButton->setGeometry(210, 180, 75, 25);
}

void ZoomDlg::onClickScaleEnable()
{
	bool enableFlag = m_pCheckBox->isChecked();
	m_pDoubleSpinBox->setEnabled(enableFlag);
	m_pButtonSlider->setEnabled(enableFlag);

	checkApplyButton();
}

void ZoomDlg::checkApplyButton()
{
	bool enableFlag = m_pCheckBox->isChecked();
	if (enableFlag != g_pIniSetting->m_scaleFlag)
	{
		m_pApplyButton->setEnabled(true);
	}
	else if ((enableFlag == true) && (m_pButtonSlider->value() != 100))
	{
		m_pApplyButton->setEnabled(true);
	}
	else
	{
		m_pApplyButton->setEnabled(false);
	}
}

void ZoomDlg::onFactorSliderChanged(int value)
{
	if (m_slotEnableFlag == false)
	{
		return;
	}

	checkApplyButton();

	m_slotEnableFlag = false;
	m_pDoubleSpinBox->setValue((double)value / 100);
	m_slotEnableFlag = true;
}

void ZoomDlg::onFactorSpinChanged(double value)
{
	if (m_slotEnableFlag == false)
	{
		return;
	}

	m_slotEnableFlag = false;
	m_pButtonSlider->setValue(qRound((float)value * 100));
	m_slotEnableFlag = true;

	checkApplyButton();
}

void ZoomDlg::onCancelClicked()
{
	reject();
}

void ZoomDlg::onApplyClicked()
{
	int ret = QMessageBox::question(this, tr("Warning"), tr("You are changing the display scale settings, and this may lead to abnormal display of the GUI application. If the display of GUI application is abnormal, please delete the file: %1 manually.\nDo you want to continue?").arg(g_pIniSetting->m_strFileName).replace("/", "\\"), QMessageBox::Yes | QMessageBox::No);
	if (ret == QMessageBox::Yes)
	{
		g_pIniSetting->m_scaleFlag = m_pCheckBox->isChecked();
		g_pIniSetting->m_scaleFactor *= m_pDoubleSpinBox->value();
		g_pIniSetting->writeAll();
		QMessageBox::information(this, tr("Information"), tr("The scale settings will take effect atftr you restart the GUI applicaiton."));
		accept();
	}
}

OptionDlg::OptionDlg(QWidget *parent)	
	: QDialog(parent)
{
	m_pOKButton = new QPushButton(tr("Ok"), this);

	m_pDebugEnableBox = new QCheckBox(tr("Enable Debug"), this);
	m_pDebugEnableBox->setChecked(g_debugEnable);

	int w = 250;
	int h = 160;
	setFixedSize(w, h);

	int okWidth = 100;
	m_pOKButton->setGeometry((w - okWidth) / 2, 110, okWidth, 22);
	connect(m_pOKButton, SIGNAL(clicked()), this, SLOT(onOK()));	

	int debugWidth = 100;
	m_pDebugEnableBox->setGeometry((w - debugWidth) / 2, 50, debugWidth, 22);
	
	setWindowIconText(tr("Debug Option"));
}

void OptionDlg::onOK()
{
	g_debugEnable = m_pDebugEnableBox->isChecked();
	
	if (g_debugEnable == true)
	{
		setBit(g_debug, LOG_SOCKET_MESSAGE);
	}
	else
	{
		clearBit(g_debug, LOG_SOCKET_MESSAGE);	
	}
	accept();
}