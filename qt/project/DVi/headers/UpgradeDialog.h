#ifndef UPGARDE_DIALOG_H
#define UPGARDE_DIALOG_H

#include <QPixmap>
#include <QDialog>
#include <QLabel>
#include <QRadioButton>
#include <QPushButton>
#include <QLineEdit>
//#include "CPi2000Setting.h"
#include "BackgroundWidget.h"
#include <QGroupBox>

typedef enum
{
	UPGRADE_OPEATION = 0,
	DOWNGRADE_OPEATION = 1,
} UPDATE_OPERATION;

class UpgradeDialog : public QDialog
{
    Q_OBJECT
public:
    UpgradeDialog(QWidget *parent);
	void setReminder(QString reminder);
	void setUpgradeFlag(bool flag) { m_upgradeFlag = flag; }
	bool getUpgradeFlag(void) { return m_upgradeFlag; }

	void setUpgradeResult(bool result) { m_upgradeResultFlag = result; }
	bool getUpgradeResult() { return m_upgradeResultFlag; }
	void setOperation(UPDATE_OPERATION operaton);

signals:

protected slots:
	void onClickPage1Yes();

protected:
	void resizeEvent(QResizeEvent * /* event */);

protected:
	QLabel *m_pVerisonLabel;
	QLabel *m_pRemainderLabel;
	BackgroundWidget *m_pWaitingWidget;
	bool m_upgradeFlag;			// upgrade or not
	bool m_upgradeResultFlag;   // if upgrade, success (true) or fail(false)
	UPDATE_OPERATION m_operation;
	QPushButton *m_pPage1_Yes, *m_pPage1_Cancel;
};



#endif

