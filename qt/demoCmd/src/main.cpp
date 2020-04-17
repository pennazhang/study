#include <QTextStream>
#include <QtCore/QCoreApplication>
#include <QThread>

int main(int argc, char **argv)
{
	QString strCmd;
    QCoreApplication app(argc, argv);

	QTextStream cout(stdout);
	QTextStream cin(stdin);
	
	cout << "[TID " << QThread::currentThreadId() << "] Hello, world!\n";

	for ( ; ; )
	{
		cout << "\nType \"exit\" to quit...\n -> ";
		cout.flush();
		
		cin >> strCmd;
		if (strCmd != "exit")
		{
			cout << strCmd << endl;
		}
		else
		{
			break;
		}

	}
	return 0;
//	return g_pApp->exec();
}
