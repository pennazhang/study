#include <QTextStream>
#include <QThread>
#include <QProcess>
#include <QString>
#include <QtCore/QCoreApplication>


QCoreApplication *g_pApp = NULL;
QTextStream *g_pCout = NULL;

#ifdef _LINUX_
QString executeLinuxScript(QString strScriptFile)
{
    QProcess process;
    process.start(QString("bash -c %1").arg(strScriptFile));
    process.waitForFinished(-1);
    QString strResult = process.readAllStandardOutput();
    return strResult;
}
#endif

#ifdef _WINDOWS_
QString executeWindowsScript(QString strScriptFile)
{
	QProcess process;
	process.setProgram("cmd");
    QStringList argument;
    argument<<"/c" << strScriptFile;
    process.setArguments(argument);
    process.start();

	process.waitForFinished(-1);
    QString strResult = process.readAllStandardOutput();
    return strResult;
}
#endif

int main(int argc, char **argv)
{
	QString strCmd;
    QCoreApplication app(argc, argv);

	QTextStream cout(stdout);
	QTextStream cin(stdin);
	
#ifdef _LINUX_
	QString result = executeLinuxScript("/git/temp/test.sh");
#endif

#ifdef _WINDOWS_
	QString result = executeWindowsScript("c:/temp/test.bat");
#endif

	cout << "Hello, world!\n";
	cout << "Executing result:" << endl;
	cout << result << endl;
	return 0;	
//	return g_pApp->exec();
}
