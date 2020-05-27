#include "MarkButton.h"
#include <QVariant>
#include <QDebug>
#include <QApplication>

MarkButton::MarkButton(QString text, QWidget *parent)
    : QPushButton(text, parent)
{
	m_markFlag = false;
}

void MarkButton::setMarkFlag(bool flag)
{
	m_markFlag = flag;
}

bool MarkButton::getMarkFlag() const
{
	return (m_markFlag);
}

void MarkButton::mark(bool flag)
{
	setProperty("markFlag", flag);
	m_markFlag = flag;

	setStyle(QApplication::style());
	ensurePolished();
}