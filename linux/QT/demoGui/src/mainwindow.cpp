#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    m_pOutputSettingButton = new QPushButton(tr("Output Setting"), this);
//	m_pOutputSettingButton->setObjectName(QStringLiteral("tabButton"));
//	connect(m_pOutputSettingButton, SIGNAL(clicked()), this, SLOT(onOutputSettingClicked()));
    m_pOutputSettingButton->setCheckable(true);

    setWindowTitle(tr("QT - GUI Demo"));
    setMinimumSize(400, 300);
    resize(600, 450);
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
    int buttonHeight = height() / 10;

    m_pOutputSettingButton->setGeometry((width() - buttonWidth) / 2, (height() - buttonHeight) / 2, buttonWidth, buttonHeight);
}
