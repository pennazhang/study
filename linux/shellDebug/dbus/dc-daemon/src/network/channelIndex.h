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


#pragma once

//#error "This file is Not used, but it is reserved for future use maybe"

#include "typedef.h"
#include <list>
#include <mutex>          // std::mutex
#include <arpa/inet.h>  // inet_addr()

class TCPSocket;
class UDPSocket;

typedef enum
{
    CHANNEL_TCP = 0,
    CHANNEL_UDP,
    CHANNEL_SSL,
} ChannelType;

struct ChannelParamType
{
    ChannelType m_channelType;    
    union channelParam
    {
        struct
        {
            TCPSocket *m_objectPointer;
        } m_tcpParam;
        struct
        {
            UDPSocket *m_objectPointer;
            sockaddr_in m_sockAddr;
        } m_udpParam;
    }m_param;
    BOOL operator==(const ChannelParamType& channelParam);
    void operator=(const ChannelParamType &channelParam);
};

struct ChannelPairType
{
    int m_channelID;
    ChannelParamType m_channelParam;
    void operator=(const ChannelPairType &channelPair);
};

class ChannelIndex
{
public:
    ChannelIndex(int cache1Size = 5, int cache2Size = 30);
    ~ChannelIndex();

    int getChannelIndex(ChannelParamType& channelParam);
    BOOL getChannelParam(int channelIndex, ChannelParamType& param);
    void clean();

protected:
    std::list<ChannelPairType> m_channelList;
    std::mutex m_mtx;           // mutex for critical section
    int m_cache1Size, m_cache2Size;
    int m_newChannelID;
};

extern ChannelIndex g_channelIndex;

