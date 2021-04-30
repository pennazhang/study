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
#include "udpSocketThread.h"
#include "dcUtility.h"

using namespace std;

UDPSocketThread::~UDPSocketThread()
{
    if (m_udpSocket != NULL)
    {
        m_udpSocket->closeSocket();
        delete m_udpSocket;
        m_udpSocket = NULL;
    }
}

UDPSocketThread::UDPSocketThread(UDPSocket *udpSocket) : Thread("UDPSocketThread"), m_udpSocket(udpSocket)
{
}

STATUS UDPSocketThread::onStart()
{
    return (STATUS_OK);
}

// Before quit the thread, we need to close the UDP socket.
void UDPSocketThread::onClose()
{
    if (m_udpSocket != NULL)
    {
        m_udpSocket->closeSocket();
        delete m_udpSocket;
        m_udpSocket = NULL;
    }
}

void *UDPSocketThread::run()
{
    for (;;)
    {
        mSleep(20);
        if (m_needStop == true)
        {
            break;
        }

        /* Check the incoming Event */
        SocketEventType eventType = m_udpSocket->checkEvent();
        if (eventType == SOCKET_EVENT_UDP_BROKEN)
        {
            logError("UDP Socket is broken");
            break;
        }
        else if (eventType == SOCKET_EVENT_RECV_DATA)
        {
            UINT8 version, opcode;
            UINT32 sig;
            /* Receive UDP Data */
            int recvLen;
            std::string outputBuffer;
            UINT8 *buffer = m_udpSocket->getReceivedData(recvLen);

            sockaddr_in *peerIP = m_udpSocket->getPeerIP();
//            char *ipAddr = inet_ntoa(peerIP->sin_addr);
//            logInfo("Recv UDP message from: %s:%d (len= %d)", ipAddr, htons(peerIP->sin_port), recvLen);

            version = buffer[0];
            if (version == 1)
            {
                // It is broadcast package for discovery protocol.
                opcode = buffer[1];
                sig = *(UINT32 *)(buffer + 2);
                if (sig == 0x3412feca)
                {
                    switch (opcode)
                    {
                    case 20:
                    case 25:
                    {
//                        unsigned char discoverCmd = (DCSettings::IamTx() == true) ? 20 : 25;
//                        if (opcode == discoverCmd)
                        {
                            DCSettings::getStatus(outputBuffer);

                            // In a large network, there may be many decoders/encoders all trying
                            // to respond at the same time.  To avoid a suddenly large number of
                            // colliding broadcast messages, we're going to delay some random time.
                            usleep(rand() % 1000);

                            m_udpSocket->sendTo(*peerIP, (UINT8 *)outputBuffer.c_str(), outputBuffer.length());
                        }
                    }
                    break;

                    default:
                        break;
                    }
                }
            }
            else if ((buffer[0] >= 'a' && buffer[0] <= 'z') || (buffer[0] >= 'A' && buffer[0] <= 'Z'))
            {
                // Just commands.
                m_protocol.appendCommand(buffer, recvLen);

                // Let's dispatch commands one by one.
                for (;;)
                {
                    ByteArray byteArray;
                    /* Get direct control command from command buffer */
                    if (m_protocol.getNextCommand(byteArray) == true)
                    {
                        std::string outputInfo;
                    
                        // Dispatch Direct Control Command.
                        int status = m_protocol.dispatchCommand(byteArray, outputInfo);

                        // Send back the result.
                        if (status == STATUS_OK)
                        {
                            m_udpSocket->sendTo(*peerIP, std::string("Execute command ok!\r\n") + outputInfo + std::string("\r\n"));
                        }
                        else
                        {
                            m_udpSocket->sendTo(*peerIP, std::string("Execute command failed!\r\n") + outputInfo + std::string("\r\n"));
                        }
                    }
                    else
                    {
                        break;
                    }
                }
            }
        }
    }
    return (0);
}
