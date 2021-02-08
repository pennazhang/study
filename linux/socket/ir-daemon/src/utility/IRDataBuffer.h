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


#ifndef _IR_RECV_DATA_H_
#define _IR_RECV_DATA_H_
#include "typedef.h"
#include <list>
#include <mutex>          // std::mutex

//#define IR_RECV_UNIT_TEST

//
// IRDataBuffer is only to store the IR Data between IR Recveive Task and TCP Client Task.
// IR Receive task will receive the IR data frame from M4 through RPMSG (Normally a RPMSG length is less than 488 bytes), and send them to this buffer.
// TCP Client task will get the data from this buffer, and integrate the frames into a complete IR signal package, and send it to the network.
// So Mutex is used between the two tasks.
//
class IRDataBuffer
{
public:
    IRDataBuffer();
    ~IRDataBuffer();
    void operator()(void);


    /* This three functions can be used for multi-thread */
//
//  Function: MsgType * getIRData()
//  Return Value: the RPMSG get from the IRDataBuffer, which is created by new[] called in this function. So any code called this function must use delete to free the RPMSG.
//  Here is the example on how to use this function:
//  
//  MsgType *pMsg = pIRDataBuffer->getIRData();
//  if (pMsg != NULL)
//  {
//      sendMsgToServer(pMsg);
//      UINT8 *pData = (UINT8)pMsg;
//      delete []pData;
//  }
//
    MsgType * getIRData();


//
//  Function: MsgType * getIRData() --  Put the PRMSG into the buffer.
//  The parameter: pRPMSG won't be delete by this function. Here is the example on how to use this function:
//
//  UINT8 data[MAX_RPMSG_BUFFER_SIZE + sizeof(MsgHeaderType)];
//  MsgHeaderType* header = (MsgHeaderType*)data;
//  UINT8 *body = data + sizeof(MsgHeaderType);
//  FillMsgHeader(header);
//  FillMsgBode(body).
//  
//  pIRDataBuffer->putIRData((MsgType *)data);
//
    void putIRData(MsgType *pRPMSG);

//
//  Clean all the IR Data buffer in the buffer. 
//
    void clean();

#ifdef  IR_RECV_UNIT_TEST
    int getNodeCount() { return m_IRDataList.size(); }
    void putDemoData(int frameCount, int dataLen);
    STATUS test();  /* put some data to IRDataList, then get it back from IRDataList and compare the data. */  
#endif

protected:
//
// The following function is not protected by mutex, so it can't be used as external interface.
//
    int ir_getIRDataLen();
    void ir_revoverIRData();
    MsgType * ir_getIRData();
    void ir_putIRData(MsgType *pRPMSG);
    void ir_clean();

#ifdef  IR_RECV_UNIT_TEST
    /* Just for perssure test */
    STATUS ir_test();
    void ir_putDemoData(int frameCount, int dataLen);
#endif

protected:
    std::list<MsgType *> m_IRDataList;      
    std::mutex m_mtx;           // mutex for critical section
};

#endif  // end of _IR_RECV_DATA_H_
