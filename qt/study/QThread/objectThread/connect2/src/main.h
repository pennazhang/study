
#ifndef _MAIN_H
#define _MAIN_H

#include <QThread>
#include <QDebug>
#include <QTimer>

class senderObject : public QObject 
{ 
    Q_OBJECT 
public: 
    senderObject(QObject* parent=0) : QObject(parent) {} ;
    void callEmitSignal();
signals: 
    void someSignal(); 
}; 

class receiverObject : public QObject 
{ 
    Q_OBJECT 
public: 
    receiverObject(QObject* parent=0) : QObject(parent) {} 
public slots: 
    // slot function connected to obj's someSignal 
    void someSlot(); 
}; 


class SubThread : public QThread 
{ 
    Q_OBJECT 
public: 
    SubThread(QObject* parent=0); 
    // reimplement run 
    void run(); 
}; 

class ThirdThread : public QThread 
{ 
    Q_OBJECT 
public: 
    ThirdThread(QObject* parent=0); 
    // reimplement run 
    void run(); 
}; 

extern senderObject *g_pSenderObject ;
extern receiverObject *g_pReceiverObject ;
extern SubThread * g_pSubThread ;
extern ThirdThread *g_pThirdThread ;
extern QThread *g_pMainThread ;

#endif
