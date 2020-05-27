#ifndef MAIN_WIDGET_H
#define MAIN_WIDGET_H

#include <QWidget>
#include <QFrame>
#include <QPushButton>
#include <QButtonGroup>
#include <QTranslator>
#include <QCloseEvent>
#include <QRadioButton>
#include <QLabel>
#include <QLineEdit>
#include <QTimer>
#include <QPaintEvent>
#include <QMenuBar>
#include <QShortcut>
#include "MarkButton.h"
#include "InputWidget.h"

class DBLButton: public QPushButton
{
    Q_OBJECT

public:
	DBLButton(QString buttonText , QWidget *pParent);

signals:
    void doubleClicked();

protected:
	void mouseDoubleClickEvent(QMouseEvent *);
};

class MainWidget : public QWidget
{
    Q_OBJECT

public:
    MainWidget(QWidget *parent = 0);
    ~MainWidget();
	void retranslateUi();
	void refresh();
	QString reminderInfo();
	void setReminderInfo(QString reminderInfo);

signals:

protected slots:
	void onClickConnectButton();
	void onDeviceConnected();
	void onDeviceDisconnected();
	void on1STimerOut();
	void onZoom();
	void onOption();
	void onMenuShortcutActivated();

	void onInputMode1Clicked();
	void onInputMode2Clicked();
	void onInputEQ1Clicked();
	void onDblClickInputEQ1();
	void onInputEQ2Clicked();
	void onXOver1Clicked();
	void onXOver2Clicked();
	void onOutputEQ1Clicked();
	void onOutputEQ2Clicked();
	void onDelay1Clicked();
	void onDelay2Clicked();
	void onLimiter1Clicked();
	void onLimiter2Clicked();
	void onOutputModeClicked();

protected:
	void createMenuBar(QWidget *pParent);
	void resizeEvent(QResizeEvent * /* event */);
	void refreshConnectedButton();
	void createPanelWidget();
	void showEvent(QShowEvent * event);
	void setFocusButton(QPushButton *pButton);
	void refreshFocusButton();

protected:
//	QFrame *m_pTopFrame;
	MarkButton *m_pConnectButton; 
	QTimer *m_pTimer1S;
	QMenuBar *m_pMenuBar;
	QMenu *m_pMenu1, *m_pMenu2, *m_pMenuOpen, *m_pMenuSave;
	QAction *m_pActionLoadDeviceFile, *m_pActionLoadFromUSB, *m_pActionSaveDeviceFile, *m_pActionUpgrade, *m_pSetIPAddress, *m_pActionAdjustTime, *m_pActionSpeakerEditor, *m_pActionRestore, *m_pActionImportSpeaker,
		*m_pActionSaveToUSB, *m_pActionExit, *m_pActionAbout, *m_pActionZoom, *m_pActionOption;
	QShortcut *m_pMenuShortcut;
	bool m_debugActive;

	QWidget *m_pPanelWidget;
		QFrame *m_pFrame1, *m_pFrame2, *m_pFrame3;

	QLabel *m_pInputLabel, *m_pLevelLabel, *m_pOutputLabel;
	QWidget *m_pLevelWidget, *m_pOutputWidget;
	InputWidget *m_pInputWidget;

	QFrame *m_pPresetFrame;
//	{
		QLabel *m_pPresetName;
		QPushButton *m_pRecallButton, *m_pAmpButton;
		MarkButton *m_pStoreButton, *m_pSignalGenButton;
//	}

	QFrame *m_pDSPFrame;
//	{
		DBLButton *m_pInputMode1Button, *m_pInputMode2Button, *m_pInputEQ1Button, *m_pInputEQ2Button, *m_pXOver1Button, *m_pXOver2Button, *m_pOutputEQ1Button, *m_pOutputEQ2Button, *m_pDelay1Button, *m_pDelay2Button, *m_pLimiter1Button, *m_pLimiter2Button, *m_pOutputModeButton;
		QPushButton *m_pFocusButton;	/* Be one of the MarkButton list above */
		QFrame *m_pFocusFrame;
//	}
	
	QFrame *m_pReminderFrame, *m_pCrownLabel;
//	{
		QLabel *m_pReminderLabel;
//	}

	QString m_reminderInfo;
};

extern MainWidget *g_pMainWidget;

#endif // MAINWIDGET_H
