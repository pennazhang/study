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
/*-------------------------------------------------------------------------*/
#include <stdlib.h>
#include <string.h>
#include "utility.h"
#include "moduleThread.h"
#include "main.h"
#include "protocol.h"

using namespace std;

typedef void (ModuleThread::*DispatchFunc)(UINT8* pData, int dataLen);

std::map<int, DispatchFunc> s_moduleFuncMap =
{
    
    {   MSG_ID_TEST, &ModuleThread::onTest },
    {   MSG_ID_ON_CHANNEL_OPENED, &ModuleThread::onChannelOpened },
    {   MSG_ID_ON_CHANNEL_CLOSED, &ModuleThread::onChannelClosed },
    {   MSG_ID_ON_CHANNEL_MESSAGE, &ModuleThread::onChannelMessageReceived },
};

ModuleThread::ModuleThread(const char * threadName) : MsgThread(threadName)
{
}

void ModuleThread::onIdle()
{
}

void ModuleThread::onDispatchMessage(UINT8* pData, int dataLen)
{
    CommonMessage msg;
    msg.serializeFromBuffer(pData, dataLen);
    
    DispatchFunc pFunc;

    auto iter = s_moduleFuncMap.find(msg.m_msgHeader.m_msgID);
    if (iter == s_moduleFuncMap.end())
    {
        logFatal("ModuleThread recv unknown msg:  %d, len = %d", msg.m_msgHeader.m_msgID, dataLen);
        dumpHex(pData, dataLen, 1);
        return;
    }

    pFunc = iter->second;
    (this->*pFunc)(pData, dataLen);
}

void ModuleThread::onTest(UINT8* pData, int len)
{
    TestMessage testMessage;
    testMessage.serializeFromBuffer(pData, len);
    dumpHex(pData, len, 1);
    
    cout << "ModuleThread::testMessage: " << testMessage.m_testID << endl;
    testMessage.m_testID++;

    g_networkThread->sendMessage(&testMessage);
}

void ModuleThread::onChannelOpened(UINT8* pData, int len)
{
    AcceptSocketMessage msg;
    msg.serializeFromBuffer(pData, len);

    cout << "ModuleThread::onChannelOpened: " << msg.m_channelID << endl;  
}

void ModuleThread::onChannelClosed(UINT8* pData, int len)
{
    CloseSocketMessage msg;
    msg.serializeFromBuffer(pData, len);

    cout << "ModuleThread::onChannelClosed: " << msg.m_channelID << endl;  
}

void ModuleThread::onChannelMessageReceived(UINT8* pData, int len)
{
    ChannelMessage msg;
//    dumpHex(pData, len, 1);
    msg.serializeFromBuffer(pData, len);

    g_dcProtocol.dispatchChannelMessage(msg);
}

