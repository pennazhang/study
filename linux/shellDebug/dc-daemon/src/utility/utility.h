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
#include <iostream>
#include <chrono>
#include <unistd.h>
#include <json/json.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <stdarg.h>
#include <mutex>          // std::mutex
#include <cassert>

using std::chrono::high_resolution_clock;
using std::chrono::milliseconds;

#define _threadID()   syscall(__NR_gettid) 

/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/
extern clock_t g_StartClock;


/*---------------------------------------------------------------------------*
 * Constants:
 *---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*
 * Prototypes:
 *---------------------------------------------------------------------------*/
class TimePoint
{
public:
    TimePoint();
    void getCurrentTime();
    
    // Check for timer expiration with optional reload.
    // \param timeSpanInMs expiration limit in millseconds.
    // \param reload TRUE to reload timer on expiration, else FALSE.
    // \return TRUE if timer expired, else FALSE.
    //
    BOOL isTimeExpired(UINT32 timeSpanInMs, bool reload);

    // Get the time span between two TimePoint in ms. 
    // Here is an example of timeDiff:
    //  TimePoint beginTime;
    //  std::this_thread::sleep_for(std::chrono::milliseconds(1000));       
    //  TimePoint endTime;
    //
    //  beginTime.timeDiff(&endTime) = 1000
    //  endTime.timeDiff(&beginTime) = -1000;
    int timeDiff(TimePoint *anotherPoint);

protected:
    high_resolution_clock::time_point m_timePoint;
};

/* This class can ensure that there is only one instance of the application in the Linux system */
class SingletonProcess
{
public:
    SingletonProcess(const char *singletonFileName);
    ~SingletonProcess();
    bool isSingleton();
protected:
    bool m_singletonFlag;
    std::string m_singletonFileName;
};


bool isFdReadable(int fd, int timeout_in_ms);

// strip the string from both header and tail.  It will change the content of s.
char *strstrip(char *s, const char* blank = " \t\n\r\f\v");


// The following characters will be trimed:
// space (0x20, ' ')
// form feed (0x0c, '\f')
// line feed (0x0a, '\n')
// carriage return (0x0d, '\r')
// horizontal tab (0x09, '\t')
// vertical tab (0x0b, '\v')

// trim from beginning (in place)
std::string ltrim(const std::string& s, const char* blank = " \t\n\r\f\v");

// trim from end (in place)
std::string rtrim(const std::string& s, const char* blank = " \t\n\r\f\v");

// trim from both ends (in place)
std::string trim(const std::string& s, const char* blank = " \t\n\r\f\v");

bool getNameByPid(pid_t pid, char task_name[128]);

void dumpDecimal(UINT8 *data, int totalSizeInByte, int unitSizeInByte, int unitCountPerLine = 100);

void dumpHex(UINT8 *data, int totalSizeInByte, int unitSizeInByte, int unitCountPerLine = 100);

// ************************************************************************************
//                   Here is the JSON Test file:
//*************************************************************************************
//
//	string resultString = resultInfo(1002, NULL);
//
//	int result = 5;
//	status = getIntFromJsonString(resultString, "result", result);
//	if (status == STATUS_OK)
//	{
//		logInfo("result = %d", result);
//	}
//	
//	std::string info;
//	status = getStringFromJsonString(resultString, "resultInfo", info);
//	if (status == STATUS_OK)
//	{
//		logInfo("info = %s", info.c_str());
//	}
std::string resultInfo(int errorCode, const char * info);

STATUS stringToJson(std::string jsonString, Json::Value &obj);

STATUS getIntFromJsonString(std::string jsonString, const char *key, int &value);

STATUS getStringFromJsonString(std::string jsonString, const char *key, std::string &value);

void dumpJsonValue(Json::Value & jsonValue);

void mSleep(int timeInMS);

/***************************************************
Function: Read Hex array from string.
For Example:  readHexString ("23:12:9a:ba:08:00", aucMac, 6);
*****************************************************/
int readHexString(const char *lpszBuffer, UINT8 *pucData, int nLen, const char *space = " ,:-");


#define SVSI_SYSLOG(format, ...) { \
    uint32_t clock_elapse = ((float)(clock() - g_StartClock + 1) / 1000); \
    printf("[%s]:[time elapse: %ds]:%s " format "\n", DAEMON_NAME, clock_elapse, __PRETTY_FUNCTION__, ## __VA_ARGS__); \
}

#define logFatal(format, ...) { \
    uint32_t clock_elapse = ((float)(clock() - g_StartClock + 1) / 1000); \
    printf("[%s]:[time elapse: %ds]:%s [Fatal Error] " format "\n", DAEMON_NAME, clock_elapse, __PRETTY_FUNCTION__, ## __VA_ARGS__); \
}

#define logError(format, ...) { \
    uint32_t clock_elapse = ((float)(clock() - g_StartClock + 1) / 1000); \
    printf("[%s]:[time elapse: %ds]:%s [Error] " format "\n", DAEMON_NAME, clock_elapse, __PRETTY_FUNCTION__, ## __VA_ARGS__); \
}

#define logWarning(format, ...) { \
    uint32_t clock_elapse = ((float)(clock() - g_StartClock + 1) / 1000); \
    printf("[%s]:[time elapse: %ds]:%s [Warning] " format "\n", DAEMON_NAME, clock_elapse, __PRETTY_FUNCTION__, ## __VA_ARGS__); \
}

#define logInfo(format, ...) { \
    uint32_t clock_elapse = ((float)(clock() - g_StartClock + 1) / 1000); \
    printf("[%s]:[time elapse: %ds]:%s [Info] " format "\n", DAEMON_NAME, clock_elapse, __PRETTY_FUNCTION__, ## __VA_ARGS__); \
}

#define logDebug(format, ...) { \
    uint32_t clock_elapse = ((float)(clock() - g_StartClock + 1) / 1000); \
    printf("[%s]:[time elapse: %ds]:%s [Debug] " format "\n", DAEMON_NAME, clock_elapse, __PRETTY_FUNCTION__, ## __VA_ARGS__); \
}

#define ASSERT assert
