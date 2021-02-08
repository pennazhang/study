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

#include <thread>
#include <string.h>
#include "typedef.h"
#include "IRDataBuffer.h"

IRDataBuffer::IRDataBuffer()
{
    
}

IRDataBuffer::~IRDataBuffer()
{
    ir_clean();
}

//
// Clear all the data in the list
//
void IRDataBuffer::ir_clean()
{
    for (auto const& pRPMSG: m_IRDataList) 
    {
        UINT8 *data = (UINT8 *)pRPMSG;
        delete []data;
    }

    m_IRDataList.clear();
}

//
// Normally, it won't be called. It is just for some unknown bug in case. 
// For example, when we found that the frameID is not continous, then we will delete all the uncontinous frame until a new frame is begin.
//
void IRDataBuffer::ir_revoverIRData()
{
    MsgType * pRpmsg;
    RpmsgHeaderType *header;
    std::list<MsgType*>::iterator it;

    for (;;) 
    {
        it = m_IRDataList.begin();
        if (it == m_IRDataList.end())
        {
            /* It is the end of the IR List */
            return;
        }

        pRpmsg = *it;
        header = (RpmsgHeaderType *)pRpmsg;
        if ((header->m_sequenceID == 0xFF) || (header->m_sequenceID == 0))
        {
            /* It is the begining of the IR Signal */
            return;
        }
        else
        {
            /* Let's delete it from the list */
            UINT8 *pData = (UINT8 *)pRpmsg;
            delete []pData;
            m_IRDataList.pop_front();
        }
    }
}

//
// Return Value: the size of the IR signal payload (not including RPMSG_Header) 
//
int IRDataBuffer::ir_getIRDataLen()
{
    bool foundEnd = false;
    int irMsgLen = 0;
    UINT8 frameID = 0xFF;

    for (auto const& pRPMSG: m_IRDataList) 
    {
        RpmsgHeaderType *header = ( RpmsgHeaderType *)pRPMSG;

        irMsgLen += header->m_msgDataLength;
        if (header->m_sequenceID == 0xFF)
        {
            /* 0XFF means the end of the IR Recv Signal */
            foundEnd = true;
            break;
        }
        else
        {
            frameID++;
            if (frameID == 0xFF)
            {
                /* OxFF means the end of the IRRecvData */
                frameID = 0;
            }
            
            if (frameID != header->m_sequenceID)
            {
                logFatal("Fatal Error: Invalid frameID found!");
                ir_revoverIRData();
                return (0);
            }
        }
    }

    if (foundEnd == true)
    {
        logDebug("readLen = %d", irMsgLen);
        return (irMsgLen);
    }
    else
    {
        return (0);
    }
}

//
// integrate several RPMSG into a completed IR signal.
// Normally the RPMSG has a limitation of 488 Bytes. so we has to use several RPMSG to transfer a completed IR signal.
//
MsgType * IRDataBuffer::ir_getIRData()
{
    int irMsgLen = sizeof(MsgHeaderType), dataSize;
    MsgType * pRpmsg;
    std::list<MsgType*>::iterator it;
    UINT8 sequenceID;
    RpmsgHeaderType *header;
    
    /* We need to check the total size of the IR signal first */
    dataSize = ir_getIRDataLen();
    if (dataSize == 0)
    {
        return (NULL);
    }
    else
    {
        UINT8 *newBuffer = new UINT8[sizeof(MsgHeaderType) + dataSize];
        
        for (;;) 
        {
            it = m_IRDataList.begin();
            if (it == m_IRDataList.end())
            {
                logError("Fatal Internal Error: unexpected end in the list!");
                break;
            }
            pRpmsg = *it;
            header = (RpmsgHeaderType *)pRpmsg;
            memcpy(newBuffer + irMsgLen, pRpmsg->m_data, pRpmsg->m_header.m_msgDataLength);
            irMsgLen += pRpmsg->m_header.m_msgDataLength;
            sequenceID =  header->m_sequenceID;
            if (sequenceID == 0xFF)
            {
                /* Fill the IR message Head with the header of late frame RPMSG */
                memcpy(newBuffer, pRpmsg, sizeof(MsgHeaderType));
            }

            /* We must free the old PRMSG */
            UINT8 *pData = (UINT8 *)pRpmsg;
            delete []pData;
            m_IRDataList.pop_front();

            logDebug("Get sequenceID = 0x%x", sequenceID);
            if (sequenceID == 0xFF)
            {
                /* The end of RPMSG */
                break;
            }
        }

        ASSERT (irMsgLen == dataSize + sizeof(MsgHeaderType));

        /* Rewrite RPMSG_Header */
        pRpmsg = (MsgType *)newBuffer;
        pRpmsg->m_header.m_msgDataLength = dataSize;
        pRpmsg->m_header.m_msgID = MSG_ID_IR_RAW_ONCE;
        IRRawHeaderType *IRRawHeader = (IRRawHeaderType *)pRpmsg;
        IRRawHeader->m_freq = 0;        /* 0 means default frequency: 38400 Hz */

        /* Recalcate the checksum */
        UINT8 checkSum = 0;
        UINT8 *data = pRpmsg->m_data;
        for (int i = 0; i < dataSize; i++)
        {
            checkSum ^= data[i];
        }
        pRpmsg->m_header.m_msgDataCheckSum = checkSum;
        return ((MsgType *)newBuffer);  
    }
}

