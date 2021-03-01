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


#ifndef _MSG_QUEUE_H_
#define _MSG_QUEUE_H_

#include "typedef.h"
#include <list>
#include <mutex>          // std::mutex

//
// Unit test for MsgQueue
// We must ensure that no memory is leak in the test.
//
//#define MSGQUEUE_UNIT_TEST

//
// The structure should be aligned with Byte. 
//
#pragma pack (1)
typedef struct 
{
// It's the valid data length which start from m_data[m_startIndex]
// Normally, sizeof(m_data) = m_startIndex + m_validDataLen 
//
    int m_validDataLen;
//
// The valid start index. 
// For example, we received a frame of RPMSG and copy them to m_data, but in fact, we don't need the frame head.
// So we can set the startIndex to the frame body
//
    int m_startIndex;

//
// We can group several Nodes into a integrated msg. This flag indicate the last frame of the msg.
// For example, each RPMSG has 488 Bytes limited, so we can use several RPMSG to transfer a 2K IR signal.
// Each RPMSG can be filled into a NodeType.
// And the last Node should mark m_endFlag as TRUE.
// 
    bool m_endFlag;

// Each message can be filled into m_data including the header, which can be discard by setting m_startIndex as sizeof(MSG_HEADER).
    UINT8 *m_data;
} NodeType;
#pragma pack ()

//
// This class will be used between IR Tx thread and TCP Server Thread, so it must use mutex as critical section locker.
// The TCP Server will receive the IR signal from client, and put them into this queue.
// Also the Dbus thread will receive IR pronto code from webserver and put the code into this queue.
// The IR Tx Thread will get the IR signal from this queue, and send them out through IR Tx Driver.
//
// For example:
//  
//  data1 = new UINT8[108];
//  msgQueue->putMsg(data1, 8, 100, false);
//  data2 = new UINT8[216];
//  msgQueue->putMsg(data1, 16, 200, false);
//  int packageLen;
//  UINT8 *newData = msgQueue->getMsg(packageLen);
//  ASSERT(packageLen == 300);
//  /* Then we will got newData with combination of data1[8..107] + data2[16..215] */
//  delete []newData;
//

class MsgQueue
{
public:
    MsgQueue();
    ~MsgQueue();

// 
//  The data must be called by new function. If not, system will crash.
//  For example:
//
//  int dataLen = msgHeader.m_msgDataLength;
//  UINT8 *data = new UINT8[dataLen + sizeof(MsgHeaderType)];
//  memcpy(data, &msgHeader, sizeof(MsgHeaderType));
//
//  /* Then, we will read the message body */
//  recvLen = blockRecv(workerSocket, data + sizeof(MsgHeaderType), dataLen, 5000);
//  if (recvLen != dataLen)
//  {               
//      logInfo("client:%d socket closed!", workerSocket);
//      break;
//  }
//              
//  msgQueue->putMsg(data, 0, dataLen + sizeof(MsgHeaderType), true);   
//
    void putMsg(UINT8 *data, int startIndex, int validDataLen, bool endFlag);

    void putMsg(UINT8 *data[], int startIndex[], int validateDataLen[], int frameCount);
//
//  Once we call the message, we got the return value which type is UINT8*, and we must free the pointer by delete [];  or memory will leak.
//  
    UINT8 *getMsg(int &len);
    void clean();
#ifdef  MSGQUEUE_UNIT_TEST
    STATUS checkDemoData(UINT8 *msg, int frameCount, int frameLen);
    void putDemoData(int frameCount, int dataLen);
    STATUS test();
#endif

protected:
    /* All the protected function is not protected by mutex, so it can't be used as class interface. */
    void _putMsg(UINT8 *data, int startIndex, int validDataLen, bool endFlag);
    UINT8 *_getMsg(int &len);
    int _getMsgLen(int & frameCount);
    void _putDemoData(int frameCount, int frameLen);
    void _clean();
    STATUS _test();
    
protected:
    std::list<NodeType> m_msgList;      
    std::mutex m_mtx;           // mutex for critical section
};

#endif  // end of _MSG_QUEUE_H_
