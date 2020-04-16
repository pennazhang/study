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
	QLabel *m_pLabel_default;
	QLabel *m_pLabel_10px, *m_pLabel_12px, *m_pLabel_14px;
	QLabel *m_pLabel_10pt, *m_pLabel_12pt, *m_pLabel_14pt;
};

extern MainWidget *g_pMainWidget;

#endif // MAINWIDGET_H
