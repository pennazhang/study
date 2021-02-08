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
#include "IRRecvThread.h"
#include "ClientSetting.h"
#include "config.h"
#include "jsIrUtils.h"

//
// Function: initialize the IR Rx device, and return the device handle. -1 means initialization failed.
// This function can be called multi-times.
//
int initDevice(void)
{
    static bool s_initFlag = false; /* TRUE: The device is already initlized. */
    int fd;
    struct termios ti;
    const char fileName[] =  "/dev/ttyRPMSG30";
    
    fd = open(fileName, O_RDWR | O_NOCTTY);
    if (fd < 0) {
        logError("Failed to open %s, error %d", fileName, fd);
        return -1;
    }
    if (s_initFlag == true)
    {
        return fd;
    }
    s_initFlag = true;

    tcflush(fd, TCIOFLUSH);
    if (tcgetattr(fd, &ti) < 0) 
    {
        logFatal("Failed to set option for %s", fileName);
        close(fd);
        return -1;
    }

    cfmakeraw(&ti);
    cfsetospeed(&ti, B115200);
    cfsetispeed(&ti, B115200);
    if (tcsetattr(fd, TCSANOW, &ti) < 0) 
    {
        logFatal("Failed to set option for %s", fileName);
        close(fd);
        return -1;
    }

    struct termios tio;
    tio.c_cc[VTIME] = 1; // timeout in deciseconds for noncanonical read
    tio.c_cc[VMIN] = 0; // minimum number of characters for noncanonical read
    tcsetattr(fd, TCSANOW, &tio);

    return fd;
}

#ifdef IR_RECV_UNIT_TEST

void IRRecvTask(IRRecvThread *pThread)
{
    IRDataBuffer *pIRDataBuffer = pThread->getIRDataBuffer();
    
    logInfo("IRRecvTask Enter...");
    for (int count = 0; ; count++)
    {
        if ((count & 0xfff) == 0)
        {
            logDebug("IRRecvTask Send = 0x%x", count);
        }
        if (pThread->getStopFlag())
        {
            logInfo("IRRecvTask Quit...");
            break;
        }

        pIRDataBuffer->putDemoData(15, 420);
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}
#else

//
// We will create a thread to try and get data from M4 through RPMSG
// If the TCP Client connection is established, then we will put the IR Received data to IRDataBuffer.
//

void IRRecvTask(IRRecvThread *pThread)
{
    int fd, result;
    UINT8 data[MAX_RPMSG_BUFFER_SIZE + sizeof(MsgHeaderType)];
    RpmsgHeaderType* header = (RpmsgHeaderType*)data;
    UINT8 *body = &data[sizeof(MsgHeaderType)];
    int i = 0;
    static int s_IRDataCount = 0;
    static UINT8 s_lastSubMsgID = 0xFF;
    IRDataBuffer *pIRDataBuffer;

    fd = initDevice();
    
    if (fd >= 0) 
    {
        logInfo("Enter IRRecvTask ...");
        
        for (; ; ) 
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            if (pThread->getStopFlag() == true)
            {
                break;
            }
            
            if (!isFdReadable(fd, 1))
            {
                continue;
            }

            /* Read the msg Header */
            result = read(fd, (void *)header, sizeof(MsgHeaderType));

            if (result == 0)
            {
                /* No data is received. */
                continue;
            }
            
            if (result != sizeof(MsgHeaderType))
            {
                logError("recv: Error, read result = %d, count = %d", result, i++);
                continue;
            }

            // Check the Message Header one by one.
            if (header->m_sendModuleID != MODULE_ID_M4)
            {
                logError("Invalid sendModuleID");
                continue;
            }
            if (header->m_recvModuleID != MODULE_ID_TTY_DRIVER)
            {
                logError("Invalid m_recvModuleID");
                continue;
            }
            if (header->m_msgID != MSG_ID_RPMSG_RECVING_IR)
            {
                logError("Invalid msgID!");
                continue;
            }
            if (header->m_msgDataLength > MAX_RPMSG_BUFFER_SIZE)
            {
                logError("Invalid msgDataLength!");
                continue;
            }
            
            /* Read the msg body */
            result = read(fd, body, header->m_msgDataLength);
            if (result != header->m_msgDataLength)
            {
                logError("recv body Error, result = %d", result);
                continue;
            }

            // Check the checksum.
            UINT8 checkSum = 0;
            for (i = 0; i < result; i++)
            {
                checkSum ^= body[i];
            }
            if (checkSum != header->m_msgDataCheckSum)
            {
                logError("checksum Error!");
                continue;
            }
            
            int wordLen =  header->m_msgDataLength / 4;
            s_IRDataCount += wordLen;
            
            logInfo("Recv RPMSG %d Words", wordLen);

            /* Put the RPMSG to IRDataBuffer */
            pIRDataBuffer = pThread->getIRDataBuffer();
            if (pIRDataBuffer != NULL)
            {
                pIRDataBuffer->putIRData((MsgType *)data);
            }

            /* Check the Frame index */
            if (header->m_sequenceID == 0xFF)
            {
                // Last Frame of IR data.
                logInfo("End of IR received Data, Effective Data Count: %d", s_IRDataCount);
                s_IRDataCount = 0;

                // Get the Whole IR Message.
                MsgType * msg = pIRDataBuffer->getIRData();
                if (msg != NULL)
                {
                    dumpIRRaw(msg);

                    // This is a IR Message under test.
                    if (g_irSetting.getTestStatus() == IR_TEST_SEND_IR)
                    {
                        // Receive IR signal under test, and the IR signal is just for test.
                        g_irSetting.setTestStatus(IR_TEST_RECV_IR);
                        g_irSetting.setTestResult(msg);
                    }

                    // We will send it to the TCPServerThread
                    if (g_client.getConnectionStatus() == STATUS_CONNECTED)
                    {
                        MsgQueue *msgQueue = g_client.getMsgQueue();
                        msgQueue->putMsg((UINT8 *)msg, 0, msg->m_header.m_msgDataLength + sizeof(MsgHeaderType), true);
                    }
                    else
                    {
                        // If no connection, then we will discard all data in IRDataBuffer.
                        UINT8 *data = (UINT8 *)msg;
                        delete []data;
                    }
                }
            }
            else
            {
                UINT8 nextSumMsgID = s_lastSubMsgID + 1;
                if (nextSumMsgID != header->m_sequenceID)
                {
                    logError("Invalid msgSubID, s_lastSubMsgID = %d, m_sequenceID = %d", s_lastSubMsgID, header->m_sequenceID);
                    continue;
                }
            }
            s_lastSubMsgID = header->m_sequenceID;
            
        }
        close(fd);
    }

    logInfo("Quit IRRecvTask...");
    return;
}

#endif

IRRecvThread::IRRecvThread()
{
    m_pThread = NULL;
    m_stopFlag = true;
}

IRRecvThread::~IRRecvThread()
{
    stop();
}
    
void IRRecvThread::start()
{
    if (m_pThread == NULL)
    {
        m_stopFlag = false;
        getIRDataBuffer()->clean();
        m_pThread = new std::thread(IRRecvTask, this);
    }
}

void IRRecvThread::stop()
{
    if (m_pThread != NULL)
    {
        m_stopFlag = true;
        
        m_pThread->join();
        delete m_pThread;
        m_pThread = NULL;
    }
}

bool IRRecvThread::getStopFlag() 
{ 
    return m_stopFlag; 
}

IRDataBuffer* IRRecvThread::getIRDataBuffer() 
{ 
    return (&m_IRDataBuffer); 
}
