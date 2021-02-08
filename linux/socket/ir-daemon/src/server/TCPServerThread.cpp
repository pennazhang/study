//####
//# Legal Notice :
//#    Copyright, AMX LLC, 2009
//#    Private, proprietary information, the sole property of AMX LLC.  The
//#    contents, ideas, and concepts expressed herein are not to be disclosed
//#    except within the confines of a confidential relationship and only
//#    then on a need to know basis.
//#
//#    Any entity in possession of this AMX Software shall not, and shall not
//#    permit any other person to, disclose, display, loan, publish, transfer
//#    (whether by sale, assignment, exchange, gift, operation of law or
//#    otherwise), license, sublicense, copy, or otherwise disseminate this
//#    AMX Software.
//#
//#    This AMX Software is owned by AMX and is protected by United States
//#    copyright laws, patent laws, international treaty provisions, and/or
//#    state of Texas trade secret laws.
//#
//#    Portions of this AMX Software may, from time to time, include
//#    pre-release code and such code may not be at the level of performance,
//#    compatibility and functionality of the final code. The pre-release code
//#    may not operate correctly and may be substantially modified prior to
//#    final release or certain features may not be generally released. AMX is
//#    not obligated to make or support any pre-release code. All pre-release
//#    code is provided "as is" with no warranties.
//#
//#    This AMX Software is provided with restricted rights. Use, duplication,
//#    or disclosure by the Government is subject to restrictions as set forth
//#    in subparagraph (1)(ii) of The Rights in Technical Data and Computer
//#    Software clause at DFARS 252.227-7013 or subparagraphs (1) and (2) of
//#    the Commercial Computer Software Restricted Rights at 48 CFR 52.227-19,
//#    as applicable.
//####

#include <stdio.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h> // exit()
#include <string.h> // memset()
#include <unistd.h> // close()
#include <sys/socket.h>
#include <netinet/in.h> // IPPROTO_TCP, htons(), htons()
#include <arpa/inet.h> // inet_addr()
#include <pthread.h>
#include <sys/select.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <chrono>
#include "TCPServerThread.h"
#include "ServerSetting.h"
#include "jsIrUtils.h"

using std::chrono::high_resolution_clock;
using std::chrono::milliseconds;

/*
    Return Value:
    -1 : Some error happened during the recving process.
    0:   No data is received by the socket before timeout.
    n [1..size]: Some data is received before timeout.
*/
int blockRecv(int socket, UINT8 *buffer, int size, int ms)
{
    int recvLen = 0;
    TimePoint beginTime;

    for ( ; ; )
    {
        if (beginTime.isTimeExpired(ms, false))
        {
            break;
        }

//      It seems that the isFdReadable has already include sleep function. or we need to add sleep function.
//      std::this_thread::sleep_for(std::chrono::milliseconds(1));
        
        if (isFdReadable(socket, 1))
        {
            int ret = recv(socket, buffer + recvLen, size - recvLen, 0);
            if (recv <= 0)
            {
                /* connection is borken */
                logInfo(" Socket is borken.");
                return (-1);
            }
            recvLen += ret;
            if (recvLen == size)
            {
                break;
            }
        }
    }
    
    logDebug( "Received %d bytes, %02x %02x %02x %02x %02x %02x %02x %02x", recvLen, buffer[0], buffer[1], 
                buffer[2], buffer[3], buffer[4], buffer[5], buffer[6], buffer[7]);
    return (recvLen);
}

#ifdef TCP_SERVER_THREAD_DEBUG

void TCPServerProc(TCPServerThread *pThread)
{
    MsgQueue *pMsgQueue = pThread->getMsgQueue();
    
    logInfo("Enter TCPServerProc ...");
    for (int count = 0; ; count++)
    {
        if (pThread->getStopFlag())
        {
            break;
        }
        
        if ((count & 0xFFF) == 0)
        {
            logDebug("Test count : %d", count);
        }
#if 0       
        for (int i = 0; i < 1000; i++)
        {
            if (pMsgQueue->test() == STATUS_ERROR)
            {
                logError("pMsgQueue->test failed!");
                goto END;
            }
        }
#else
        for (int i = 0; i < 10; i++)
        {
            pMsgQueue->putDemoData(15, 250);
        }
#endif
//      
        std::this_thread::sleep_for(std::chrono::milliseconds(1));      
    }
    
END:
    logInfo("Leave TCPServerProc...");
}

#else

