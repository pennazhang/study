#include "mainApplication.h"
#include <QTime>
#include <QDebug>
#include "main.h"

MainApplication *g_pApp = NULL;

// slot function connected to obj's someSignal 
void SubThread::someSlot() 
{ 
    QString msg; 
    msg.append(this->metaObject()->className()); 
    msg.append("::obj's thread is "); 
    if (obj->thread() == g_pApp->thread()) 
    { 
        msg.append("MAIN thread;"); 
    } 
    else if (obj->thread() == this) 
    { 
        msg.append("SUB thread;"); 
    } 
    else 
    { 
        msg.append("OTHER thread;"); 
    } 
    msg.append(" someSlot executed in "); 
    if (QThread::currentThread() == g_pApp->thread()) 
    { 
        msg.append("MAIN thread;"); 
    } 
    else if (QThread::currentThread() == this) 
    { 
        msg.append("SUB thread;"); 
    } 
    else 
    { 
        msg.append("OTHER thread;"); 
    } 
    qDebug() << msg; 
    quit(); 
}

//////////////////////////////////////////////////////// 
// class SubThread1 
//////////////////////////////////////////////////////// 
SubThread1::SubThread1(QObject* parent) 
    : SubThread(parent) 
{ 
    obj = new SomeObject();//由主线程创建 
    connect(obj, SIGNAL(someSignal()), this, SLOT(someSlot())); 
} 
// reimplement run 
void SubThread1::run() 
{ 
    obj->callEmitSignal(); 
    exec(); 
}

//////////////////////////////////////////////////////// 
// class SubThread2 
//////////////////////////////////////////////////////// 
SubThread2::SubThread2(QObject* parent) 
    : SubThread(parent) 
{ 
    obj=0; 
} 
// reimplement run 
void SubThread2::run() 
{ 
    obj = new SomeObject(); //由当前子线程创建 
    connect(obj, SIGNAL(someSignal()), this, SLOT(someSlot())); 
    obj->callEmitSignal(); 
    exec(); 
}

//////////////////////////////////////////////////////// 
// class SubThread3 
//////////////////////////////////////////////////////// 
SubThread3::SubThread3(QObject* parent) 
    : SubThread(parent) 
{ 
    obj=0; 
} 
// reimplement run 
void SubThread3::run() 
{ 
    obj = new SomeObject(); 
    connect(obj, SIGNAL(someSignal()), this, SLOT(someSlot()), 
            Qt::DirectConnection); 
    obj->callEmitSignal(); 
    exec(); 
}

int main(int argc, char **argv)
{
	MainApplication app(argc, argv);
	g_pApp = &app;

    SubThread1* t1 = new SubThread1(g_pApp); //由主线程创建 
    t1->start(); 
    SubThread2* t2 = new SubThread2(g_pApp); //由主线程创建 
    t2->start(); 
    SubThread3* t3 = new SubThread3(g_pApp); //由主线程创建 
    t3->start(); 

	return g_pApp->exec();
}
