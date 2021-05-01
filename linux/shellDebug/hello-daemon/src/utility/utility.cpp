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
#include <algorithm>    // std::find_if
#include "unistd.h"     // usleep

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
char *strstrip(char *s, const char *blankCharacters)
{
    size_t size;
    char *end;

    size = strlen(s);

    if (!size)
        return s;

    end = s + size - 1;
    while (end >= s)
    {
        if ((blankCharacters != NULL) && (strchr(blankCharacters, *end)))
        {
            end--;
        }
        else 
        {
            break;
        }
    }
    *(end + 1) = '\0';

    while (*s)
    { 
        if ((blankCharacters != NULL) && (strchr(blankCharacters, *s)))
        {
            s++;
        }
        else
        {
            break;
        }
    }
    return s;
}

// trim from start (in place)
std::string ltrim(const std::string& s, const char* t)
{
    std::string newString = s;
    size_t index = newString.find_first_not_of(t);

    if (index != string::npos)
    {
        newString.erase(0, index);
    }
    return (newString);
}

// trim from end (in place)
std::string rtrim(const std::string& s, const char* t)
{
    std::string newString = s;
    size_t index = newString.find_last_not_of(t);
    if (index != string::npos)
    {
        newString.erase(index + 1);
    }
    return (newString);
}

// trim from both ends (in place)
std::string trim(const std::string& s, const char* t)
{
    std::string newString;

//    logInfo("trim before = [%s]", s.c_str());
    newString = ltrim(s, t);
//    logInfo("ltrim = [%s]", newString.c_str());
    newString = rtrim(newString, t);
//    logInfo("rtrim = [%s]", newString.c_str());

    return (newString);
}

template <typename Out>
void split(const std::string &s, char delim, Out result) {
    std::istringstream iss(s);
    std::string item;
    while (std::getline(iss, item, delim)) {
        *result++ = item;
    }
}

std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, std::back_inserter(elems));
    return elems;
}

// Get the Process name according to the PID.
// just read the file: /proc/3302/status, the first line shoud be like this: "Name:   xx-daemon"
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

SingletonProcess::SingletonProcess(const char * fileName) : m_singletonFileName(fileName)
{
    char line[1024] = {0};
    FILE *fp  = fopen(m_singletonFileName.c_str(),"rt");
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
        fp = fopen(m_singletonFileName.c_str(), "wt");
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
        remove(m_singletonFileName.c_str());
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

void mSleep(int timeInMS)
{
    usleep(timeInMS * 1000);
}

BOOL IsValidNumber(char pucData)
{
    if ((pucData >= '0') && (pucData <= '9'))
    {
        return (TRUE);
    }
    if ((pucData >= 'a') && (pucData <= 'f'))
    {
        return (TRUE);
    }
    if ((pucData >= 'A') && (pucData <= 'F'))
    {
        return (TRUE);
    }

    return (FALSE);
}

/***************************************************
Function: Read Hex array from string.
For Example:  readHexString ("23:12:9a:ba:08:00", aucMac, 6);
*****************************************************/
int readHexString(const char *lpszBuffer, UINT8 *pucData, int nLen, const char *space)
{
    int i;
    char cData[3];
    UINT ulData;

    cData[2] = 0;
    for (i = 0; i < nLen; i++)
    {
        cData[0] = lpszBuffer[0];
        cData[1] = lpszBuffer[1];

        if ((IsValidNumber(cData[0]) == TRUE) && (IsValidNumber(cData[1]) == TRUE))
        {
            sscanf(cData, "%x", &ulData);
            pucData[i] = (UINT8)ulData;
            
            if (strchr(space, lpszBuffer[2]) != NULL)
            {
                i++;
                break;
            }
            lpszBuffer += 3;
        }
        else
        {
            break;
        }
    }

    return (i);
}
