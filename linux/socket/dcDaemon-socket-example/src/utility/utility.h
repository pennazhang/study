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

using std::chrono::high_resolution_clock;
using std::chrono::milliseconds;

#define _threadID()   syscall(__NR_gettid) 


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
    SingletonProcess();
    ~SingletonProcess();
    bool isSingleton();
protected:
    bool m_singletonFlag;   
};


bool isFdReadable(int fd, int timeout_in_ms);

// strip the string from both header and tail. copied from Linux kernel.
char *strstrip(char *s);

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

