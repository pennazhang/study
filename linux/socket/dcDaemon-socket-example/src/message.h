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

#include "typedef.h"
#include "utility.h"
#include "serializer.h"
#include "byteArray.h"

struct MsgHeader
{
    MsgHeader(UINT32 msgID = 0, UINT32 bodyLen = 0) : m_msgID(msgID), m_msgBodyLen(bodyLen), m_senderID(0) { };
    virtual Serializer& operator<<(Serializer& out);
    virtual Serializer& operator>>(Serializer& in);
    UINT32 m_msgID;
    UINT32 m_msgBodyLen;    // = 0, Reserved for future.
    UINT32 m_senderID;    // Thread ID of sender, get by _threadID().
};

typedef enum
{
    MSG_ID_NULL = 0,
    MSG_ID_TEST,
    MSG_ID_ON_CHANNEL_OPENED,
    MSG_ID_ON_CHANNEL_CLOSED,
    MSG_ID_ON_CHANNEL_MESSAGE,
} MsgIDType;

class CommonMessage: public Serializable
{
public:
    CommonMessage(UINT32 msgID = MSG_ID_NULL) { m_msgHeader.m_msgID = msgID; };
    virtual Serializer& operator<<(Serializer& out);
    virtual Serializer& operator>>(Serializer& in);

public:
    MsgHeader m_msgHeader;
};


// m_msgID = MSG_ID_TEST
class TestMessage : public CommonMessage
{
public:
    TestMessage(int testID = 0) : CommonMessage(MSG_ID_TEST)  {  m_testID = testID; };
    virtual Serializer& operator<<(Serializer& out);
    virtual Serializer& operator>>(Serializer& in);

public: 
    UINT32 m_testID;
};

// m_msgID = MSG_ID_ON_CHANNEL_OPENED
class AcceptSocketMessage : public CommonMessage
{
public:
    AcceptSocketMessage(int socketID = 0) : CommonMessage(MSG_ID_ON_CHANNEL_OPENED)  {  m_channelID = socketID; };
    virtual Serializer& operator<<(Serializer& out);
    virtual Serializer& operator>>(Serializer& in);

public: 
    int m_channelID;
};

// m_msgID = MSG_ID_ON_CHANNEL_CLOSED
class CloseSocketMessage : public CommonMessage
{
public:
    CloseSocketMessage(int socketID = 0) : CommonMessage(MSG_ID_ON_CHANNEL_CLOSED)  {  m_channelID = socketID; };
    virtual Serializer& operator<<(Serializer& out);
    virtual Serializer& operator>>(Serializer& in);

public: 
    int m_channelID;
};

// m_msgID = MSG_ID_ON_CHANNEL_MESSAGE
class ChannelMessage : public CommonMessage
{
public:
    ChannelMessage() : CommonMessage(MSG_ID_ON_CHANNEL_MESSAGE), m_recvByteArray(0, NULL)  {  };
    virtual Serializer& operator<<(Serializer& out);
    virtual Serializer& operator>>(Serializer& in);
    void init(int socketID, ByteArray& receivedData)
    {
        m_channelID = socketID;
        m_recvByteArray = receivedData;
    };

public: 
    int m_channelID;
    ByteArray m_recvByteArray;
};


Serializer& operator<<(Serializer& out, MsgHeader& value);
Serializer& operator>>(Serializer& in, MsgHeader& value);
Serializer& operator<<(Serializer& out, CommonMessage& value);
Serializer& operator>>(Serializer& in, CommonMessage& value);
Serializer& operator<<(Serializer& out, TestMessage& value);
Serializer& operator>>(Serializer& in, TestMessage& value);


