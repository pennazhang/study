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
#include <iostream>  // std::cout
#include <algorithm> // std::find
#include <vector>    // std::vector
#include "tcpSocketThread.h"

using namespace std;

TCPSocketThread::~TCPSocketThread()
{
    if (m_tcpSocket != NULL)
    {
        m_tcpSocket->closeSocket();
        delete m_tcpSocket;
        m_tcpSocket = NULL;
    }
}

TCPSocketThread::TCPSocketThread(TCPSocket *tcpSocket) : m_tcpSocket(tcpSocket)
{
}

STATUS TCPSocketThread::onStart()
{
    return (STATUS_OK);
}

void TCPSocketThread::quit()
{
    /* If the thread is running in non-block mode, normally we need to set m_needStop to false to quit the thread */
    /* The Thread has already set m_needStop to true when closing the thread. */
    m_needStop = true;

    /* If the thread is running in block mode, we need to shutdown the socket so that we can quit the thread */
    m_tcpSocket->shutdown();
}

//  Before quit this thread, we need to do something more, such as close the socket and close the subThread.
void TCPSocketThread::onClose()
{
    if (m_tcpSocket != NULL)
    {
        m_tcpSocket->closeSocket();
        delete m_tcpSocket;
        m_tcpSocket = NULL;
    }
}

/* 
For TCP socket, we will use block mode.
For SSL, we will use non-block mode. 
So the run function should support both mode.
 */
void *TCPSocketThread::run()
{
    for (;;)
    {
        /* If the thread is running in non-block mode, normally we should quit the thread by setting m_needStop to true */
        if (m_needStop == true)
        {
            break;
        }
        mSleep(10);

        /* It will be more efficiently to run in block mode. */
        SocketEventType eventType = m_tcpSocket->checkEvent_InBlockMode();
        if (eventType == SOCKET_EVENT_TCP_CONNECTION_BROKEN)
        {
            // If the socket is broken, then we need to quit the task ASAP.
            break;
        }
        else if (eventType == SOCKET_EVENT_RECV_DATA)
        {
            // Here we received some data in m_tcpSocket.
            int recvLen = 0;

            // Get the received data from m_tcpSocket.
            UINT8 *buffer = m_tcpSocket->getReceivedData(recvLen);

            logInfo("Recv TCP message (len= %d)", recvLen);
            // append the received data to buffer in protocol.
            m_protocol.appendCommand(buffer, recvLen);

            for (;;)
            {
                ByteArray byteArray;
                
                // Get a command from the buffer of protocol.
                if (m_protocol.getNextCommand(byteArray) == true)
                {
                    std::string outputInfo;

                    // dispatch the command in the protocol class.
                    int status = m_protocol.dispatchCommand(byteArray, outputInfo);
                    std::string output;
                    if (status == STATUS_OK)
                    {
                        output = std::string("Execute command ok!\r\n") + outputInfo + std::string("\r\n");
                    }
                    else
                    {
                        output = std::string("Execute command failed!\r\n") + outputInfo + std::string("\r\n");
                    }
                    m_tcpSocket->send(output);
//                    logInfo("Send TCP Message : %s", output.c_str());
                }
                else
                {
                    break;
                }
            }
        }
    }

    return (0);
}
