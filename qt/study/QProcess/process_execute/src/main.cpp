#include <QTextStream>
#include <QThread>
#include <QProcess>
#include <QString>
#include <QtCore/QCoreApplication>

int main(int argc, char **argv)
{
	QString strCmd;
    QCoreApplication app(argc, argv);

	QTextStream stdOut(stdout);
	QTextStream stdIn(stdin);
	
#ifdef _LINUX_
	QProcess::execute("bash -c /git/temp/test.sh");
#endif

#ifdef _WINDOWS_
	QProcess::execute("c:/temp/test.bat");
#endif

	stdOut << "Hello, world!\n";
	return 0;	
//	return g_pApp->exec();
}
