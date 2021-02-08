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


#ifndef _CLIENT_SETTING_H_
#define _CLIENT_SETTING_H_

#include <thread>
#include "typedef.h"
#include "TCPClientThread.h"
#include "IRRecvThread.h"
#include "jsIrUtils.h"
#include "MsgQueue.h"

//
// The ClientSetting includes the thread of IR Rx thread and TCP client Thread.
// Normally the IR Rx thread received IR code from M4 through RPMSG, and put them into IRDataBuffer.
// Then the TCP Client Thread get IR data from IRDataBuffer and send it to server through network.
//
class ClientSetting
{
public:
    ClientSetting();
    ~ClientSetting();

    // To call this function, you need to ensure all thread is stopped already. 
    void start(BOOL passThroughFlag, UINT8 peerIP[4]);

    // To call this function, you need to ensure that start() function is called already.
    void reset(BOOL passThroughFlag, UINT8 peerIP[4]);

    void stop();
    MsgQueue * getMsgQueue() { return &m_msgQueue; }

    // get the Server IP Address for Pass Through function.
    UINT8 *getServerIP();

    CommunicationStatusType getConnectionStatus();
    
protected:
    void startTCPClientThread();
    void stopTCPClientThread();
    void startIRRecvThread();
    void stopIRRecvThread();

protected:
    TCPClientThread m_TCPClientThread;
    IRRecvThread    m_IRRecvThread;

    /* All the data that Received by IR will be stored in this buffer */
    MsgQueue    m_msgQueue; 

    /* used for PassThrough function */
    UINT8           m_serverIP[4];

    /* Enable / Disable the PassThrough function */
    BOOL            m_passThroughEnableFlag;
};

extern ClientSetting g_client;

#endif  // end of _CLIENT_SETTING_H_
