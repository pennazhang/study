#include <QtCore/QCoreApplication>
#include <QSemaphore>
#include <QThread>
#include <iostream>
#include <QTextStream>
#include <QTime>

#define P(S)	S.acquire()
#define V(S)	S.release()

const int DataSize = 100;	// The product number we need to build.
const int BufferSize = 10;	// 
char buffer[BufferSize];
QSemaphore s_freeSpace (BufferSize);
QSemaphore s_usedSpace(0);

//生产者
class Producer:public QThread
{
protected:
    void run()
    {
        qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
//        qsrand(NULL);
        for (int i = 0;i < DataSize;++i)
        {
			P(s_freeSpace);
            std::cerr<<"P";
            V(s_usedSpace);
        }
    }
};
//消费者
class Consumer:public QThread
{
protected:
    void run()
    {
        for (int i = 0;i < DataSize;++i)
        {
            P(s_usedSpace);
            std::cerr<<"C";
            V(s_freeSpace);
        }
    }
};

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Producer producer;
    Consumer consumer;
    producer.start();
    consumer.start();
    producer.wait();

    consumer.wait();

    std::cerr << "\n";
	return 0;
}
