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
#include "protocol.h"
#include "main.h"

Protocol g_dcProtocol("Direct Control Protocol");

using namespace std;

Channel::Channel(int channelID) : m_channelID(channelID)
{
    m_commandRunningFlag = false;
}

void Channel::appendCommand(std::string &command)
{
    m_commandList.push_back(command);
}

std::string Channel::executeNextCommand()
{
    if (m_commandList.size() == 0)
    {
        m_commandRunningFlag = false;
        return (std::string(""));
    }
    else
    {
        m_commandRunningFlag = true;
        m_commandStartPoint.getCurrentTime();
        std::string command = m_commandList.front();
        m_commandList.pop_front();
        return (command);
    }
}

Protocol::Protocol(std::string protocolName) :  m_protocolName(protocolName)
{

}

Protocol::~Protocol() 
{

}

void Protocol::dispatchChannelMessage(ChannelMessage& msg)
{
    std::string command((char *)msg.m_recvByteArray.data());
    cout << "ModuleThread::onChannelMessageReceived " << msg.m_channelID <<  ": " << command << endl;

    command += ", ret = STATUS_OK\r\n";
    msg.m_recvByteArray.resize(command.size());
    memcpy(msg.m_recvByteArray.data(), command.c_str(), command.size());
    g_networkThread->sendMessage(&msg);    
}
