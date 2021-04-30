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

#include <iostream>
#include <string.h>
#include <dbus-c++-1/dbus-c++/dbus.h>
#include <math.h>
#include "config.h"
#include "utility.h"


using namespace std;

TimePoint::TimePoint()
{
    m_timePoint = high_resolution_clock::now();
}

void TimePoint::getCurrentTime()
{
    m_timePoint = high_resolution_clock::now();
}

// Check for timer expiration with optional reload.
// \param timeSpanInMs expiration limit in millseconds.
// \param reload TRUE to reload timer on expiration, else FALSE.
// \return TRUE if timer expired, else FALSE.
//
bool TimePoint::isTimeExpired(UINT32 timeSpanInMs, bool reload)
{
    high_resolution_clock::time_point currentTime = high_resolution_clock::now();

    milliseconds timeInterval= std::chrono::duration_cast<milliseconds>(currentTime - m_timePoint);
    UINT32 timePassed = timeInterval.count();
    
    if (timePassed >= timeSpanInMs) 
    {
        if (reload) 
        {
            m_timePoint = currentTime;
        }
        return true;
    }
    else 
    {
        return false;
    }
}

int TimePoint::timeDiff(TimePoint *anotherPoint)
{
    milliseconds timeInterval = std::chrono::duration_cast<milliseconds>(this->m_timePoint - anotherPoint->m_timePoint);
    int ret = timeInterval.count();
    return (ret);
#if 0
    if (ret < 0)
    {
        return (-ret);
    }
    else
    {
        return (ret);
    }
#endif  
}

//
// To decide that the fd (or socket) is readable.
//
bool isFdReadable(int fd, int timeout_in_ms)
{   
    //we can use select to check the fd is readable.    
    int rc; 
    fd_set fds; 
    struct timeval tv = {0, timeout_in_ms * 1000};      
    FD_ZERO(&fds);  
    FD_SET(fd, &fds);   
    rc = select(fd + 1, &fds, NULL, NULL, &tv); 
    if (rc < 0) //error 
    {
        return false;   
    }
    return FD_ISSET(fd, &fds) ? true : false;
}

// strip the space from both head and tail.
// Warning: it will change the character of the parameter: s
char *strstrip(char *s)
{
    size_t size;
    char *end;

    size = strlen(s);

    if (!size)
        return s;

    end = s + size - 1;
    while (end >= s && isspace(*end))
        end--;
    *(end + 1) = '\0';

    while (*s && isspace(*s))
        s++;

    return s;
}

// Get the Process name according to the PID.
// just read the file: /proc/3302/status, the first line shoud be like this: "Name:   ir-daemon"
//
bool getNameByPid(pid_t pid, char task_name[128])
{
    char proc_pid_path[128];
    char buf[128];
    bool ret = false;

    sprintf(proc_pid_path, "/proc/%d/status", pid);
    FILE* fp = fopen(proc_pid_path, "r");
    if (NULL != fp)
    {
        if (fgets(buf, 127, fp) != NULL)
        {
            if (strtok(buf, ": ") != NULL)
            {
                char *name = strtok(NULL,"\r\n");
                name = strstrip(name);
                strcpy(task_name, name);
                ret = true;
            }
        }
        fclose(fp);
    }

    return ret;
}

