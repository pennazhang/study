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

#include "ServerSetting.h"
#include "config.h"
#include <string.h>

ServerSetting g_server;

ServerSetting::ServerSetting()
{
    m_passThroughEnableFlag = false;
    memset(m_clientIP, 0, 4);
}

ServerSetting::~ServerSetting()
{
    stopTCPServerThread();
}


void ServerSetting::start(BOOL passThroughFlag, UINT8 peerIP[4])
{
    UINT32 *ipAddr = (UINT32 *)peerIP;
    if ((*ipAddr != 0) && (passThroughFlag == TRUE))
    {
        m_passThroughEnableFlag = true;
        memcpy(m_clientIP, peerIP, 4);
    }
    else
    {
        m_passThroughEnableFlag = false;
        memset(m_clientIP, 0, 4);
    }

    if (m_passThroughEnableFlag == true)
    {
        startTCPServerThread();
    }
    startIRSendThread();
}

void ServerSetting::stop()
{
    stopTCPServerThread();
    stopIRSendThread();
}

UINT8 * ServerSetting::getClientIP()
{
    return (m_clientIP);
}


void ServerSetting::reset(BOOL passThroughFlag, UINT8 peerIP[4])
{
    BOOL flag = false;
    UINT32 *ipAddr = (UINT32 *)peerIP;

    if ((*ipAddr != 0) && (passThroughFlag == TRUE))
    {
        flag = true;
    }

    if ((flag == true) && (m_passThroughEnableFlag == true))
    {
        if (memcmp(peerIP, m_clientIP, 4) == 0)
        {
            return;
        }
    }

    stopTCPServerThread();
    if (flag == true)
    {
        m_passThroughEnableFlag = true;
        memcpy(m_clientIP, peerIP, 4);
        startTCPServerThread();
    }
    else
    {
        m_passThroughEnableFlag = false;
        memset(m_clientIP, 0, 4);
    }
}

void ServerSetting::startTCPServerThread()
{
    m_TCPServerThread.start();
}

void ServerSetting::stopTCPServerThread()
{
    m_TCPServerThread.stop();
}

void ServerSetting::startIRSendThread()
{
    m_IRSendThread.start();
}

void ServerSetting::stopIRSendThread()
{
    m_IRSendThread.stop();
}


MsgQueue * ServerSetting::getMsgQueue()
{
    return (&m_msgQueue);
}

