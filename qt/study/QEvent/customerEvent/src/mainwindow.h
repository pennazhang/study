#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QEvent>

enum E_USER_EVENT
{
    EVENT_START = QEvent::User,
	EVENT_CONNECT,		/* GUI connect the KX200 Device by user */
	EVENT_DISCONNECT,	/* GUI disconnect the KX200 Device, caused by user, or No communication etc  */
	EVENT_QUITAPP,
	EVENT_CUSTOMER_EVENT,
	EVENT_CUSTOMER_EVENT_1,
};

class CustomerEvent : public QEvent
{
public:
    CustomerEvent(QString valueString = "");
	~CustomerEvent() {};

    static Type eventType();
	QString getValueString(void) { return m_String; }

private:
    static Type m_EventType;
    QString m_String;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void resizeEvent(QResizeEvent* /* event */);
	bool event(QEvent *event);

protected slots:
	void onClickedPostEvent();
	void onClickedSendEvent();

private:
    QPushButton* m_pPostEventButton, *m_pSendEventButton;
};
#endif // MAINWINDOW_H
