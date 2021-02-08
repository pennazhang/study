#include "mainwindow.h"
#include <QDebug>
#include <QTranslator>
#include <QApplication>

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
	this->setObjectName("BackGround");
    
    m_pChineseButton = new QPushButton(tr("Chinese"), this);
	m_pChineseButton->setObjectName("tabButton");
	connect(m_pChineseButton, SIGNAL(clicked()), this, SLOT(onClickChinese()));
    m_pChineseButton->setCheckable(true);

	m_pEnglishButton = new QPushButton(tr("English"), this);
	m_pEnglishButton->setObjectName("tabButton");
	connect(m_pEnglishButton, SIGNAL(clicked()), this, SLOT(onClickEnglish()));
    m_pEnglishButton->setCheckable(true);

	m_pLanguageButtonGroup = new QButtonGroup(this);
	m_pLanguageButtonGroup->addButton(m_pEnglishButton);
	m_pLanguageButtonGroup->addButton(m_pChineseButton);

	m_pOutputSettingButton = new QPushButton(tr("Output Setting"), this);
//	m_pOutputSettingButton->setObjectName("tabButton");
//	connect(m_pOutputSettingButton, SIGNAL(clicked()), this, SLOT(onOutputSettingClicked()));
    m_pOutputSettingButton->setCheckable(true);


    setWindowTitle(tr("QT - GUI Demo") + QString(", Default Font:") + qApp->font().toString());
    setMinimumSize(400, 300);
    resize(600, 450);

	qDebug() << qApp->font().rawName();
	qDebug() << qApp->font().family();
	qDebug() << qApp->font().defaultFamily();
	qDebug() << qApp->font().styleName();
	qDebug() << qApp->font().toString();
	qDebug() << qApp->font().key();
}

MainWindow::~MainWindow()
{
}

void MainWindow::resizeEvent(QResizeEvent* /* event */)
{
    int buttonWidth = width() / 5;
    if (buttonWidth < 140)
    {
        buttonWidth = 140;
    }

	setPosition(m_pOutputSettingButton, 0.4, 0.45 , 0.2, 0.1);
	setPosition(m_pChineseButton, 0.6, 0.05, 0.1, 0.1);
	setPosition(m_pEnglishButton, 0.8, 0.05, 0.1, 0.1);
}

void MainWindow::onClickChinese()
{
    static QTranslator *pTranslator = NULL;
    if (pTranslator != NULL)
    {
        qApp->removeTranslator(pTranslator);
        delete pTranslator;
        pTranslator = NULL;
    }
    pTranslator = new QTranslator;
    QString qmFileName = ":demoGui_zh.qm";
    if (pTranslator->load(qmFileName))
    {
        qApp->installTranslator(pTranslator);
    }
	retranslateUi();
}

void MainWindow::onClickEnglish()
{
    static QTranslator *pTranslator = NULL;
    if (pTranslator != NULL)
    {
        qApp->removeTranslator(pTranslator);
        delete pTranslator;
        pTranslator = NULL;
    }
    pTranslator = new QTranslator;
    QString qmFileName = ":demoGui_en.qm";
    if (pTranslator->load(qmFileName))
    {
        qApp->installTranslator(pTranslator);
    }
	retranslateUi();
}

void MainWindow::retranslateUi()
{
	m_pOutputSettingButton->setText(tr("Output Setting"));
	m_pChineseButton->setText(tr("Chinese"));
	m_pEnglishButton->setText(tr("English"));
    setWindowTitle(tr("QT - GUI Demo") + QString(", Default Font:") + qApp->font().toString());
}
