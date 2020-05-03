#include <QTextStream>
#include <QThread>
#include <QProcess>
#include <QtCore/QCoreApplication>
#include <QString>
#include "main.h"
#include <QFile>

QCoreApplication *g_pApp = NULL;
QTextStream g_stdOut(stdout), g_stdIn(stdin);

void saveTest()
{
    QFile file("serialization.data");
    if(!file.open(QIODevice::WriteOnly))
    {
        g_stdOut << "failed to open file!" << endl;
        return;
    }

    g_stdOut<< "\nSaving file..." << endl;

	Student s(20180651, "Mike", 23, 78);
	s.show();

    QDataStream out(&file);

    out << s ;
    file.close();
    g_stdOut<< "save file OK" << endl;

}

void loadTest()
{
	Student s;
	QFile file("serialization.data");
    if (!file.open(QIODevice::ReadOnly)) 
	{
        g_stdOut << "failed to open file!" << endl;
        return;
    }

    g_stdOut<< "\nLoading file..." << endl;
    QDataStream in(&file);
    in >> s ;
    file.close();
	s.show();
    g_stdOut<< "load file OK" << endl;
}


int main(int argc, char **argv)
{
	QString strCmd;
    QCoreApplication app(argc, argv);
	g_pApp = &app;

	saveTest();
	loadTest();

	return 0;

	g_stdOut << "Hello, world!\n";
	return g_pApp->exec();
}

QDataStream & People::operator <<(QDataStream &output)
{
	output << m_id << m_strName << m_age; 
	return (output); 
}

QDataStream & People::operator >>(QDataStream &input) 
{
	input >> m_id >> m_strName >> m_age; 
	return (input); 
}

QDataStream & Student::operator <<(QDataStream &output)
{
	output << m_averageScore;
	People::operator<<(output);
	return (output);
}

QDataStream & Student::operator >>(QDataStream &input)
{
	input >> m_averageScore;
	People::operator>>(input);
	return (input);
}

void Student::show()
{
	g_stdOut << "id: " << m_id << endl;
	g_stdOut << "name: " << m_strName << endl;
	g_stdOut << "age: " << m_age << endl;
	g_stdOut << "score: " << m_averageScore << endl;
}


