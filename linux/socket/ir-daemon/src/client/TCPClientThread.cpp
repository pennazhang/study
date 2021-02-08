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
#include <string.h>
#include <unistd.h>
#include <curses.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/select.h> 
#include <sys/time.h> 
#include <sys/socket.h>

#include "TCPClientThread.h"
#include "ServerSetting.h"
#include "ClientSetting.h"
#include "jsIrUtils.h"

#if 1

void TCPCommunicationProc(TCPClientThread *pThread, int workerSocket)
{
    char buffer[1024] = "";
    MsgType *pIRSignal;
    int recvLen, bufferLen;
    
#ifdef  COMMUNICATOIN_SHAKE_HAND
    TimePoint currentTime, lastRecvDataTime;
#endif

    MsgQueue *msgQueue = pThread->getMsgQueue();
    msgQueue->clean();
    
    logInfo("TCP Client socket is working now...");

    while(1)
    {
        //  Let's sleep for 10ms
        std::this_thread::sleep_for(std::chrono::milliseconds(10));     
                
        if (pThread->getStopFlag()== true)
        {
            break;
        }

        /* Recv something from Server */
        bool ret = isFdReadable(workerSocket, 1);
        if (ret == true)
        {
            recvLen = recv(workerSocket, buffer, sizeof(buffer), 0);
            if (recvLen <= 0)
            {
                logInfo("client:%d socket closed!", workerSocket);
                break;
            }

            // Dispatch Data from Server Socket.
/*          
            logDebug("Recv %d Bytes.", recvLen);
            buffer[recvLen] = 0;
            logDebug("->%s", buffer);
*/

#ifdef  COMMUNICATOIN_SHAKE_HAND            
            lastRecvDataTime.getCurrentTime();
#endif
        }
#if 1
        /* Check MsgQueue, and send it out to Server */
        int msgLen;
        pIRSignal = (MsgType *)msgQueue->getMsg(msgLen);
        if (pIRSignal != NULL)
        {
            bufferLen = sizeof(MsgHeaderType) + pIRSignal->m_header.m_msgDataLength;
            logDebug("Client send IR len: 8 + %d", pIRSignal->m_header.m_msgDataLength);
        
            UINT8 *tempBuffer = (UINT8 *)pIRSignal;
            send(workerSocket, tempBuffer, bufferLen, 0);
            delete []tempBuffer;
        }
#endif          

#ifdef COMMUNICATOIN_SHAKE_HAND
        if (currentTime.isTimeExpired(SHAKE_HAND_INTERVAL, true))
        {
            MsgType msg;
            msg.m_header.m_msgID = MSG_SOCKET_SHAKE_HAND;
            msg.m_header.m_msgDataLength = 0;
            msg.m_header.m_msgDataCheckSum = 0;
            send(workerSocket, (void *)&msg, sizeof(MsgHeaderType), 0);
        }
        
        if (lastRecvDataTime.isTimeExpired(SHAKE_HAND_CHECK_INTERVAL, false))
        {
            logInfo("Shake hand stopped!");
            break;
        }
#endif      
    }
    logWarning("TCP Client socket is broken now...");   
}

STATUS waitForConnection(int sock_fd, int waitTimeInSecond)
{
    int ret;

    /* set select() time out */
    struct timeval tv;
    tv.tv_sec = waitTimeInSecond; 
    tv.tv_usec = 0;
                    
    fd_set wfds;
    FD_ZERO(&wfds);
    FD_SET(sock_fd, &wfds);
            
    ret = select(sock_fd + 1, NULL, &wfds, NULL, &tv);
    if (ret > 0)
    {
        int errinfo;
        socklen_t errlen;
        if (-1 == getsockopt(sock_fd, SOL_SOCKET, SO_ERROR, &errinfo, &errlen))
        {
            logInfo("getsockopt SO_ERROR Failed.");
        }
        else if (0 == errinfo)
        {
            return (STATUS_OK);
        }
        else
        {
            logInfo("getsockopt return errinfo = %d.", errinfo);
        }
    }
    else if (ret == 0)
    {
        logInfo("Connection Timeout!");
    }
    else
    {
        logInfo("select error!");
    }
    return (STATUS_ERROR);  
}

