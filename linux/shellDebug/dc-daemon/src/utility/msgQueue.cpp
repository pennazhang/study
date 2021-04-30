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


#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "msgQueue.h"
#include "utility.h"

MsgQueue::MsgQueue()
{
}

MsgQueue::~MsgQueue()
{
    _clean();
}

//
// clean all the data in the Queue. 
//
void MsgQueue::_clean()
{
    for (auto const& node: m_msgList) 
    {
        UINT8 *data = (UINT8 *)node.m_data;
        delete []data;
    }

    m_msgList.clear();
}

void MsgQueue::_putMsg(UINT8 *data, int startIndex, int validDataLen, bool endFlag)
{
    NodeType node;
    
    node.m_validDataLen = validDataLen;
    node.m_endFlag = endFlag;
    node.m_data = data;
    node.m_startIndex = startIndex;
    
    m_msgList.push_back(node);
}

/**
 *  Get the length of the next completed package. Normally a completed package includes several frames of Msg.
 *
 *  @param [out] frameCount -- Output of the frame number which can be integrated into a sinale completed package.
 *
 *  @return length of the completed package, 0=none
 */
/*---------------------------------------------------------------------------*/
int MsgQueue::_getMsgLen(int & frameCount)
{
    int msgLen = 0;
    frameCount = 0;
    
    for (auto const& node: m_msgList) 
    {
        msgLen += node.m_validDataLen;
        frameCount++;
        if (node.m_endFlag == true)
        {
            // this is the last frame of the completed package.  
            return (msgLen);
        }
    }
    return (0);
}

/**
 *  Get a completed package from the queue. Normally a completed package includes several frames of Msg.
 *
 *  @param [out] msgLen -- length of the completed package.
 *
 *  @return pointer of the completed package, NULL=none.
 */
/*---------------------------------------------------------------------------*/
UINT8 *MsgQueue::_getMsg(int &msgLen)
{
    int frameCount;
    UINT8 *msgData;
    NodeType node;
    
    /* Check if the next completed package exist */
    msgLen = _getMsgLen(frameCount);
    if (msgLen == 0)
    {
        return (NULL);
    }
    
    /* The package only include 1 frame of Msg */
    if (frameCount == 1)
    {
        node = m_msgList.front();
        ASSERT(node.m_endFlag == true);
        ASSERT(node.m_validDataLen == msgLen);

        if (node.m_startIndex != 0)
        {
            /* we need to delete the msg header */
            msgData = new UINT8[msgLen];
            memcpy(msgData, node.m_data + node.m_startIndex, node.m_validDataLen);
            delete []node.m_data;
            m_msgList.pop_front();
            return (msgData);
        }
        else
        {
            msgData = node.m_data;
            m_msgList.pop_front();
            return (msgData);
        }
    }
    else
    {
        /* The completed package includes several frames of msg */
        int len = 0;
        msgData = new UINT8[msgLen];

        for (int frameIndex = 0; frameIndex < frameCount; frameIndex++) 
        {
            node = m_msgList.front();
            memcpy(msgData + len, node.m_data + node.m_startIndex, node.m_validDataLen);
            len += node.m_validDataLen;
            delete []node.m_data;
            m_msgList.pop_front();
        }
        ASSERT(len == msgLen);
    }
    
    return (msgData);
}

void MsgQueue::_putDemoData(int frameCount, int frameLen)
{
    UINT8 *frameData;
    int startIndex;
    for (int frameIndex = 0; frameIndex < frameCount; frameIndex++)
    {
        frameData = new UINT8[frameLen + frameCount];
        memset(frameData, 0, frameLen + frameCount);
        startIndex = frameIndex;
        memset(frameData + startIndex, startIndex, frameLen);
        _putMsg(frameData, startIndex, frameLen, (frameIndex == (frameCount - 1)) ? true: false);
    }
}

void MsgQueue::putMsg(UINT8 *data[], int startIndex[], int validateDataLen[], int frameCount)
{
    bool endFlag = false;
    m_mtx.lock();
    for (int frameIndex = 0; frameIndex < frameCount; frameIndex++)
    {
        if (frameIndex == (frameCount - 1))
        {
            endFlag = true;
        }       
        _putMsg(data[frameIndex], startIndex[frameIndex], validateDataLen[frameIndex], endFlag);
    }
    m_mtx.unlock();     
}

void MsgQueue::putMsg(UINT8 *data, int startIndex, int dataLen, bool endFlag)
{
    m_mtx.lock();
    _putMsg(data, startIndex, dataLen, endFlag);
    m_mtx.unlock(); 
}

UINT8 *MsgQueue::getMsg(int &len)
{
    UINT8 *msg;
    m_mtx.lock();
    msg = _getMsg(len);
    m_mtx.unlock(); 
    
    return (msg);
}

void MsgQueue::clean()
{
    m_mtx.lock();
    _clean();
    m_mtx.unlock(); 
}

#ifdef  MSGQUEUE_UNIT_TEST
void MsgQueue::putDemoData(int frameCount, int dataLen)
{
    m_mtx.lock();
    _putDemoData(frameCount, dataLen);
    m_mtx.unlock();
}

STATUS MsgQueue::test()
{
    m_mtx.lock();
    _test();
    m_mtx.unlock();
}

STATUS MsgQueue::checkDemoData(UINT8 *msg, int frameCount, int frameLen)
{
    int msgLen = 0;
    
    for (int frameIndex = 0; frameIndex < frameCount; frameIndex++)
    {
        for (int len = 0; len < frameLen; len++, msgLen++)
        {
            if (msg[msgLen] != frameIndex)
            {
                logInfo("_getDemoData failed, index = %d, write: %d, read: %d", msgLen, frameIndex, msg[msgLen]);
                return (STATUS_ERROR);
            }
        }
    }
    
    return (STATUS_OK);
}

#define TEST_FRAME_COUNT 10
#define TEST_FRAME_LEN  250
STATUS MsgQueue::_test()
{
    STATUS ret = STATUS_ERROR;
    int msgLen;
    UINT8 *data = NULL;

    _putDemoData(TEST_FRAME_COUNT, TEST_FRAME_LEN);
    data = _getMsg(msgLen);
    
    if ((data == NULL) || (msgLen != TEST_FRAME_COUNT * TEST_FRAME_LEN))
    {
        logDebug("_getDemoData failed, msgLen = %d", msgLen);
        goto END;
    }
    
    /* Check the content of the received message */
    ret = checkDemoData(data, TEST_FRAME_COUNT, TEST_FRAME_LEN);

END:
    if (data != NULL)
    {
        delete []data;
    }
    return (ret);
}
#endif