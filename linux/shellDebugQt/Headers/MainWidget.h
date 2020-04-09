#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>

class MainWidget : public QWidget
{
    Q_OBJECT

public:
    MainWidget(QWidget *parent = 0);
    ~MainWidget();

public slots:
	void setSize(int width, int height);
	void showSize();

protected:
	void resizeEvent(QResizeEvent * /* event */);

protected:
	QLabel *m_pLabel;
	QLineEdit *m_pLineEdit;
};

extern MainWidget *g_pMainWidget;

#endif // MAINWIDGET_H
