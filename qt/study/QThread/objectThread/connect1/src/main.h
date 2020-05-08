
#ifndef _MAIN_H
#define _MAIN_H

#include <QThread>
#include <QDebug>
#include <QTimer>

class SomeObject : public QObject 
{ 
    Q_OBJECT 
public: 
    SomeObject(QObject* parent=0) : QObject(parent) {} 
    void callEmitSignal()
    { 
        emit someSignal(); 
    } 
signals: 
    void someSignal(); 
}; 
class SubThread : public QThread 
{ 
    Q_OBJECT 
public: 
    SubThread(QObject* parent=0) : QThread(parent){} 
    virtual ~SubThread() 
    { 
        if (obj!=NULL) delete obj; 
    } 
public slots: 
    // slot function connected to obj's someSignal 
    void someSlot(); 
public: 
    SomeObject * obj; 
}; 

// define sub thread class 1 
class SubThread1 : public SubThread 
{ 
    Q_OBJECT 
public: 
    SubThread1(QObject* parent=0); 
    // reimplement run 
    void run(); 
}; 

class SubThread2 : public SubThread 
{ 
    Q_OBJECT 
public: 
    SubThread2(QObject* parent=0); 
    // reimplement run 
    void run(); 
};

class SubThread3 : public SubThread 
{ 
    Q_OBJECT 
public: 
    SubThread3(QObject* parent=0); 
    // reimplement run 
    void run(); 
};

#endif
