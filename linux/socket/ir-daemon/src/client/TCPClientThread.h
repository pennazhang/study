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


#ifndef _TCP_CLIENT_THREAD_H_
#define _TCP_CLIENT_THREAD_H_

#include "typedef.h"
#include "IRDataBuffer.h"
#include <thread>
#include "MsgQueue.h"


//
// Once the IR Recv Thread put the received IR signal into the IRDataBuffer, then the TCP Client will
// get the IR data from the IRDataBuffer, and send it to TCP Server through network.
//
class TCPClientThread
{
public:
    TCPClientThread();
    ~TCPClientThread();
    
    void start();
    void stop();
    bool getStopFlag();
    MsgQueue* getMsgQueue();
    void setConnectionStatus(CommunicationStatusType status);
    CommunicationStatusType getConnectionStatus();
    
protected:
    std::thread     *m_pThread;

    // For the thread to quit gracefully. 
    // The thread should quit when this flag is set to TRUE.
    bool m_stopFlag;    

    // Connected or disconnected
    CommunicationStatusType     m_communicationStatus;  
};

#endif  // end of _TCP_CLIENT_THREAD_H_
