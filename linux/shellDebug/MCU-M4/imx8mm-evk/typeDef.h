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

typedef unsigned long	SYSWORD;

typedef unsigned long	ULONG;
typedef long			LONG;

typedef  unsigned int   UINT;
typedef  int     		INT;

typedef unsigned int	UINT32;
typedef signed int		INT32;
	
typedef unsigned short 	UINT16;
typedef short			INT16;

typedef  unsigned char  UINT8;
typedef  char    INT8;


typedef	    int	STATUS;
#define 	STATUS_OK	0
#define 	STATUS_ERROR -1

#ifndef  FALSE
#define  FALSE false
#endif

#ifndef  TRUE
#define  TRUE true
#endif

#ifndef BOOL
#define BOOL bool
#endif

#define outputDebug		printf

// the following definition is for RpmsgHeaderType::m_sendModuleID and RpmsgHeaderType::m_recvModuleID
#define MODULE_ID_M4			0X20
#define MODULE_ID_RPMSG_DRIVER	0X21
#define MODULE_ID_TTY_DRIVER	0X22

// the following definition is for RpmsgHeaderType::m_msgID
#define MSG_ID_IR_RECEIVE		0x0
/*
	m_msgSubID = 0,  --  First frame of IR Receive msg
	m_msgSubID = 1,  --  2nd frame of IR Receive msg
	m_msgSubID = 2,  --  3rd frame of IR Receive msg
	...
	m_msgSubID = 0xFF,  --  last frame of IR Receive msg	
*/

/* The structure should be aligned with Byte. */
#pragma pack (1)
typedef struct 
{
	UINT8 m_sendModuleID;
	UINT8 m_recvModuleID;
	UINT8 m_msgID;			
	UINT8 m_msgSubID;			// Sub Msg ID
	UINT8 m_reserved1;			// Not used yet, reserved for future.
	UINT8 m_msgDataCheckSum;	// XOR all bytes in RpmsgType::m_data;
	UINT16 m_msgDataLength;		// m_msgLength is the size of RpmsgType::m_data in Bytes (1 Word = 4 Bytes).
} RpmsgHeaderType;

typedef struct
{
	RpmsgHeaderType m_header;
	UINT8 m_data[1];
} RpmsgType;
#pragma pack ()

#endif
