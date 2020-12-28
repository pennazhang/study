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


#ifndef __IR_H__
#define __IR_H__

#include "FreeRTOS.h"

// We need to send the Received IR Signal to A53 every 200ms.
#define IR_RECV_BUFFER_DEBUG

/* Freescale includes. */

enum IRStatus
{
	IR_STATUS_BEGIN_ACTIVE,		// timer is not used.
	IR_STATUS_BEGIN_DEACTIVE,	// timer is the timestamp that the signal is been deactived.
	IR_STATUS_READY,			// timer is not used.
	IR_STATUS_RUNNING_ACTIVE,	// timer is the timestamp that the signal is been actived.
	IR_STATUS_RUNNING_DEACTIVE,	// timer is the timestamp that the signal is been deavtived.
} ;

typedef enum
{
	IR_EVENT_BEGIN,
	IR_EVENT_ENTER_ACTIVE,
	IR_EVENT_LEAVE_ACTIVE,
	IR_EVENT_END,
} IREventType;

typedef struct
{
	enum IRStatus 	status;
	UINT32			timer;
} IRStatusType;


#define FIRST_FRAME_INDEX		0
#define LAST_FRAME_INDEX		0xFF

// (496 - sizeof(RpmsgHeaderType)) / 4 = 122
#define MAX_RPMSG_BUFFER_SIZE 	122

#define BUFFER_RING_SIZE 	2000
typedef struct
{
#if defined IR_RECV_BUFFER_DEBUG	
	UINT32 m_IRRecvBuffer[BUFFER_RING_SIZE];
	UINT16 m_IRRecvBufferCount;				//  Total IR Signals in Words, <= 2000.
#endif	
	UINT32 m_sendTick;						// Last send tick in millisecond. Valid only when m_frameIndex != 0xFF
	UINT32 m_frameBuffer[MAX_RPMSG_BUFFER_SIZE];	// Used to send RPMSG. The length of RPMSG should less than or equal to 496 Bytes.
	UINT8 m_frameIndex;						// 0: Begin of Message; 0xFF : End of Message
	UINT8 m_frameBufferCount;						// effective FrameBuffer count in Words, <= 496.
} IRRecvBufferType;

void initIRMessage(IRRecvBufferType *pRingBuffer);
BOOL isMessageValid(IRRecvBufferType *pRingBuffer, int messageIndex);
int getRingBuffer(IRRecvBufferType *pRingBuffer, int nMaxLen, int messageIndex);

void checkIRMessage(IRRecvBufferType *pRingBuffer);
void beginIRMessage(IRRecvBufferType *pRingBuffer);
void endIRMessage(IRRecvBufferType *pRingBuffer);
void recordIRMessage(IRRecvBufferType *pRingBuffer, UINT32 usValue);
void IRProc(void);

extern IRRecvBufferType g_IRRecvBuffer;

#endif
