#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include "buttonSlider.h"
#include <QDoubleSpinBox>
#include <QLabel>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
	void refreshGEQGroup_SW();

protected:
    void resizeEvent(QResizeEvent* /* event */);

private slots:
	void onSWPEQGainSliderChanged1(int gain_10x);
	void onSWPEQGainSliderChanged2(int gain_10x);
	void onPEQCutChanged1(double gain);

private:
    QPushButton* m_pOutputSettingButton;
	ButtonSlider *m_pButtonSlider1, *m_pButtonSlider2, *m_pButtonSlider3, *m_pButtonSlider4;
	bool m_slotEnableFlag;
	QDoubleSpinBox *m_pPEQCutSpin1;
	QLabel *m_pPictureLabel;
	double m_gain;
};
#endif // MAINWINDOW_H
