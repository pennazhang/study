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

#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <stdint.h>
#include <time.h>
#include "hxLog.h"

//#define _DEBUG_

// Max RPMSG frame size.
#define MAX_RPMSG_BUFFER_SIZE 488

// TCP Server for IR Passthrough function
#define IR_PASSTHROUGH_PORT           50020

// only valid for msgID = MSG_ID_IR_RAW_REPEAT
// The total repeat time is decided by irStop, but the time repeating a IR signal should not longer than MAX_IR_REPEAT_TIME_IN_MILLSECONDS 
//
#define MAX_IR_REPEAT_TIME_IN_MILLSECONDS   5000

// only valid for msgID = MSG_ID_IR_RAW_REPEAT_COUNT
//
#define DEFAULT_IR_REPEAT_COUNT 2

// Only valid when MsgID = MSG_ID_IR_RAW_REPEAT_TIME
// 1 = 100 ms; 5 = 500ms.
// Total repeat time should just greater than DEFAULT_IR_REPEAT_TIME * 100 ms.
#define DEFAULT_IR_REPEAT_TIME  5

// Used for processSettings where the command is came from Direct Control.
// According to N2400, MAX_COMMAND_LENGTH is defined as 2048 
#define MAX_COMMAND_LENGTH      2048

//
// Enable this feature will enable the sockets to send SHAKE_HAND message periodically.
// Also the sockets will check the received message periodically. If no message is received, it will disconnect the TCP connection automatically.
//
//#define COMMUNICATOIN_SHAKE_HAND

//
// Send SHAKE_HAND message periodically in ms */
//
#define SHAKE_HAND_INTERVAL         1000

//
// Check SHAKE_HAND message periodically in ms 
//
#define SHAKE_HAND_CHECK_INTERVAL   5000

/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/
extern HxLogger* g_logger;
extern const char *DAEMON_NAME;
extern clock_t g_StartClock;

#define SVSI_SYSLOG(format, ...) { \
    uint32_t clock_elapse = ((float)(clock() - g_StartClock + 1) / 1000); \
    hxDebug(g_logger, "[%s]:[time elapse: %ds]:%s " format, DAEMON_NAME, clock_elapse, __PRETTY_FUNCTION__, ## __VA_ARGS__); \
}

#define logFatal(format, ...) { \
    uint32_t clock_elapse = ((float)(clock() - g_StartClock + 1) / 1000); \
    hxFatal(g_logger, "[%s]:[time elapse: %ds]:%s [Fatal Error] " format, DAEMON_NAME, clock_elapse, __PRETTY_FUNCTION__, ## __VA_ARGS__); \
}

#define logError(format, ...) { \
    uint32_t clock_elapse = ((float)(clock() - g_StartClock + 1) / 1000); \
    hxError(g_logger, "[%s]:[time elapse: %ds]:%s [Error] " format, DAEMON_NAME, clock_elapse, __PRETTY_FUNCTION__, ## __VA_ARGS__); \
}

#define logWarning(format, ...) { \
    uint32_t clock_elapse = ((float)(clock() - g_StartClock + 1) / 1000); \
    hxWarn(g_logger, "[%s]:[time elapse: %ds]:%s [Warning] " format, DAEMON_NAME, clock_elapse, __PRETTY_FUNCTION__, ## __VA_ARGS__); \
}

#define logInfo(format, ...) { \
    hxInfo(g_logger, "[%s] [Info] " format, DAEMON_NAME, ## __VA_ARGS__); \
}

#ifdef _DEBUG_
#define logDebug(format, ...) { \
    hxDebug(g_logger, format, ## __VA_ARGS__); \
}
#else
#define logDebug(format, ...)  ((void)0)
#endif  

#ifdef _DEBUG
    #define ASSERT(f) \
        do \
        { \
            if (!(f)) \
            logFatal("Fatal Error at %s:  Line: %d\n", __FILE__, __LINE__); \
        } while (0)
#else   // _DEBUG
    #define ASSERT(f)          ((void)0)
#endif // !_DEBUG


#endif /* _CONFIG_H_ */