//
// Put the frame of RPMSG into the buffer.
//
void IRDataBuffer::ir_putIRData(MsgType *pRpmsg)
{
    int dataLen = pRpmsg->m_header.m_msgDataLength;
    UINT8 *pMsg = new UINT8[sizeof(MsgHeaderType) + dataLen];
    memcpy(pMsg, pRpmsg, sizeof(MsgHeaderType) + dataLen);
    m_IRDataList.push_back((MsgType *)pMsg);
}

#ifdef IR_RECV_UNIT_TEST
//
// Unit test to ensure that no memory is leak.
//
void IRDataBuffer::ir_putDemoData(int frameCount, int dataLen)
{
    UINT8 pData[512];

    MsgType *pRpmsg = (MsgType *)pData;
    int i = 0; 

    memset(pData, frameCount, 512);
    
    for (i = 0; i < frameCount; i++)
    {
        if (i == (frameCount - 1))
        {
            pRpmsg->m_header.m_sequenceID = 0xFF;
        }
        else
        {
            pRpmsg->m_header.m_sequenceID = i;
        }
        pRpmsg->m_header.m_msgDataLength = dataLen;
        ir_putIRData(pRpmsg);
    }
}
#endif

MsgType* IRDataBuffer::getIRData()
{
    MsgType *rpmsg;
    
    m_mtx.lock();
    rpmsg = ir_getIRData();
    m_mtx.unlock();

    return (rpmsg);
}

void IRDataBuffer::putIRData(MsgType *pRpmsg)
{
    m_mtx.lock();
    ir_putIRData(pRpmsg);
    m_mtx.unlock();
}


void IRDataBuffer::clean()
{
    m_mtx.lock();
    ir_clean();
    m_mtx.unlock();
}

#ifdef  IR_RECV_UNIT_TEST
STATUS IRDataBuffer::ir_test()
{
    int msgLen;
    static int frameCount = 1;
    STATUS ret = STATUS_ERROR;
    const int msgSize = 420;
    
    ir_clean();
    
    frameCount++;
    if (frameCount == 20)
    {
        frameCount = 1;
    }

    /* First we need to put some data into the List */
    ir_putDemoData(frameCount, msgSize);

    /* Then we will get them back from the List and check the size */
    MsgType* rpmsg = ir_getIRData();
    if (rpmsg != NULL)
    {
        if (rpmsg->m_header.m_msgDataLength != msgSize * frameCount)
        {
            logDebug("getIRDataLen return invalid! %d != %d", msgLen, msgSize * frameCount); 
        }
        else
        {
            ret = STATUS_OK;
        }
    }
    else
    {
        logError("Failed to get RPMSG from the List."); 
    }

    if (rpmsg != nullptr)
    {
        delete []rpmsg;
    }
    return (ret);
}


void IRDataBuffer::operator()(void)
{
    int i;
#if 0
    static int count = 0;
    while (true)
    {
        count++;
        logDebug("Count = %d", count);
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
#endif

    for (int count = 0; ; count++)
    {
        for (i = 0; i < 100000; i++)
        {
            if (ir_test() == STATUS_ERROR)
            {
                logDebug("Test Failed!");
                return;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        logInfo("Test ok, count = %d", count);
    }

    logDebug("Test finished, total success = %d", i);
}

STATUS IRDataBuffer::test()
{
    STATUS ret;
    
    m_mtx.lock();
    ret = ir_test();
    m_mtx.unlock();

    return (ret);
}

void IRDataBuffer::putDemoData(int frameCount, int dataLen)
{
    m_mtx.lock();
    ir_putDemoData(frameCount, dataLen);
    m_mtx.unlock();
}

#endif
