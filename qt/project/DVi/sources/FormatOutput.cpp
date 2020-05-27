#include "FormatOutput.h"
#include <QDebug>

int FormatOutput::m_dumpLevel = 0;
QTextStream *FormatOutput::m_pTextStream = nullptr;
	
void FormatOutput::setTextStream(QTextStream *out)
{
	m_pTextStream = out;
}

void FormatOutput::dump(QString str)
{
    QString text;
	if (m_pTextStream == nullptr)
	{
		for (int i = 0; i < m_dumpLevel; i++)
		{
            text += "    ";
		}
        text += str;
        qDebug() << text;
	}
	else
	{
		for (int i = 0; i < m_dumpLevel; i++)
		{
			*m_pTextStream << "\t";
		}
		*m_pTextStream << str << endl;
	}
}

void FormatOutput::incDumpLevel()
{
	dump("{");
	m_dumpLevel++; 
}

void FormatOutput::decDumpLevel()
{
	m_dumpLevel--;
	if (m_dumpLevel <0)
	{
		m_dumpLevel = 0;
	}
	dump("}");
}