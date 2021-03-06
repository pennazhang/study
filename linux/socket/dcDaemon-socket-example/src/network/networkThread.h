//####
//# Legal Notice :
//#    Copyright, Harman International, 2020
//#    Private, proprietary information, the sole property of Harman.  The
//#    contents, ideas, and concepts expressed herein are not to be disclosed
//#    except within the confines of a confidential relationship and only
//#    then on a need to know basis.
//#
//#    Any entity in possession of this Harman Software shall not, and shall not
//#    permit any other person to, disclose, display, loan, publish, transfer
//#    (whether by sale, assignment, exchange, gift, operation of law or
//#    otherwise), license, sublicense, copy, or otherwise disseminate this
//#    Harman Software.
//#
//#    This Harman Software is owned by Harman and is protected by United States
//#    copyright laws, patent laws, international treaty provisions, and/or
//#    state of Texas trade secret laws.
//#
//#    Portions of this Harman Software may, from time to time, include
//#    pre-release code and such code may not be at the level of performance,
//#    compatibility and functionality of the final code. The pre-release code
//#    may not operate correctly and may be substantially modified prior to
//#    final release or certain features may not be generally released. Harman
//#    is not obligated to make or support any pre-release code. All pre-release
//#    code is provided "as is" with no warranties.
//#
//#    This Harman Software is provided with restricted rights. Use,duplication,
//#    or disclosure by the Government is subject to restrictions as set forth
//#    in subparagraph (1)(ii) of The Rights in Technical Data and Computer
//#    Software clause at DFARS 252.227-7013 or subparagraphs (1) and (2) of
//#    the Commercial Computer Software Restricted Rights at 48 CFR 52.227-19,
//#    as applicable.
//####
#pragma once

#include "msgThread.h"
#include "tcpServer.h"
#include "udpSocket.h"
#include <vector>   // std::vector
#include "secureTCPServer.h"

class NetworkThread : public MsgThread
{
public:
    NetworkThread(const char * threadName = NULL);
    virtual ~NetworkThread();

    virtual void onDispatchMessage(UINT8*pData, int len);
    virtual void onIdle();

    /* This method can be called from any Thread  */
    void cleanAllMessage();
    void closeAllSocket();

    /* Dispatch Message from ModuleThread */
    void onTest(UINT8* pData, int len);
    void onSendChannelMessage(UINT8* pData, int len);

    virtual STATUS onStart();
    virtual void onQuit();

protected:
    void sendChannelMessage(int channelID, ByteArray &byteArray);

protected:
    SecureTCPServer m_sslServer;
    TCPServer m_tcpServer; 
    UDPSocket m_udpSocket;
    std::vector<TCPSocket *> m_tcpSocketVector;
};
