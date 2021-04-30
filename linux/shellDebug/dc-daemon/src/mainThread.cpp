//####
//# Legal Notice :
//#    Copyright, Harman International, 2020
//#    Private, proprietary information, the sole property of Harman.  The
//#    contents, ideas, and concepts expressed herein are not to be disclosed
//#    except within the confines of a confidential relationship and only
//#    then on a need to know basis.
//#
//#    Any entity in possession of this Harman Software shall not, and shall not
//#    permit any other person to, disclose, display, loan, publish, transfer
//#    (whether by sale, assignment, exchange, gift, operation of law or
//#    otherwise), license, sublicense, copy, or otherwise disseminate this
//#    Harman Software.
//#
//#    This Harman Software is owned by Harman and is protected by United States
//#    copyright laws, patent laws, international treaty provisions, and/or
//#    state of Texas trade secret laws.
//#
//#    Portions of this Harman Software may, from time to time, include
//#    pre-release code and such code may not be at the level of performance,
//#    compatibility and functionality of the final code. The pre-release code
//#    may not operate correctly and may be substantially modified prior to
//#    final release or certain features may not be generally released. Harman
//#    is not obligated to make or support any pre-release code. All pre-release
//#    code is provided "as is" with no warranties.
//#
//#    This Harman Software is provided with restricted rights. Use,duplication,
//#    or disclosure by the Government is subject to restrictions as set forth
//#    in subparagraph (1)(ii) of The Rights in Technical Data and Computer
//#    Software clause at DFARS 252.227-7013 or subparagraphs (1) and (2) of
//#    the Commercial Computer Software Restricted Rights at 48 CFR 52.227-19,
//#    as applicable.
//####
/*-------------------------------------------------------------------------*/
#include <stdlib.h>
#include <string.h>
#include "utility.h"
#include "main.h"
#include "utility.h"
#include "tcpServer.h"
#include "channelIndex.h"
#include <iostream>     // std::cout
#include <algorithm>    // std::find
#include <vector>       // std::vector
#include "dcProtocol.h"
#include "mainThread.h"

using namespace std;

#define UDP_PORT  5023
#define TCP_PORT  5023
#define SSL_PORT  5024

MainThread::~MainThread()
{
    closeAllSocketThread();
}

MainThread::MainThread(const char * threadName) : MsgThread(threadName)
{
    m_udpSocketThread = NULL;
}


STATUS MainThread::onStart()
{
    UDPSocket* udpSocket;
#ifdef SSL_SOCKET  
    /* Create a SSL server at port:SSL_PORT to listen the incoming socket. */
    if (m_sslServer.listenAt(SSL_PORT) == STATUS_OK)
    {
        logInfo("listen at SSL:%d...", SSL_PORT);
    }
    else
    {
        logError("Failed to listen at SSL:%d.", SSL_PORT);
        goto END;
    }
#endif

    /* Create a TCP server at port:TCP_PORT to listen the incoming socket. */
    if (m_tcpServer.listenAt(TCP_PORT) == STATUS_OK)
    {
        logInfo("listen at TCP:%d...", TCP_PORT);
    }
    else
    {
        logError("Failed to listen at %d", TCP_PORT);
        goto END;
    }

    /* Create UDPSocketThread to dispatch all commands from UDP */
    udpSocket = new UDPSocket();
    if (udpSocket->bind("0.0.0.0", UDP_PORT) == STATUS_OK)
    {
        logInfo("listen at UDP:%d...", UDP_PORT);
        m_udpSocketThread = new UDPSocketThread(udpSocket);
        m_udpSocketThread->start();
    }
    else
    {
        logError("Failed to listen at UDP:%d.", UDP_PORT);
        goto END;
    }

    return (STATUS_OK);

END:
    closeAllSocketThread();
    return (STATUS_ERROR);
}

