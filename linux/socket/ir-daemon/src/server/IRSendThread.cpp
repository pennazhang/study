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
#include <sys/ioctl.h>
#include "IRSendThread.h"
#include "ServerSetting.h"
#include "linux/lirc.h"
#include "typedef.h"
#include "jsIrUtils.h"
#include <math.h>

static const char* LIRC_DRIVER_DEVICE = "/dev/lirc0";

// The output pulse width and freq are not accruate so we made some adjustments to make the receive device happy.
// Need further investigation of this adjustment.
#define GPIO_IR_FREQ_ADJUST_COEFFICIENT     1.1
#define GPIO_IR_PULSE_ADJUST_COEFFICIENT    1.1

//
// Init the IR Send device. This function must be called before any IR Send command.
// This function can be called multi-times.
//
int initIRSendDevice(void)
{
    int irFd;
    static bool s_initFlag = false; /* TRUE: The device is already initlized. */
    int32_t send_mode = LIRC_MODE_PULSE;
    int32_t duty_cycle = 50;
    
    // Setup the lirc device.
    irFd = open(LIRC_DRIVER_DEVICE, O_WRONLY);
    if (irFd == -1) 
    {
        logFatal("Error: Cannot open device: %s", LIRC_DRIVER_DEVICE);
        return -1;
    }
    
    /* We only set Send Mode and Duty Cycle just once. */
    if (s_initFlag == true)
    {
        return irFd;
    }
    s_initFlag = true;

    ioctl(irFd, LIRC_SET_SEND_MODE, &send_mode);
    ioctl(irFd, LIRC_SET_SEND_DUTY_CYCLE, &duty_cycle);
    
    return irFd;
}

//
// Send the IR data through IR Tx Driver.
//
void sendIRRawData(int fd, int carryFreq, UINT32 *data, int count)
{
    // The count of pulse and space should be odd.
    if (((count & 1) == 0) || count <= 0)
    {
        logError("Invalid nLen of IR Signal: %d", count);
        count -= 1;
    }

    logInfo("Sending IR: %d Words at carry freq = %d Hz", count, carryFreq);
    dumpDecimal((UINT8 *)data, count * 4, sizeof(UINT32));

    int *sendData = new int[count];
    /* We need to adjust the width of pulse and space. */
    for (int i = 0; i < count; i++)
    {
        sendData[i] = round((double)data[i] / GPIO_IR_PULSE_ADJUST_COEFFICIENT);
    }

    /* We need to adjust the frequency */
    carryFreq = carryFreq * GPIO_IR_FREQ_ADJUST_COEFFICIENT;
    ioctl(fd, LIRC_SET_SEND_CARRIER, &carryFreq);
    
    write(fd, (UINT8 *)sendData, count * 4);

    delete []sendData;
}

#ifdef  IR_TX_DEBUG
//
// This is only used to debug the IR Tx process.
// Normally we send some standardize IR code (For example, Volume + / Volume -), and verify the result with IR Rx driver.
//
void sendIRTest(int method)
{
    UINT32 signal[][75] = 
    {
        /* Volume + */
        {   
            4500, 4500, 560, 1680, 560, 1680, 560, 1680, 560, 560, 560, 560, 560, 560, 560, 560, 
            560, 560, 560, 1680, 560, 1680, 560, 1680, 560, 560, 560, 560, 560, 560, 560, 560, 
            560, 560, 560, 1680, 560, 1680, 560, 1680, 560, 560, 560, 560, 560, 560, 560, 560, 
            560, 560, 560, 560, 560, 560, 560, 560, 560, 1680, 560, 1680, 560, 1680, 560, 1680, 
            560, 1680, 560, 560, 560, 560, 560, 560, 560, 560, 560 
        },
        
        /* Volume - */
        {
            4500, 4500, 560, 1680, 560, 1680, 560, 1680, 560, 560, 560, 560, 560, 560, 560, 560,
            560, 560, 560, 1680, 560, 1680, 560, 1680, 560, 560, 560, 560, 560, 560, 560, 560,
            560, 560, 560, 1680, 560, 1680, 560, 560, 560, 1680, 560, 560, 560, 560, 560, 560,
            560, 560, 560, 560, 560, 560, 560, 1680, 560, 560, 560, 1680, 560, 1680, 560, 1680,
            560, 1680, 560, 560, 560, 560, 560, 560, 560, 560, 560,
        },
    };
    
    int fd = initIRSendDevice();
    if (fd < 0)
    {
        logError("Failed to open the device!");
        return;
    }

    sendIRRawData(fd, 38400, signal[method], 75);
    close(fd);  
}

