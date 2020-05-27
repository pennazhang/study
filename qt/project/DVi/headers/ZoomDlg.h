#ifndef ZOOM_DIALOG_H
#define ZOOM_DIALOG_H

#include <QDialog>
#include <QLabel>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QPushButton>
#include "ButtonSlider.h"

class ZoomDlg : public QDialog
{
    Q_OBJECT
public:
    ZoomDlg(QWidget *parent);

signals:

protected slots:
	void onClickScaleEnable();
	void onFactorSliderChanged(int value);
	void onFactorSpinChanged(double value);
	void onApplyClicked();
	void onCancelClicked();

protected:
	void resizeEvent(QResizeEvent * /* event */);
	void checkApplyButton();

protected:
	QCheckBox *m_pCheckBox;
	QLabel *m_pZoomLable;
	ButtonSlider *m_pButtonSlider;
	QDoubleSpinBox *m_pDoubleSpinBox;
	QPushButton *m_pApplyButton, *m_pCancelButton;
	bool m_slotEnableFlag;
};

class OptionDlg : public QDialog
{
    Q_OBJECT
public:
    OptionDlg(QWidget *parent);

signals:

protected slots:
	void onOK();

protected:
	QCheckBox * m_pDebugEnableBox;
	QPushButton *m_pOKButton;
};

#endif

