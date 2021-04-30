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
#include "networkThread.h"
#include "main.h"
#include "utility.h"
#include "tcpServer.h"
#include "channelIndex.h"
#include <iostream>     // std::cout
#include <algorithm>    // std::find
#include <vector>       // std::vector

using namespace std;

NetworkThread::~NetworkThread()
{
    closeAllSocket();
}

NetworkThread::NetworkThread(const char * threadName) : MsgThread(threadName)
{
}


STATUS NetworkThread::onStart()
{
    if (m_sslServer.listenAt(5024) == STATUS_OK)
    {
        logInfo("listen at SSL:5024...");
    }
    else
    {
        logError("Failed to listen at SSL:5024.");
        goto END;
    }

    if (m_tcpServer.listenAt(5023) == STATUS_OK)
    {
        logInfo("listen at TCP:5023...");
    }
    else
    {
        logError("Failed to listen at TCP:5023.");
        goto END;
    }

    if (m_udpSocket.listenAt(5023) == STATUS_OK)
    {
        logInfo("listen at UDP:5023...");
    }
    else
    {
        logError("Failed to listen at UDP:5023.");
        goto END;
    }
    return (STATUS_OK);

END:
    closeAllSocket();
    return (STATUS_ERROR);
}

void NetworkThread::onQuit()
{
    closeAllSocket();
}


typedef void (NetworkThread::*DispatchFunc)(UINT8* pData, int dataLen);

std::map<int, DispatchFunc> s_networkFuncMap =
{
	{   MSG_ID_TEST, &NetworkThread::onTest },
    {   MSG_ID_ON_CHANNEL_MESSAGE, &NetworkThread::onSendChannelMessage },
};

void NetworkThread::onDispatchMessage(UINT8* pData, int dataLen)
{
    CommonMessage msg;
    msg.serializeFromBuffer(pData, dataLen);
    
    DispatchFunc pFunc;

    auto iter = s_networkFuncMap.find(msg.m_msgHeader.m_msgID);
    if (iter == s_networkFuncMap.end())
    {
        logFatal("NetworkThread recv unknown msg:  %d", msg.m_msgHeader.m_msgID);
        return;
    }

    pFunc = iter->second;
    (this->*pFunc)(pData, dataLen);
}

void NetworkThread::closeAllSocket()
{
    m_sslServer.closeSocket();
    m_tcpServer.closeSocket();
    m_udpSocket.closeSocket();

    for (UINT32 i = 0; i < m_tcpSocketVector.size(); i++)
    {
        TCPSocket *tcpSocket = m_tcpSocketVector.at(i);

//        CloseSocketMessageType *pMsg = allocateCloseSocketMessage(tcpSocket->getSocketID());
//        g_moduleThread->sendMessage((CommonMessage *)pMsg);

        delete tcpSocket;
    }
    m_tcpSocketVector.clear();
}

