#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include "buttonSlider.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void resizeEvent(QResizeEvent* /* event */);

private:
    QPushButton* m_pOutputSettingButton;
	ButtonSlider *m_pButtonSlider1, *m_pButtonSlider12, *m_pButtonSlider3;
};
#endif // MAINWINDOW_H
