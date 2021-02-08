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


#ifndef TYPEDEF_H
#define TYPEDEF_H

#include <stdbool.h>
#include <stdio.h>
#include "config.h"

//
// Re-define the Basic type of variable.
//
typedef unsigned long   SYSWORD;

typedef unsigned long   ULONG;
typedef long            LONG;

typedef  unsigned int   UINT;
typedef  int            INT;

typedef unsigned int    UINT32;
typedef signed int      INT32;
    
typedef unsigned short  UINT16;
typedef short           INT16;

typedef  unsigned char  UINT8;
typedef  char    INT8;


typedef     int STATUS;
#define     STATUS_OK   0
#define     STATUS_ERROR -1

#ifndef  FALSE
#define  FALSE false
#endif

#ifndef  TRUE
#define  TRUE true
#endif

#ifndef BOOL
#define BOOL bool
#endif

//
// Define the socket connection status.
//
typedef enum
{
    STATUS_DISCONNECTED = 1,
    STATUS_CONNECTED,
} CommunicationStatusType;

//
// the following definition is for MsgHeaderType::m_sendModuleID and MsgHeaderType::m_recvModuleID
//
#define MODULE_ID_M4            0X20
#define MODULE_ID_RPMSG_DRIVER  0X21
#define MODULE_ID_TTY_DRIVER    0X22
#define MODULE_ID_WEB_SERVER    0X23
#define MODULE_ID_IR_SEND_DRIVER    0X24
#define MODULE_ID_NOT_CARE  0XFF

//
// the following definition is for MsgHeaderType::m_msgID
//
#define MSG_ID_RPMSG_RECVING_IR     0x0
#define MSG_ID_IR_RAW_ONCE          0X4
#define MSG_ID_IR_RAW_REPEAT        0X8
#define MSG_ID_IR_RAW_REPEAT_COUNT  0X9
#define MSG_ID_IR_RAW_REPEAT_TIME   0XA
#define MSG_ID_IR_RAW_TEST          0xB
#define MSG_SOCKET_SHAKE_HAND       0x10

// m_sequenceID = 0,  --  First frame of IR Receive msg
// m_sequenceID = 1,  --  2nd frame of IR Receive msg
// m_sequenceID = 2,  --  3rd frame of IR Receive msg
//  ...
// m_sequenceID = 0xFF,  --  last frame of IR Receive msg   
//
#define SEQUENCE_END    0xff

//
// The structure should be aligned with Byte. 
//
#pragma pack (1)

typedef struct 
{
    UINT8 m_sendModuleID;
    UINT8 m_recvModuleID;
    UINT8 m_msgID;  
    UINT8 m_reserved_4;
    UINT8 m_reserved_5;         // Not used yet, reserved for future.
    UINT8 m_msgDataCheckSum;    // XOR all bytes in MsgType::m_data;
    UINT16 m_msgDataLength;     // m_msgLength is the size of MsgType::m_data in Bytes (1 Word = 4 Bytes).  
} MsgHeaderType;

// m_msgID = MSG_ID_RPMSG_RECEVING_IR
typedef struct 
{
    UINT8 m_sendModuleID;
    UINT8 m_recvModuleID;
    UINT8 m_msgID;          
    UINT8 m_sequenceID;         // sequence number.
    UINT8 m_reserved_5;         // Not used yet, reserved for future.
    UINT8 m_msgDataCheckSum;    // XOR all bytes in MsgType::m_data;
    UINT16 m_msgDataLength;     // m_msgLength is the size of MsgType::m_data in Bytes (1 Word = 4 Bytes).
} RpmsgHeaderType;

// m_msgID = MSG_ID_IR_RAW_ONCE
// m_msgID = MSG_ID_IR_RAW_REPEAT
// m_msgID = MSG_ID_IR_RAW_TEST
typedef struct 
{
    UINT8 m_sendModuleID;
    UINT8 m_recvModuleID;
    UINT8 m_msgID;          
    UINT8 m_freq;               // Frequency of Pronto Code.
    UINT8 m_reserved_5;         // Not used yet, reserved for future.
    UINT8 m_msgDataCheckSum;    // XOR all bytes in MsgType::m_data;
    UINT16 m_msgDataLength;     // m_msgLength is the size of MsgType::m_data in Bytes (1 Word = 4 Bytes).
} IRRawHeaderType;

// m_msgID = MSG_ID_IR_RAW_REPEAT_COUNT
typedef struct 
{
    UINT8 m_sendModuleID;
    UINT8 m_recvModuleID;
    UINT8 m_msgID;          
    UINT8 m_freq;               // Frequency of Pronto Code.
    UINT8 m_repeatCount;        // Repeat Count. 1 = send only once.
    UINT8 m_msgDataCheckSum;    // XOR all bytes in MsgType::m_data;
    UINT16 m_msgDataLength;     // m_msgLength is the size of MsgType::m_data in Bytes (1 Word = 4 Bytes).
} IRRepeatCountHeaderType;

// m_msgID = MSG_ID_IR_RAW_REPEAT_TIME
typedef struct 
{
    UINT8 m_sendModuleID;
    UINT8 m_recvModuleID;
    UINT8 m_msgID;          
    UINT8 m_freq;               // Frequency of Pronto Code.
    UINT8 m_repeatTime;         // Repeat Time in 100ms. 1 = 100ms, 2 = 200ms.
    UINT8 m_msgDataCheckSum;    // XOR all bytes in MsgType::m_data;
    UINT16 m_msgDataLength;     // m_msgLength is the size of MsgType::m_data in Bytes (1 Word = 4 Bytes).
} IRRepeatTimeHeaderType;

typedef struct
{
    MsgHeaderType m_header;
    UINT8 m_data[1];
} MsgType;
#pragma pack ()

#endif