const char SingletonFileName[] = "/tmp/ir-daemon-singleFileName"; 
SingletonProcess::SingletonProcess()
{
    char line[1024] = {0};
    FILE *fp  = fopen(SingletonFileName,"rt");
    int lastPid = 0;
    int currentPid = getpid();

    m_singletonFlag = true;

    if (fp != NULL)
    {
        if (fgets(line, 1023, fp) != NULL)
        {
            if (strlen(line) != 0)
            {
                lastPid = atoi(line);
            }
        }
        fclose(fp);
    }

    if ((lastPid != 0) && (lastPid != currentPid))
    {
        char lastPidName[128], currentPidName[128];
        if (getNameByPid(lastPid, lastPidName) == true)
        {
            getNameByPid(currentPid, currentPidName);
            logInfo("old [PID:%d]  = %s\t\tcurrent [PID:%d] = %s", lastPid, lastPidName, currentPid, currentPidName);
            if (strcmp(lastPidName, currentPidName) == 0)
            {
                m_singletonFlag = false;
            }
        }
    }

    if (m_singletonFlag == true)
    {
        fp = fopen(SingletonFileName, "wt");
        if (fp != NULL)
        {
            fprintf(fp, "%d", currentPid);
        }
        fclose(fp);
    }
}

SingletonProcess::~SingletonProcess()
{
    if (m_singletonFlag == true)
    {
        // We'd better to remove the pid file.
        // Even we have no chance to remove the file, it is still ok.
        remove(SingletonFileName);
    }
}

bool SingletonProcess::isSingleton()
{
    return (m_singletonFlag);
}

void dumpDecimal(UINT8 *data, int totalSizeInByte, int unitSizeInByte, int unitCountPerLine)
{
    char lineBuffer[4096] = {0};
    char strBuffer[20] = {0};
    int unitCount = totalSizeInByte / unitSizeInByte;
    int i, lineIndex = 0;
    
    if (unitCountPerLine >= 256)
    {
        unitCountPerLine = 256;
    }
    if (unitSizeInByte == 1)
    {
        /* UINT8 */ 
        UINT8 *pData = (UINT8 *)data;
        UINT8 unitData;
        lineIndex = 0;
        for (i = 0; i < unitCount; i++, lineIndex++)
        {
            if (lineIndex == unitCountPerLine)
            {
                logInfo("%s", lineBuffer);
                lineIndex = 0;
                lineBuffer[0] = 0;
            }

            unitData = pData[i];
            sprintf(strBuffer, "%d, ", unitData);
            strcat(lineBuffer, strBuffer);
        }
        logInfo("%s", lineBuffer);
    }

    if (unitSizeInByte == 2)
    {   
        /* UINT16 */
        UINT16 *pData = (UINT16 *)data;
        UINT16 unitData;
        lineIndex = 0;
        for (i = 0; i < unitCount; i++, lineIndex++)
        {
            if (lineIndex == unitCountPerLine)
            {
                logInfo("%s", lineBuffer);
                lineIndex = 0;
                lineBuffer[0] = 0;
            }

            unitData = pData[i];
            sprintf(strBuffer, "%d, ", unitData);
            strcat(lineBuffer, strBuffer);
        }
        logInfo("%s", lineBuffer);
    }

    if (unitSizeInByte == 4)
    {   
        /* UINT16 */
        UINT32 *pData = (UINT32 *)data;
        UINT32 unitData;
        lineIndex = 0;
        for (i = 0; i < unitCount; i++, lineIndex++)
        {
            if (lineIndex == unitCountPerLine)
            {
                logInfo("%s", lineBuffer);
                lineIndex = 0;
                lineBuffer[0] = 0;
            }

            unitData = pData[i];
            sprintf(strBuffer, "%d, ", unitData);
            strcat(lineBuffer, strBuffer);
        }
        logInfo("%s", lineBuffer);
    }
}

