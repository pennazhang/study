#ifndef MarkButton_H
#define MarkButton_H

#include "QPushButton"

class MarkButton : public QPushButton
{
public:
	MarkButton(QString buttonText, QWidget *pParent);
	void mark(bool flag); /* Please call this function to mark/disable the button */
    bool getMarkFlag() const;

protected:
    Q_PROPERTY(bool markFlag READ getMarkFlag WRITE setMarkFlag)
	void setMarkFlag(bool flag);

protected:
	bool m_markFlag;
};


#endif
