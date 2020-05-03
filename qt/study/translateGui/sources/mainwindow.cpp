#include "mainwindow.h"
#include <QDateTime>
#include <QString>
#include <QTranslator>

void setPosition(QWidget *pWidget, qreal x, qreal y, qreal w, qreal h)
{
	QWidget *pParentWidget = pWidget->parentWidget();

	int parentWidth = pParentWidget->width();
	int parentHeight = pParentWidget->height();

	int xp, yp, wp, hp;
	if ((x > 0) && (x < 1))
	{
		xp = x * parentWidth;
	}
	else
	{
		xp = x;
	}

	if ((y > 0) && (y < 1))
	{
		yp = y * parentHeight;
	}
	else
	{
		yp = y;
	}

	if ((w > 0) && (w < 1))
	{
		wp = w * parentWidth;
	}
	else
	{
		wp = w;
	}

	if ((h > 0) && (h < 1))
	{
		hp = h * parentHeight;
	}
	else
	{
		hp = h;
	}
	pWidget->setGeometry(xp, yp, wp, hp);
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    m_pEnglishButton = new QPushButton(tr("English"), this);
    m_pEnglishButton->setCheckable(true);
    m_pEnglishButton->setObjectName("languageButton");
	m_pEnglishButton->setChecked(true);

    m_pChineseButton = new QPushButton(tr("Chinese"), this);
    m_pChineseButton->setCheckable(true);
    m_pChineseButton->setObjectName("languageButton");
	m_pChineseButton->setChecked(false);

	m_pLanguageButtonGroup = new QButtonGroup(this);
	m_pLanguageButtonGroup->addButton(m_pEnglishButton);
	m_pLanguageButtonGroup->addButton(m_pChineseButton);

    m_pExitButton = new QPushButton(tr("Exit"), this);
    m_pExitButton->setObjectName("Black_Font_10pt");

    m_pLabel = new QLabel(tr("Current Time"), this);
    m_pLabel->setObjectName("Black_Font_10pt");

	m_pLineEdit = new QLineEdit(this);
    m_pLineEdit->setObjectName("Black_Font_10pt");

	m_pTimer = new QTimer(this);
	m_pTimer->start(1000);
    connect(m_pTimer, SIGNAL(timeout()), this, SLOT(onTimeOut()));

	connect(m_pEnglishButton, SIGNAL(clicked()), this, SLOT(onClickLanguageButton()));
	connect(m_pChineseButton, SIGNAL(clicked()), this, SLOT(onClickLanguageButton()));
	connect(m_pExitButton, SIGNAL(clicked()), this, SLOT(close()));


    setWindowTitle(tr("QT - Tranlater Demo"));
    setMinimumSize(400, 300);
    resize(600, 450);

//	m_pChineseButton->emit clicked();
}

MainWindow::~MainWindow()
{
}

void MainWindow::resizeEvent(QResizeEvent* /* event */)
{
    setPosition(m_pEnglishButton, 0.2, 0.2, 0.2, 0.08);
    setPosition(m_pChineseButton, 0.6, 0.2, 0.2, 0.08);
    setPosition(m_pLabel, 0.2, 0.45, 0.2, 0.1);
    setPosition(m_pLineEdit, 0.4, 0.45, 0.4, 0.1);
    setPosition(m_pExitButton, 0.6, 0.7, 0.2, 0.1);
}

void MainWindow::onTimeOut(void)
{
	QDateTime current_time = QDateTime::currentDateTime(); 
	QStringList weekDayList;
	weekDayList << tr("Sunday") << tr("Monday") << tr("Tuesday") << tr("Wednesday") << tr("Thursday") << tr("Friday") << tr("Saturday") << tr("Sunday"); 

    QString StrCurrentTime = current_time.toString("yyyy-MM-dd hh:mm:ss ") + weekDayList.at(current_time.date().dayOfWeek()); 
    m_pLineEdit->setText(StrCurrentTime); 
}

void MainWindow::retranslateUi()
{
    setWindowTitle(tr("QT - Tranlater Demo"));
	m_pEnglishButton->setText(tr("English"));
	m_pChineseButton->setText(tr("Chinese"));
	m_pExitButton->setText(tr("Exit"));
	m_pLabel->setText(tr("Current Time"));
	
	onTimeOut();
}

void MainWindow::onClickLanguageButton()
{
	static QTranslator *s_pTranslator = NULL;
    if (s_pTranslator != NULL)
    {
        qApp->removeTranslator(s_pTranslator);
        delete s_pTranslator;
        s_pTranslator = NULL;
    }
    s_pTranslator = new QTranslator;

	QString qmFileName;
	if (m_pEnglishButton->isChecked())
	{
		qmFileName = ":translateGui_en.qm";
	}
	else
	{
		qmFileName = ":translateGui_zh.qm";	
	}
    if (s_pTranslator->load(qmFileName))
    {
        qApp->installTranslator(s_pTranslator);
    }
	retranslateUi();
}