void dumpHex(UINT8 *data, int totalSizeInByte, int unitSizeInByte, int unitCountPerLine)
{
    char lineBuffer[4096] = {0};
    char strBuffer[20] = {0};
    int unitCount = totalSizeInByte / unitSizeInByte;
    int i, lineIndex = 0;
    
    if (unitCountPerLine >= 256)
    {
        unitCountPerLine = 256;
    }
    if (unitSizeInByte == 1)
    {
        /* UINT8 */ 
        UINT8 *pData = (UINT8 *)data;
        UINT8 unitData;
        lineIndex = 0;
        for (i = 0; i < unitCount; i++, lineIndex++)
        {
            if (lineIndex == unitCountPerLine)
            {
                logInfo("%s", lineBuffer);
                lineIndex = 0;
                lineBuffer[0] = 0;
            }

            unitData = pData[i];
            sprintf(strBuffer, "%02x, ", unitData);
            strcat(lineBuffer, strBuffer);
        }
        logInfo("%s", lineBuffer);
    }

    if (unitSizeInByte == 2)
    {   
        /* UINT16 */
        UINT16 *pData = (UINT16 *)data;
        UINT16 unitData;
        lineIndex = 0;
        for (i = 0; i < unitCount; i++, lineIndex++)
        {
            if (lineIndex == unitCountPerLine)
            {
                logInfo("%s", lineBuffer);
                lineIndex = 0;
                lineBuffer[0] = 0;
            }

            unitData = pData[i];
            sprintf(strBuffer, "%04x, ", unitData);
            strcat(lineBuffer, strBuffer);
        }
        logInfo("%s", lineBuffer);
    }

    if (unitSizeInByte == 4)
    {   
        /* UINT16 */
        UINT32 *pData = (UINT32 *)data;
        UINT32 unitData;
        lineIndex = 0;
        for (i = 0; i < unitCount; i++, lineIndex++)
        {
            if (lineIndex == unitCountPerLine)
            {
                logInfo("%s", lineBuffer);
                lineIndex = 0;
                lineBuffer[0] = 0;
            }

            unitData = pData[i];
            sprintf(strBuffer, "%08x, ", unitData);
            strcat(lineBuffer, strBuffer);
        }
        logInfo("%s", lineBuffer);
    }
}

std::string resultInfo(int errorCode, const char * info)
{
    Json::Value obj;
	obj["result"] = errorCode;
	if (info != NULL)
	{
		if (strlen(info) != 0)
		{
			obj["resultInfo"] = info;
		}
	}
	
	return (obj.toStyledString());
}

STATUS getIntFromJsonString(std::string jsonString, const char *key, int &value)
{
	Json::Value jsonValue, subObj;
	if (stringToJson(jsonString, jsonValue) != STATUS_OK)
	{
		return (STATUS_ERROR);
	}
	
	subObj = jsonValue.operator[](key);
	if (subObj == Json::nullValue)
	{
		return (STATUS_ERROR);
	}
	
	value = subObj.asInt();
	return (STATUS_OK);
}

STATUS getStringFromJsonString(std::string jsonString, const char *key, std::string &value)
{
	Json::Value jsonValue, subObj;
	if (stringToJson(jsonString, jsonValue) != STATUS_OK)
	{
		return (STATUS_ERROR);
	}

	subObj = jsonValue.operator[](key);
	if (subObj == Json::nullValue)
	{
		return (STATUS_ERROR);
	}
	
	value = jsonValue.operator[](key).toStyledString();
	return (STATUS_OK);
}

STATUS stringToJson(std::string jsonString, Json::Value &jsonValue)
{
	bool retCode;
    Json::CharReaderBuilder readerBuilder;
    std::unique_ptr<Json::CharReader> const reader(readerBuilder.newCharReader());
    if (jsonString.empty()) 
    {
        return (STATUS_ERROR);
    }

    JSONCPP_STRING jerrs;
    retCode = reader->parse(jsonString.c_str(), jsonString.c_str() + jsonString.size(), &jsonValue, &jerrs);
    if (!retCode) 
    {
        return (STATUS_ERROR);
    }

	return (STATUS_OK);
}

void dumpJsonValue(Json::Value & jsonValue)
{
	logInfo("jsonSize = %d", jsonValue.size());

	Json::Value::Members members = jsonValue.getMemberNames();
	for (auto key : members) 
    {
		logInfo("[%s] = %s", key.c_str(), jsonValue.operator[](key).toStyledString().c_str());
	} 
}
