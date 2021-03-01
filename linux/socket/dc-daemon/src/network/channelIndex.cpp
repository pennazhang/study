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


#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include "channelIndex.h"

using namespace std;

ChannelIndex g_channelIndex;

BOOL ChannelParamType::operator==(const ChannelParamType& channelParam)
{
    if (m_channelType == channelParam.m_channelType)
    {
        if (m_channelType == CHANNEL_TCP)
        {
            if (m_param.m_tcpParam.m_objectPointer == channelParam.m_param.m_tcpParam.m_objectPointer)
            {
                return (true);
            }
        }
        else if (m_channelType == CHANNEL_UDP)
        {
            if (memcmp((void *)&m_param.m_udpParam, (void *)&channelParam.m_param.m_udpParam, sizeof(m_param.m_udpParam)) == 0)
            {
                return (true);
            }
        }
    }
    return (false);
}

void ChannelParamType::operator=(const ChannelParamType &channelParam)
{
    this->m_channelType = channelParam.m_channelType;
    memcpy((void *)&m_param, (void *)&channelParam.m_param, sizeof(m_param));
}

ChannelIndex::ChannelIndex(int cache1Size, int cache2Size) 
    : m_cache1Size(cache1Size), m_cache2Size(cache2Size), m_newChannelID(1)
{

}

ChannelIndex::~ChannelIndex()
{
    clean();
}

void ChannelIndex::clean()
{
    m_mtx.lock();
    m_channelList.clear();
    m_mtx.unlock();
}

void ChannelPairType::operator=(const ChannelPairType &channelPair)
{
    m_channelID = channelPair.m_channelID;
    m_channelParam = channelPair.m_channelParam;
}


int ChannelIndex::getChannelIndex(ChannelParamType& channelParam)
{
    m_mtx.lock();

    int retChannelID = -1;
    int index = 0;
    ChannelPairType pair;
    std::list<ChannelPairType>::iterator iter = m_channelList.begin();
    for( ; iter != m_channelList.end(); index++, iter++)
	{
        if (iter->m_channelParam == channelParam)
        {
            break;
        }
	}
    if (iter == m_channelList.end())
    {
        // Not found in the list, then we need to insert a new channel.
        pair.m_channelID = m_newChannelID;
        m_newChannelID++;
        if (m_newChannelID < 1)
        {
            m_newChannelID = 1;
        }
        pair.m_channelParam = channelParam;
        m_channelList.push_front(pair);
        retChannelID = pair.m_channelID;

        if (index >= m_cache2Size)
        {
            cout << "delete tail" << endl;
            m_channelList.pop_back();
        }
    }
    else
    {
        //We found the Channel in the list.
        retChannelID = iter->m_channelID;

        /* Check if we need to adjust the positoin */
        if (index >= m_cache1Size)
        {
            pair = *iter;
            cout << "cache: " << index << endl;
            m_channelList.erase(iter);
            m_channelList.push_front(pair);
        }
    }

    m_mtx.unlock();
    return (retChannelID);
}
    
BOOL ChannelIndex::getChannelParam(int channelIndex, ChannelParamType& channelParam)
{
    m_mtx.lock();

    BOOL ret = false;
    int index = 0;
    std::list<ChannelPairType>::iterator iter = m_channelList.begin();
    for( ; iter != m_channelList.end(); index++, iter++)
	{
        if (iter->m_channelID == channelIndex)
        {
            break;
        }
	}
    if (iter != m_channelList.end())
    {
        //We found the Channel in the list.
        ret = true;
        channelParam = iter->m_channelParam;
    }

    m_mtx.unlock();

    return (ret);
}

