#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QButtonGroup>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected slots:
	void onClickChinese();
	void onClickEnglish();

protected:
    void resizeEvent(QResizeEvent* /* event */);
	void retranslateUi();

private:
    QPushButton* m_pOutputSettingButton;
	QPushButton* m_pChineseButton, *m_pEnglishButton;
	QButtonGroup *m_pLanguageButtonGroup;
};
#endif // MAINWINDOW_H
