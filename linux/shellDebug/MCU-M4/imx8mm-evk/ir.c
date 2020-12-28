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

#include "userCommand.h"
#include <math.h>
#include <stdarg.h>
#include <stdlib.h>
#include "fsl_str.h"
#include "typeDef.h"
#include "shellFrame.h"
#include "commonLib.h"
#include "fsl_debug_console.h"
#include "ir.h"
#include "evkApp.h"

// If the user long press the IR button, we should send the IR signal every 500ms.
#define IR_RPMSG_INTERVAL (500 * 1000)

/* Here 1 timerCounter = 1us */
const int 		IRActiveValue = 0;				/* Define IR Signal.  0:  Active,  1: Deactive */
const UINT32	maxCarrierPeriodCounter = 100;	/* All the carrier less than 100us should be consider as carrier. */
const UINT32 	endIRDeactiveCounter = 200000;		//  If the deactive time exceed endTime, then we can end the process. 
const UINT32 	EXTRA_CARRIER_PERIOD = 52 ;		

IRStatusType s_IRStatusType = { IR_STATUS_BEGIN_ACTIVE, 0 };


IRRecvBufferType g_IRRecvBuffer;

void initIRMessage(IRRecvBufferType *pIRRecvBuffer)
{
	pIRRecvBuffer->m_frameIndex = LAST_FRAME_INDEX;
	pIRRecvBuffer->m_frameBufferCount = 0;
	
#if defined IR_RECV_BUFFER_DEBUG	
	pIRRecvBuffer->m_IRRecvBufferCount = 0;
#endif
}

void beginIRMessage(IRRecvBufferType *pIRRecvBuffer)
{
#if defined IR_RECV_BUFFER_DEBUG	
	PRINTF("beginIRMessage called here...\r\n");
	pIRRecvBuffer->m_IRRecvBufferCount = 0;
#endif	
	pIRRecvBuffer->m_frameIndex = FIRST_FRAME_INDEX;
	pIRRecvBuffer->m_frameBufferCount = 0;
	pIRRecvBuffer->m_sendTick = getTimerCounter();
}

void endIRMessage(IRRecvBufferType *pIRRecvBuffer)
{	
#if defined IR_RECV_BUFFER_DEBUG	
	int dataLen = pIRRecvBuffer->m_IRRecvBufferCount;
	if (dataLen > BUFFER_RING_SIZE)
	{
		dataLen = BUFFER_RING_SIZE;
	}
	
	for (int i = 0; i < dataLen; i++)
	{
		if (pIRRecvBuffer->m_IRRecvBuffer[i] & 0x80000000) 
		{
			PRINTF("+%d - %08x\r\n", pIRRecvBuffer->m_IRRecvBuffer[i] & 0x7fffffff, pIRRecvBuffer->m_IRRecvBuffer[i] & 0x7fffffff);
		}
		else
		{
			PRINTF("-%d - %08x\r\n", pIRRecvBuffer->m_IRRecvBuffer[i], pIRRecvBuffer->m_IRRecvBuffer[i]);
		}
	}
	PRINTF("efficient length = %d\r\n", pIRRecvBuffer->m_IRRecvBufferCount);
	pIRRecvBuffer->m_IRRecvBufferCount = 0;
#endif	
	
	/* Send out the RPMSG */
	if ((pIRRecvBuffer->m_frameIndex == FIRST_FRAME_INDEX) && (pIRRecvBuffer->m_frameBufferCount < 16))
	{
		pIRRecvBuffer->m_frameIndex = LAST_FRAME_INDEX;
		/* This is just Noise that need to be filtered */
	}
	else
	{
		pIRRecvBuffer->m_frameIndex = LAST_FRAME_INDEX;
		rpmsg_sendReceivedIR(pIRRecvBuffer->m_frameIndex, (UINT8 *)pIRRecvBuffer->m_frameBuffer, pIRRecvBuffer->m_frameBufferCount * sizeof(UINT32));
	}
}