void NetworkThread::onIdle()
{
    static TimePoint currentTime;
    if (currentTime.isTimeExpired(10, true))
    {
        /* If checkIncomingConnection != NULL for SSL Server */
        SecureTCPSocket *sslSocket = m_sslServer.checkIncomingConnection();
        if (sslSocket != NULL)
        {
            AcceptSocketMessage msg(sslSocket->getSocketID());
            g_moduleThread->sendMessage(&msg);

            m_tcpSocketVector.push_back(sslSocket);
        }

        /* If checkIncomingConnection != NULL for TCP Server*/
        TCPSocket *tcpSocket = m_tcpServer.checkIncomingConnection();
        if (tcpSocket != NULL)
        {
            AcceptSocketMessage msg(tcpSocket->getSocketID());
            g_moduleThread->sendMessage(&msg);

            m_tcpSocketVector.push_back(tcpSocket);
        }

        /* Check the incoming data for tcp socket */
        for (int i = m_tcpSocketVector.size() - 1; i >= 0; i--)
        {
            TCPSocket *tcpSocket = m_tcpSocketVector[i];
            SocketEventType eventType = tcpSocket->checkEvent();

            if (eventType == SOCKET_EVENT_TCP_CONNECTION_BROKEN)
            {
                CloseSocketMessage msg(tcpSocket->getSocketID());
                g_moduleThread->sendMessage(&msg);

                m_tcpSocketVector.erase(m_tcpSocketVector.begin() + i);
                delete tcpSocket;
            }
            else if (eventType == SOCKET_EVENT_RECV_DATA)
            {
                for (;;)
                {
                    ByteArray byteArray;
                    if (tcpSocket->getReceivedData(byteArray) == true)
                    {
//                        cout << "size = " << byteArray.size() << ": " << (char *)byteArray.data() << endl;
                        ChannelMessage msg;
                        int channelID = tcpSocket->getChannelID();
                        msg.init(channelID, byteArray);
//                        cout << "send ChannelMessage to ModuleThread, channel = " << channelID << endl;
                        g_moduleThread->sendMessage(&msg);
                    }
                    else
                    {
                        break;
                    }
                }
            }
        }

        /* Check UDP socket */
        {
            SocketEventType eventType = m_udpSocket.checkEvent();
            if (eventType == SOCKET_EVENT_UDP_BROKEN)
            {
                logError("UDP Socket is broken");
            }
            else if (eventType == SOCKET_EVENT_RECV_DATA)
            {
                logInfo("UDP Socket recv data!");
                ByteArray byteArray;
                if (m_udpSocket.getReceivedData(byteArray) == true)
                {
                    //                        cout << "size = " << byteArray.size() << ": " << (char *)byteArray.data() << endl;
                    ChannelMessage msg;
                    int channelID = m_udpSocket.getChannelID();
                    msg.init(channelID, byteArray);
//                    cout << "send ChannelMessage to ModuleThread, channel = " << channelID << endl;
                    g_moduleThread->sendMessage(&msg);
                }
            }
        }
    }
}

void NetworkThread::onTest(UINT8* pData, int len)
{
    TestMessage testMessage;
    testMessage.serializeFromBuffer(pData, len);
    dumpHex(pData, len, 1);

    cout << "NetworkThread::testMessage: " << testMessage.m_testID << endl;

    testMessage.m_testID++;

    g_moduleThread->sendMessage(&testMessage);
}

void NetworkThread::onSendChannelMessage(UINT8* pData, int len)
{
    ChannelMessage msg;

    msg.serializeFromBuffer(pData, len);
    sendChannelMessage(msg.m_channelID, msg.m_recvByteArray);
}

void NetworkThread::sendChannelMessage(int channelID, ByteArray &byteArray)
{
    ChannelParamType channelParam;
    if (g_channelIndex.getChannelParam(channelID, channelParam))
    {
        if (channelParam.m_channelType == CHANNEL_TCP)
        {
            TCPSocket *pObject = channelParam.m_param.m_tcpParam.m_objectPointer;
            auto iter = std::find(m_tcpSocketVector.begin(), m_tcpSocketVector.end(), pObject);
            if (iter != m_tcpSocketVector.end())
            {
                // we found the TCPSocket;
                pObject->send(byteArray.data(), byteArray.size());
                return;
            }
            else
            {
                logError("Can't find TCPSocket: %p", pObject);
                return;
            }
        }
        else if (channelParam.m_channelType == CHANNEL_UDP)
        {
            UDPSocket *pObject = channelParam.m_param.m_udpParam.m_objectPointer;
            if (pObject == &m_udpSocket)
            {
                pObject->sendTo(channelParam.m_param.m_udpParam.m_sockAddr, byteArray.data(), byteArray.size());
                return;
            }
            else
            {
                logError("Can't find UDPSocket: %p", pObject);
                return;
            }

        }
        else
        {
            logError("Invalid type of channelID: %d = %d", channelID, (int)channelParam.m_channelType);
            return;
        }
    }
    else
    {
        logError("Can't find the channelID: %d", channelID);
        return;
    }
}
