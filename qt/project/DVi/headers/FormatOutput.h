#ifndef FORMAT_OUTPUT_H
#define FORMAT_OUTPUT_H

#include <QString>
#include <QTextStream>

class FormatOutput
{
public:
	static void setTextStream(QTextStream *out);
	static void dump(QString str);
	static void incDumpLevel();
	static void decDumpLevel();
	
protected:
	static int m_dumpLevel;
	static QTextStream *m_pTextStream;
};


#endif