void checkIRMessage(IRRecvBufferType *pIRRecvBuffer)
{
	if (pIRRecvBuffer->m_frameIndex == LAST_FRAME_INDEX)
	{
		return;
	}
	
	if (timerExpired(&pIRRecvBuffer->m_sendTick, IR_RPMSG_INTERVAL, TRUE))
	{
		rpmsg_sendReceivedIR(pIRRecvBuffer->m_frameIndex, (UINT8 *)pIRRecvBuffer->m_frameBuffer, pIRRecvBuffer->m_frameBufferCount * sizeof(UINT32));
		pIRRecvBuffer->m_frameIndex++;
		pIRRecvBuffer->m_frameBufferCount = 0;		
	}
}

void recordIRMessage(IRRecvBufferType *pIRRecvBuffer, UINT32 value)
{
#if defined IR_RECV_BUFFER_DEBUG		
	if ((pIRRecvBuffer->m_IRRecvBufferCount + 1) < BUFFER_RING_SIZE)
	{
		pIRRecvBuffer->m_IRRecvBuffer[pIRRecvBuffer->m_IRRecvBufferCount] = value;
	}
	pIRRecvBuffer->m_IRRecvBufferCount++;	
#endif
	pIRRecvBuffer->m_frameBuffer[pIRRecvBuffer->m_frameBufferCount] = (value & 0x7fffffff);
	pIRRecvBuffer->m_frameBufferCount++;
	if (pIRRecvBuffer->m_frameBufferCount >= MAX_RPMSG_BUFFER_SIZE)
	{
		rpmsg_sendReceivedIR(pIRRecvBuffer->m_frameIndex, (UINT8 *)pIRRecvBuffer->m_frameBuffer, pIRRecvBuffer->m_frameBufferCount * sizeof(UINT32));
		pIRRecvBuffer->m_sendTick = getTimerCounter();
		pIRRecvBuffer->m_frameIndex++;
		pIRRecvBuffer->m_frameBufferCount = 0;
	}
}

UINT32 getLastingTime(BOOL activeFlag, UINT32 beginCounter, UINT32 endCounter)
{
	UINT32 lastingTime = endCounter - beginCounter;
//	PRINTF("%d - %d\r\n", activeFlag, lastingTime);
	if (lastingTime > endIRDeactiveCounter)
	{
		PRINTF("Invalid Signal Found!\r\n");
	}
	lastingTime &= 0x7fffffff;
	if (activeFlag == true)
	{
		lastingTime |= 0x80000000;
	}
	return (lastingTime);
}

/*
UINT16 getLastingTime(BOOL activeFlag, UINT32 s_lastCounter, UINT32 currentTimer)
{
	UINT32 lastingTime = currentTimer - s_lastCounter;
	PRINTF("%d - %d\r\n", activeFlag, lastingTime);
	if (lastingTime > endIRDeactiveCounter)
	{
		PRINTF("Invalid Signal Found!\r\n");
	}
	lastingTime &= 0x7fff;
	if (activeFlag == true)
	{
		lastingTime |= 0x8000;
	}
	return ((UINT16)lastingTime);
}
*/

void sendIREvent(IREventType eventType, UINT32 currentCounter)
{
	static UINT32 s_lastCounter = 0;
	static BOOL s_firstActiveFlag = TRUE;	
	
	UINT32 record;
	
	switch (eventType)
	{
	case IR_EVENT_BEGIN:
		beginIRMessage(&g_IRRecvBuffer);
//		addRingBuffer(&g_IRRecvBuffer, 0);
		s_firstActiveFlag = TRUE;
		break;
	case IR_EVENT_ENTER_ACTIVE:
		/* The first time that Active Signal is assert */
		if (s_firstActiveFlag == TRUE)
		{
//			PRINTF("First Active...\r\n");
			s_firstActiveFlag = FALSE;
		}
		else
		{
			record = getLastingTime(0, s_lastCounter, currentCounter);
			recordIRMessage(&g_IRRecvBuffer, record);
		}
		break;
	case IR_EVENT_LEAVE_ACTIVE:
		
		record = getLastingTime(1, s_lastCounter, currentCounter);
		recordIRMessage(&g_IRRecvBuffer, record);
		break;
	case IR_EVENT_END:
//		addRingBuffer(&g_IRRecvBuffer, 0xff);
		endIRMessage(&g_IRRecvBuffer);
		break;
	}
	s_lastCounter = currentCounter;
}