void TCPClientProc(TCPClientThread *pThread)
{
    UINT8 *serverIP;
    struct sockaddr_in server_sock;
    char lpszServerIP[20];
    
    serverIP = g_client.getServerIP();
    sprintf(lpszServerIP, "%d.%d.%d.%d", serverIP[0], serverIP[1], serverIP[2], serverIP[3]);
    logInfo("Enter TCPClientProc ...");
    
    for (;;)
    {
        int workerSocket = socket(AF_INET, SOCK_STREAM, 0);
        bzero(&server_sock, sizeof(server_sock));
        server_sock.sin_family = AF_INET;
        inet_pton(AF_INET, lpszServerIP, &server_sock.sin_addr);
        server_sock.sin_port=htons(IR_PASSTHROUGH_PORT);

        int flags = fcntl(workerSocket, F_GETFL, 0);
        if (flags < 0)
        {
            logError("Failed to get flag of socket.");
            return ;
        }
            
        flags |= O_NONBLOCK;
        if (fcntl(workerSocket, F_SETFL, flags) < 0)
        {
            logError("Failed to set socket to NON-BLOCK mode.");
            return;
        }

        logInfo("Connecting to %s:%d...", serverIP, IR_PASSTHROUGH_PORT);

        for (;;)
        {
            bool connectFlag = false;
            int ret;
            
            /* Let's sleep for 1000ms */        
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));       

            ret = connect(workerSocket, (struct sockaddr *)&server_sock, sizeof(server_sock));
            if (ret < 0)
            {
                if (errno == EINPROGRESS)
                {
                    if (waitForConnection(workerSocket, 2) == STATUS_OK)
                    {
                        connectFlag = true;
                    }
                }
                else
                {
                    logDebug("connect failed, errno = %d", errno);
                }
            }
            else
            {
                connectFlag = true;
            }
            
            if (connectFlag == true)
            {
                // Connection is established.
                // Set socket to block mode.
                flags &= ~O_NONBLOCK;
                if (fcntl(workerSocket, F_SETFL, flags) < 0)
                {
                    logError("Failed to set socket to BLOCK mode");
                    break;
                }

                logInfo("connect to server success.");
                pThread->setConnectionStatus(STATUS_CONNECTED);
                TCPCommunicationProc(pThread, workerSocket);
                pThread->setConnectionStatus(STATUS_DISCONNECTED);
                break;
            }
            
            if (pThread->getStopFlag()== true)
            {
                break;
            }
        }
        close(workerSocket);
        
        if (pThread->getStopFlag()== true)
        {
            break;
        }
    }
    logInfo("Quit TCPClientProc ...");  
}

#else
void TCPClientProc(TCPClientThread *pThread)
{
    MsgQueue *msgQueue = pThread->getMsgQueue();
    
    logInfo("TCPClientProc Enter...");
    for (int count = 0; ; )
    {
        if (pThread->getStopFlag())
        {
            break;
        }

        MsgType *pRpmsg = (MsgType *)msgQueue->getMsg();
        if (pRpmsg != NULL)
        {
            int payLoadLen = pRpmsg->m_header.m_msgDataLength;
            UINT32 *value = (UINT32 *)pRpmsg->m_data;
            for (int i = 0; i < payLoadLen / 4; i++)
            {
                logDebug("%d", value[i]);
            }
            logDebug("TCPClientProc received Data Count: %d", payLoadLen / 4);
            UINT8 *pData = (UINT8 *)pRpmsg;
            delete []pData;
        }
    
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
END:    
    logInfo("TCPClientProc Quit...");
}
#endif

TCPClientThread::TCPClientThread()
{
    m_pThread = NULL;
    m_stopFlag = true;
}

TCPClientThread::~TCPClientThread()
{
    stop();
}
    
void TCPClientThread::start()
{
    if (m_pThread == NULL)
    {
        m_stopFlag = false;
        getMsgQueue()->clean();
        m_pThread = new std::thread(TCPClientProc, this);
    }
}

void TCPClientThread::stop()
{
    if (m_pThread != NULL)
    {
        m_stopFlag = true;
        
        m_pThread->join();
        delete m_pThread;
        m_pThread = NULL;
    }
}

bool TCPClientThread::getStopFlag() 
{ 
    return m_stopFlag; 
}

MsgQueue* TCPClientThread::getMsgQueue()
{ 
    return (g_client.getMsgQueue()); 
}

void TCPClientThread::setConnectionStatus(CommunicationStatusType status)
{
    m_communicationStatus = status;
}

CommunicationStatusType TCPClientThread::getConnectionStatus()
{
    return (m_communicationStatus);
}



