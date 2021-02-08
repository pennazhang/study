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


#ifndef _SERVER_SETTING_H_
#define _SERVER_SETTING_H_

#include <thread>
#include "typedef.h"
#include "TCPServerThread.h"
#include "IRSendThread.h"
#include "MsgQueue.h"

//
// The ServerSetting include the thread of IR Tx thread and TCP Server Thread.
// Normally the TCP Server Thread received IR code from client and put it into MsgQueue.
// Then the IR Tx Thread get IR data from the MsgQueue and send it out through IR Tx driver.
//
class ServerSetting
{
public:
    ServerSetting();
    ~ServerSetting();

    // To call this function, you need to ensure all thread is stopped already.
    void start(BOOL passThroughFlag, UINT8 peerIP[4]);

    void stop();

    // This function take effect only for TCP Server Thread.
    void reset(BOOL passThroughFlag, UINT8 peerIP[4]);

    // we will reject all the connection except for the specified client IP address.
    UINT8 *getClientIP();

    MsgQueue * getMsgQueue();

protected:
    void startTCPServerThread();
    void stopTCPServerThread();
    void startIRSendThread();
    void stopIRSendThread();

protected:
    TCPServerThread m_TCPServerThread;
    IRSendThread    m_IRSendThread;
    MsgQueue    m_msgQueue; 
    BOOL    m_passThroughEnableFlag;
    UINT8   m_clientIP[4];
};

extern ServerSetting g_server;

#endif  // end of _SERVER_SETTING_H_
