#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QButtonGroup>
#include <QLineEdit>
#include <QLabel>
#include <QTimer>
#include <QApplication>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
	void retranslateUi();

private slots:
	void onTimeOut();
	void onClickLanguageButton();

protected:
    void resizeEvent(QResizeEvent* /* event */);

private:
    QPushButton* m_pEnglishButton, *m_pChineseButton, *m_pExitButton;
	QButtonGroup *m_pLanguageButtonGroup;
	QLabel *m_pLabel;
	QLineEdit *m_pLineEdit;
	QTimer *m_pTimer;
};

extern QApplication *g_pApp;

#endif // MAINWINDOW_H