void TCPCommunicatonProc(TCPServerThread *pThread, int workerSocket)
{
    int recvLen = 0;    
    MsgHeaderType msgHeader;    
    MsgQueue *msgQueue = pThread->getMsgQueue();

#ifdef  COMMUNICATOIN_SHAKE_HAND
    TimePoint lastRecvDataTime;
#endif
    do
    {
        //  Let's sleep for 10ms
        std::this_thread::sleep_for(std::chrono::milliseconds(10));     

        if (isFdReadable(workerSocket, 1))
        {
            /* First, we need to read the msg Header */
            recvLen = blockRecv(workerSocket, (UINT8 *)&msgHeader, sizeof(MsgHeaderType), 1000);
            if (recvLen != sizeof(MsgHeaderType))
            {
                logInfo("client:%d socket closed!", workerSocket);
                break;
            }
            
#ifdef  COMMUNICATOIN_SHAKE_HAND            
            lastRecvDataTime.getCurrentTime();
#endif
            /* PASS_THROUGH message */
            if (msgHeader.m_msgID == MSG_ID_IR_RAW_ONCE)
            {
                // Normally only MSG_ID_IR_PASS_THROUTH was transfered in network.
                int dataLen = msgHeader.m_msgDataLength;
                UINT8 *data = new UINT8[dataLen + sizeof(MsgHeaderType)];

                // Save the message Header
                memcpy(data, &msgHeader, sizeof(MsgHeaderType));

                /* Then, we will read the message body */
                recvLen = blockRecv(workerSocket, data + sizeof(MsgHeaderType), dataLen, 5000);
                if (recvLen != dataLen)
                {
                    logInfo("Invalid socket length: need %d, read %d", dataLen, recvLen);
                    break;
                }
                
                msgQueue->putMsg(data, 0, dataLen + sizeof(MsgHeaderType), true);               
            }
            else if (msgHeader.m_msgID == MSG_SOCKET_SHAKE_HAND)
            {
                // Shake Hand Message, just send it back.
                send(workerSocket, (void *)&msgHeader, sizeof(MsgHeaderType), 0);
            }
            else
            {
                logError("Invalid socket message ID = %d", msgHeader.m_msgID);
                break;
            }

        }
        
#ifdef  COMMUNICATOIN_SHAKE_HAND
        if (lastRecvDataTime.isTimeExpired(SHAKE_HAND_CHECK_INTERVAL, false))
        {
            logInfo("Shake hand stopped!");
            break;
        }
#endif

        if (pThread->getStopFlag()== true)
        {
            break;
        }
    }while (1);
}

void TCPServerProc(TCPServerThread *pThread)
{
    UINT16 tcpPort = pThread->getTCPServerPort();
    struct sockaddr_in server_addr;

    /* Create Socket */
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    logInfo("Enter TCPServerProc ...");
    if (serverSocket < 0)    
    {        
        logFatal("Create socket failed!");  
        logInfo("Leave TCPServerProc ...");
        return;
    }    
    int opt = 1;    
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;    
    server_addr.sin_port = htons(tcpPort);    
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY); 

    /* jetstream didn't support select function for "accept" in Block mode, so we have to use non-block mode for accept function.  */
    if ((fcntl(serverSocket, F_SETFL, O_NONBLOCK)) < 0)
    {
        logFatal("Failed to set the socket to non-block mode.");
        goto END;
    }
    
    if (bind(serverSocket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)    
    {        
        logFatal("Socket bind failed!");        
        goto END;    
    }    
    if (listen(serverSocket, 1) < 0)    
    {        
        logFatal("Socket listen failed!");
        goto END;    
    }    
    logInfo("TCPServerProc listening at port: %d...", tcpPort);

    while (1)
    {       
        if (pThread->getStopFlag()== true)
        {
            break;
        }

        /* Let's sleep for 1000ms */        
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));

        struct sockaddr_in client;        
        socklen_t client_addr_len = sizeof(client);     
//      if (isFdReadable(serverSocket, 1000))        
        {
            int workerSocket = accept(serverSocket, (struct sockaddr *)&client, &client_addr_len);
            if (workerSocket > 0)           
            {
                UINT32 * clientIP = (UINT32 *)g_server.getClientIP();
                logDebug("addr = 0x%x = 0x%x", client.sin_addr.s_addr, *clientIP);

                // We only accept the specified client ip address.
                if (client.sin_addr.s_addr != *clientIP)
                {
                    close(workerSocket);
                    continue;
                }

                char cli_ip[40] = "";
                inet_ntop(AF_INET, &client.sin_addr, cli_ip, INET_ADDRSTRLEN);
                logInfo("accept socket = %d, client ip=%s, port=%d", workerSocket, cli_ip, ntohs(client.sin_port));

                pThread->setConnectionStatus(STATUS_CONNECTED);
                logInfo("Enter TCPCommunicatonProc...")

                /* Only one connection is accept by TCP Server */
                TCPCommunicatonProc(pThread, workerSocket);
                
                logInfo("Leave TCPCommunicatonProc...")
                pThread->setConnectionStatus(STATUS_DISCONNECTED);
                close(workerSocket);
            }
        }
    }
END:
    close(serverSocket);
    logInfo("Quit TCPServerProc ...");
    return;
}
#endif


TCPServerThread::TCPServerThread()
{
    m_pThread = NULL;
    m_stopFlag = true;
    m_tcpPort = IR_PASSTHROUGH_PORT;
    m_communicationStatus = STATUS_DISCONNECTED;
}

TCPServerThread::~TCPServerThread()
{
    stop();
}
    
void TCPServerThread::start()
{
    if (m_pThread == NULL)
    {
        m_stopFlag = false;
        getMsgQueue()->clean();
        m_pThread = new std::thread(TCPServerProc, this);
    }
}

void TCPServerThread::stop()
{
    if (m_pThread != NULL)
    {
        m_stopFlag = true;
        
        m_pThread->join();
        delete m_pThread;
        m_pThread = NULL;
    }
}

bool TCPServerThread::getStopFlag() 
{ 
    return m_stopFlag; 
}

MsgQueue* TCPServerThread::getMsgQueue() 
{ 
    return (g_server.getMsgQueue()); 
}

UINT16 TCPServerThread::getTCPServerPort()
{
    return (m_tcpPort);
}

void TCPServerThread::setConnectionStatus(CommunicationStatusType status)
{
    m_communicationStatus = status;
}

CommunicationStatusType TCPServerThread::getConnectionStatus()
{
    return (m_communicationStatus);
}


