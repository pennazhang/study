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

#ifndef _JS_M4_APP_H_
#define _JS_M4_APP_H_

#include "fsl_gpio.h"
#include "typeDef.h"

//#define RPMSG_DEBUG

typedef struct
{
	GPIO_Type *base;
	UINT32 pin;
	gpio_pin_config_t pinConfig;
} GPIOConfigType;

extern void setGPIO(int group, int pin, int value);

extern UINT32 getGPIO(int group, int pin);


/* 
	load the current time. 
	For this project, 1 timerTick = 1us. 
*/
void timerLoad(UINT32 *timerTicks);

/*!
 Check for timer expiration with optional reload.
 \param timerTicks pointer to timer.
 \param tickCount expiration limit.
 \param reload TRUE to reload timer on expiration, else FALSE.
 \return TRUE if timer expired, else FALSE.
 */
bool timerExpired(UINT32 *timerTicks, UINT32 tickCount, bool reload);

#if defined RPMSG_DEBUG
void rpmsg_sendTest(UINT8 frameIndex, int lengthInByte);
#endif
// Send Message through RPMSG. notice: sizeInByte must not greater than 488.
void rpmsg_sendReceivedIR(UINT8 frameIndex, UINT8 *receivedIRData, int sizeInByte);

/* Get the timer counter in MilliSecond */
UINT32 getTimerCounter(void);

#endif