//
// You can add your test code here
// Notice: the test process should be quit gracefully when stop flag is set to TRUE.
//
void IRSendProc(IRSendThread *pThread)
{
    MsgQueue *msgQueue = pThread->getMsgQueue();
        
    logInfo("IRSendProc Enter...");
    for (int count = 0; ; count++)
    {
        if (pThread->getStopFlag())
        {
            break;
        }

#if 0       
        if (count & 1)
        {
            sendIRTest(0);
        }
        else
        {
            sendIRTest(1);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
#else
        for (;;)
        {
            int msgLen;
            UINT8 *msg = msgQueue->getMsg(msgLen);
            if (msg != NULL)
            {
                //  call pMsgQueue->putDemoData(15, 250) in TCPServerThread.cpp
                ASSERT(msgLen = 15 * 250);
                msgQueue->checkDemoData(msg, 15, 250);
                delete []msg;
            }
            else
            {
                break;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
#endif      
    }
    logInfo("IRSendProc quit...");
}

#else
//
// In this proc, we will get the IR code out of MsgQueue, and send it out through IR Tx Driver.
//
void IRSendProc(IRSendThread *pThread)
{
    int fd;
    MsgType* msg;
    int msgLen;
    UINT8 *data;
    int carryFreq;

    logInfo("Enter IRSendProc ...");
    MsgQueue *msgQueue = pThread->getMsgQueue();

    fd = initIRSendDevice();
    if (fd >= 0) 
    {
        for (; ; ) 
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            if (pThread->getStopFlag() == true)
            {
                break;
            }
            
            data = msgQueue->getMsg(msgLen);
            msg = (MsgType *)data;
            if (msg != NULL)
            {
                ASSERT((msg->m_header.m_msgDataLength + sizeof(MsgHeaderType)) == msgLen);
                IRRawHeaderType *pIRRawHeader = (IRRawHeaderType *)msg;
                if (pIRRawHeader->m_freq == 0)
                {
                    carryFreq = 38000;
                }
                else 
                {
                    // For the Pronto code, the frequency is stored in the Byte: m_header.m_reserved1
                    carryFreq = ProntoCodeType::freqToHz(pIRRawHeader->m_freq);
                }

                switch (msg->m_header.m_msgID)
                {
                    case MSG_ID_IR_RAW_ONCE:
                        sendIRRawData(fd, carryFreq, (UINT32 *)msg->m_data , (msg->m_header.m_msgDataLength) / 4 );
                        break;

                    case MSG_ID_IR_RAW_TEST:
                        {
                            if (g_irSetting.getTestStatus() != IR_TEST_START)
                            {
                                break;
                            }
                            g_irSetting.setTestStatus(IR_TEST_SEND_IR);
                            sendIRRawData(fd, carryFreq, (UINT32 *)msg->m_data , (msg->m_header.m_msgDataLength) / 4 );
                        }
                        break;

                    case MSG_ID_IR_RAW_REPEAT:
                        {
                            TimePoint currentTime;
                            currentTime.getCurrentTime();

                            /* Before sending the IR_RAW_REPEAT, we need to set the Repeat flag  */
                            g_irSetting.setRepeatFlag(TRUE);

                            for (;;)
                            {
                                /* Repeat time should not longer than MAX_IR_REPEAT_TIME_IN_MILLSECONDS */
                                if (currentTime.isTimeExpired(MAX_IR_REPEAT_TIME_IN_MILLSECONDS, false) == true)
                                {
                                    g_irSetting.setRepeatFlag(false);
                                    break;
                                }

                                /* send IR Data with carrier frequency */
                                sendIRRawData(fd, carryFreq, (UINT32 *)msg->m_data , (msg->m_header.m_msgDataLength) / 4 );

                                /* Let's delay Command Holdoff */
                                std::this_thread::sleep_for(std::chrono::milliseconds(g_irSetting.getCommandHoldOff()));

                                if (pThread->getStopFlag() == true)
                                {
                                    break;
                                }

                                /* We need to check the repeat flag. if false, then we can quit the repeat process */
                                if (g_irSetting.getRepeatFlag() == false)
                                {
                                    break;
                                }
                            }
                        }
                        break;

                    case MSG_ID_IR_RAW_REPEAT_COUNT:
                        {
                            IRRepeatCountHeaderType *header = (IRRepeatCountHeaderType *)msg;
                            for (int i = 0; i < header->m_repeatCount; i++)
                            {
                                /* send IR Data with carrier frequency */
                                sendIRRawData(fd, carryFreq, (UINT32 *)msg->m_data , (msg->m_header.m_msgDataLength) / 4 );

                                /* Let's delay Command Holdoff */
                                std::this_thread::sleep_for(std::chrono::milliseconds(g_irSetting.getCommandHoldOff()));

                                if (pThread->getStopFlag() == true)
                                {
                                    break;
                                }
                            }
                        }
                        break;

                    case MSG_ID_IR_RAW_REPEAT_TIME:
                        {
                            IRRepeatTimeHeaderType *header = (IRRepeatTimeHeaderType *)msg;
                            UINT32 ulTimeInMS = header->m_repeatTime * 100;
                            TimePoint currentTime;
                            currentTime.getCurrentTime();

                            for (;;)
                            {
                                /* Repeat time should not longer than MAX_IR_REPEAT_TIME_IN_MILLSECONDS */
                                if (currentTime.isTimeExpired(ulTimeInMS , false) == true)
                                {
                                    break;
                                }

                                /* send IR Data with carrier frequency */
                                sendIRRawData(fd, carryFreq, (UINT32 *)msg->m_data , (msg->m_header.m_msgDataLength) / 4 );

                                /* Let's delay Command Holdoff */
                                std::this_thread::sleep_for(std::chrono::milliseconds(g_irSetting.getCommandHoldOff()));

                                if (pThread->getStopFlag() == true)
                                {
                                    break;
                                }
                            }
                        }
                }
                
                delete []data;
            }           
        }
        close(fd);
    }
    else
    {
        logError("Failed to initialize the IR Tx device!");
    }
    logInfo("Quit IRSendProc ...");
    return;
}

#endif

IRSendThread::IRSendThread()
{
    m_pThread = NULL;
    m_stopFlag = true;
}

IRSendThread::~IRSendThread()
{
    stop();
}

//
// create a thread and run the IR tx proc.
//  
void IRSendThread::start()
{
    if (m_pThread == NULL)
    {
        m_stopFlag = false;
        getMsgQueue()->clean();
        m_pThread = new std::thread(IRSendProc, this);
    }
}

//
// Set the stop flag for the IR Tx thread, and wait for it to stop.
// Cautious: This function is blocked for the stop of IR Tx thread.
//
void IRSendThread::stop()
{
    if (m_pThread != NULL)
    {
        // Set the stop flag
        m_stopFlag = true;
        
        /* just wait for the IR Tx thread to stop, then we can go on */
        m_pThread->join();
        delete m_pThread;
        m_pThread = NULL;
    }
}

bool IRSendThread::getStopFlag() 
{ 
    return m_stopFlag; 
}

//
// MsgQueue is the only share object that used by IR Tx thread.
//
MsgQueue* IRSendThread::getMsgQueue() 
{ 
    return (g_server.getMsgQueue());
}