int getIRValue()
{
	int value = getGPIO(1, 13);
	return value;
}

bool isSignalActive(int signal)
{
	return (IRActiveValue == signal);
}

void changeStatus(enum IRStatus newStatus)
{
	s_IRStatusType.status = newStatus;
}

void IRProc(void)
{
	static BOOL s_isCarruerActive_atLastTime = TRUE;
	int IRValue = getIRValue();
	BOOL isIRActive = isSignalActive(IRValue);
	static int carrierCount = 0;
	
	checkIRMessage(&g_IRRecvBuffer);
	switch (s_IRStatusType.status)
	{
	case IR_STATUS_BEGIN_ACTIVE:
		/* The IR Status is not ready yet */
		if (isIRActive == FALSE)
		{
			changeStatus(IR_STATUS_BEGIN_DEACTIVE);
			timerLoad(&s_IRStatusType.timer);
		}
		break;
		
	case IR_STATUS_BEGIN_DEACTIVE:
		/* The IR Status is not ready yet */
		if (isIRActive == TRUE)
		{
			changeStatus(IR_STATUS_BEGIN_ACTIVE);
		} 
		else
		{
			if (timerExpired(&s_IRStatusType.timer, endIRDeactiveCounter, TRUE))
			{
//				PRINTF("Status Ready!\r\n");
				changeStatus(IR_STATUS_READY);
			}
		}
		break;
		
	case IR_STATUS_READY:
		/* IR is ready now, and a new receiver is not start yet */
		if (isIRActive == TRUE)
		{
			timerLoad(&s_IRStatusType.timer);
			sendIREvent(IR_EVENT_BEGIN, 0);
			sendIREvent(IR_EVENT_ENTER_ACTIVE, s_IRStatusType.timer);
			changeStatus(IR_STATUS_RUNNING_ACTIVE);
			s_isCarruerActive_atLastTime = TRUE;
			carrierCount = 0;
		}
		break;
		
	case IR_STATUS_RUNNING_ACTIVE:
		if (isIRActive == FALSE)
		{
			if (s_isCarruerActive_atLastTime == TRUE)
			{
				carrierCount++;	//carrier found.
				timerLoad(&s_IRStatusType.timer);
			}
			else
			{
				if (timerExpired(&s_IRStatusType.timer, maxCarrierPeriodCounter, FALSE))
				{
					/* If carrier found, we need to add some extra carrier period */
					if (carrierCount >= 3)
					{
						PUTCHAR('+');
						sendIREvent(IR_EVENT_LEAVE_ACTIVE, s_IRStatusType.timer + EXTRA_CARRIER_PERIOD);
					}
					else
					{
						sendIREvent(IR_EVENT_LEAVE_ACTIVE, s_IRStatusType.timer);
					}
					changeStatus(IR_STATUS_RUNNING_DEACTIVE);
				}
			}
		}
		s_isCarruerActive_atLastTime = isIRActive;
		break;
		
	case IR_STATUS_RUNNING_DEACTIVE:
		if (isIRActive == FALSE)
		{
			if (timerExpired(&s_IRStatusType.timer, endIRDeactiveCounter, FALSE))
			{
				sendIREvent(IR_EVENT_END, 0);			
				changeStatus(IR_STATUS_READY);
			}
		}
		else
		{
			timerLoad(&s_IRStatusType.timer);
			sendIREvent(IR_EVENT_ENTER_ACTIVE, s_IRStatusType.timer);
			changeStatus(IR_STATUS_RUNNING_ACTIVE);
			s_isCarruerActive_atLastTime = TRUE;
			carrierCount = 0;
		}
		break;
	}
}