/* We need to close all sub threads created by MainThread */
void MainThread::closeAllSocketThread()
{
#ifdef SSL_SOCKET
    // close the SSL server.
    m_sslServer.closeSocket();
#endif	

    // close the tcp server.
    m_tcpServer.closeSocket();

    // Stop the UDPSocket.
    if (m_udpSocketThread != NULL)
    {
        m_udpSocketThread->stop();
        delete m_udpSocketThread;
        m_udpSocketThread = NULL;
    }

    // Close all the TCP and SSL connection.
    for (UINT32 i = 0; i < m_tcpSocketThreadVector.size(); i++)
    {
        TCPSocketThread * tcpSocketThread = m_tcpSocketThreadVector.at(i);
//        logInfo("Stopping %s, running flag = %d", tcpSocketThread->getThreadName().c_str(), tcpSocketThread->isRunning());
        tcpSocketThread->stop();
        delete tcpSocketThread;
    }
    m_tcpSocketThreadVector.clear();
}

void MainThread::onClose()
{
    closeAllSocketThread();
}

typedef void (MainThread::*DispatchFunc)(UINT8* pData, int dataLen);

std::map<int, DispatchFunc> s_funcMap =
{
//	{   MSG_ID_TEST, &MainThread::onTest },
//    {   MSG_ID_ON_CHANNEL_MESSAGE, &MainThread::onSendChannelMessage },
};

void MainThread::onDispatchMessage(UINT8* pData, int dataLen)
{
    CommonMessage msg;
    msg.serializeFromBuffer(pData, dataLen);
    
    DispatchFunc pFunc;

    auto iter = s_funcMap.find(msg.m_msgHeader.m_msgID);
    if (iter == s_funcMap.end())
    {
        logFatal("MainThread recv unknown msg:  %d", msg.m_msgHeader.m_msgID);
        return;
    }

    pFunc = iter->second;
    (this->*pFunc)(pData, dataLen);
}

/* MainLoop from MessageThread */
void MainThread::onIdle()
{
    static TimePoint currentTime;
    if (currentTime.isTimeExpired(100, true))
    {
        for (UINT32 i = 0; i < m_tcpSocketThreadVector.size(); i++)
        {
            TCPSocketThread * tcpSocketThread = m_tcpSocketThreadVector.at(i);
            if (tcpSocketThread->isRunning() == false)
            {
                delete tcpSocketThread;
//                logInfo("Remove thread: %s", tcpSocketThread->getThreadName().c_str());
                m_tcpSocketThreadVector.erase (m_tcpSocketThreadVector.begin() + i);
            }
        }

        /* If checkIncomingConnection != NULL for SSL Server */
#ifdef SSL_SOCKET
        /* Since the sslSocket is set as non-block mode, so checkIncomingConnection is a non-block function. */
        SecureTCPSocket *sslSocket = m_sslServer.checkIncomingConnection();
        if (sslSocket != NULL)
        {
            TCPSocketThread *pThread = new TCPSocketThread(sslSocket);
            m_tcpSocketThreadVector.push_back(pThread);

            char buff[100];
            snprintf(buff, sizeof(buff), "ssl_%d", sslSocket->getSocketID());
            pThread->setThreadName(buff);

            pThread->start();
        }
#endif
        /* If checkIncomingConnection != NULL for TCP Server*/
        /* Since the tcpSocket is set as non-block mode, so checkIncomingConnection is a non-block function. */
        TCPSocket *tcpSocket = m_tcpServer.checkIncomingConnection();
        if (tcpSocket != NULL)
        {
            logInfo("Open Socket: %d", tcpSocket->getSocketID());
            TCPSocketThread *pThread = new TCPSocketThread(tcpSocket);
            m_tcpSocketThreadVector.push_back(pThread);

            char buff[100];
            snprintf(buff, sizeof(buff), "tcp_%d", tcpSocket->getSocketID());
            pThread->setThreadName(buff);

            pThread->start();
        }

    }
}
