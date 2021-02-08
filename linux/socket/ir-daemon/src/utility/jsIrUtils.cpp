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
#include <string>
#include <sysexits.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <dbus-c++-1/dbus-c++/dbus.h>
#include <math.h>
#include "config.h"
#include "jsIrUtils.h"
#include "CmdHandlerInterface.h"
#include "CmsfsIrInterface.h"
#include "ConfigManagerAPI.h"
#include "NetworkCfg.h"
#include "ClientSetting.h"
#include "ServerSetting.h"
#include <fcntl.h>

#define PRONTO_SPLIT_TOKEN              " ,\t\r\n"

// The Golbal stuct.
IRSettings g_irSetting;

#define IR_DAEMON_TEMP  "/tmp/ir-daemon.pid"

IRSettings::IRSettings()
{
    m_passThroughFlag = false;
    m_irPassThroughPeerIP[0] = m_irPassThroughPeerIP[1]= m_irPassThroughPeerIP[2] = m_irPassThroughPeerIP[3] = 0;
    m_timeRepeatHoldoff = 25;
    m_timeCommandHoldoff = 90;
    m_myip[0] = m_myip[1] = m_myip[2] = m_myip[3] = 0;
    m_repeatFlag = false;
    m_testStatus = IR_TEST_END;
}


IRSettings::~IRSettings()
{
}

bool IRSettings::getRepeatFlag()
{
    return (m_repeatFlag);
}

void IRSettings::setRepeatFlag(bool flag)
{
    m_repeatFlag = flag;
}

int IRSettings::getRepeatHoldOff()
{
    return (m_timeRepeatHoldoff);
}

void IRSettings::setRepeatHoldOff(int repeatHoldOff)
{
    if (repeatHoldOff >= 0)
    {
        m_timeCommandHoldoff = repeatHoldOff;
    }
    else
    {
        logError("Set invalid repeatHoldOff: %dms", repeatHoldOff);
    }
}


int IRSettings::getCommandHoldOff()
{
    return (m_timeCommandHoldoff);
}

void IRSettings::setCommandHoldOff(int commandHoldOff)
{
    if (commandHoldOff >= 0)
    {
        m_timeCommandHoldoff = commandHoldOff;
    }
    else
    {
        logError("Set invalid commandHoldoff: %dms", commandHoldOff);
    }
}

void IRSettings::getTestResult(UINT32 &wordCount, UINT32 &totalTime)
{
    wordCount = m_testWordCount;
    totalTime = m_testTotalTime;
}

void IRSettings::setTestResult(MsgType *msg)
{
    m_testWordCount = msg->m_header.m_msgDataLength / 4;
    m_testTotalTime = getTotalTime(msg);
}


/*---------------------------------------------------------------------------*
 * Routine: getIrConfig
 *---------------------------------------------------------------------------*/
/**
 *  Read IR Setting from WebServer service through DBus.
 *
 */
/*---------------------------------------------------------------------------*/
void IRSettings::getIrConfig()
{
    std::string ipAddr;
    int err;

    std::map<std::string,std::string> m_sconfig = g_pCmsfsIrInterface->getAll();

    if (!m_sconfig.empty())
    {
#ifdef _DEBUG_      
        // Debug
        std::cout << "Config From CMFS: " << std::endl;
        for (auto const& x : m_sconfig)
        {
            std::cout << x.first  // string (key)
                << ':' 
                << x.second // string's value 
                << std::endl ;
        } 
        std::cout << std::endl;
        //end debug
#endif

        // Set ir parameters
        this->setCommandHoldOff(std::stoi(m_sconfig[IR_COMMAND_HOLD_OFF], nullptr, 0));
        this->m_timeRepeatHoldoff = (std::stoi(m_sconfig[IR_REPEAT_HOLD_OFF], nullptr, 0));
        this->m_passThroughFlag = (std::stoi(m_sconfig[IR_ENABLE_PASSTHROUGH], nullptr, 0));

        /* set peer ip address */
        ipAddr = m_sconfig[IR_PASSTHROUGH_PEER_IP];
        err = inet_pton(AF_INET, ipAddr.c_str(), this->m_irPassThroughPeerIP);
        if (err < 1) {
            logInfo("Invalid IPv4 Address, set to default.");
            this->m_irPassThroughPeerIP[0] = 0;
            this->m_irPassThroughPeerIP[1] = 0;
            this->m_irPassThroughPeerIP[2] = 0;
            this->m_irPassThroughPeerIP[3] = 0;
        }
        logInfo("PassThrough Peer IP: %d.%d.%d.%d", this->m_irPassThroughPeerIP[0],
                this->m_irPassThroughPeerIP[1], this->m_irPassThroughPeerIP[2], this->m_irPassThroughPeerIP[3]);
        logInfo("PassThrough flag = %d", this->m_passThroughFlag);

        //Setup my IP
        //establishes a connection to ConfigManagerService
        cmAPI* api = getCMApi();
  
        //NetworkCfg net instantiation gives access to network configuration sets/gets
        NetworkCfg net(api);
        ipAddr = net.getIP();  // Currently eth0 link local. Should be br0/br1 once we get Jarvis support
        logInfo("Jarvis ip: %s.", ipAddr.c_str());
        err = inet_pton(AF_INET, ipAddr.c_str(), this->m_myip);
        if (err < 1) {
            logInfo("Invalid IPv4 Address, set to default.");
            this->m_myip[0] = 0;
            this->m_myip[1] = 0;
            this->m_myip[2] = 0;
            this->m_myip[3] = 0;
        }
    }
    else
    {
        //empty config is not good
        logError("[%s] Config Not Found In CMSFS", DAEMON_NAME);
        exit(1);
    }
}

BOOL isHex(char  c)
{
    if ((c >= '0') && (c <= '9'))
    {
        return (true);
    }
    else if ((c >= 'a') && (c <= 'f'))
    {
        return (true);
    }
    else if ((c >= 'A') && (c <= 'F'))
    {
        return (true);
    }
    return (false);
}

int getHex(char c)
{
    if ((c >= '0') && (c <= '9'))
    {
        return (c - '0');
    }
    else if ((c >= 'a') && (c <= 'f'))
    {
        return (c - 'a' + 10);
    }
    else if ((c >= 'A') && (c <= 'F'))
    {
        return (c - 'A' + 10);
    }
    return (-1);    
}

/*---------------------------------------------------------------------------*
 * Routine: loadIRCommand
 *---------------------------------------------------------------------------*/
/**
 *  Load an IR command stored in Philips Pronto format from a file.
 *
 *  @param [in] irCmdBuf -- The buffer with string type to load
 *  @param [out] irParams -- Output of imported file
 *
 *  @return Error code, 0=none
 */
/*---------------------------------------------------------------------------*/
STATUS ProntoCodeType::loadIRCommand(const char *irCmdBuf)
{
    STATUS ret = STATUS_ERROR;
    UINT32 i;
    ProntoCodeHeaderType *header;
    UINT32 code[MAX_IR_VALUES];
    UINT32 codeCount = 0;
    UINT32 *seq1Code, *seq2Code;

    if ((irCmdBuf == NULL) || (strlen(irCmdBuf) == 0))
    {
        logError("Invalid IR Code = NULL!");
        return STATUS_ERROR;
    }
    // Make a copy instance of the ir command string.
    char *strBuf = strdup(irCmdBuf);

    // Reset length of sequence and check if file opened
    this->m_seq1Count = 0;
    this->m_seq2Count = 0;

    UINT32 value = 0;
    bool numberFound = false;

    // Read the IR code format
    for (i = 0; strBuf[i] != 0; i++)
    { 
        if (isHex(strBuf[i]) == false)
        {
            if (strchr(PRONTO_SPLIT_TOKEN, strBuf[i]) == NULL)
            {
                logError("Invalid IRCode:%s i = %d, char = %d ", strBuf, i, strBuf[i]);
                goto End;
            }
            else
            {
                if (numberFound == false)
                {
                    continue;
                }
                else
                {
                    numberFound = false;
                    code[codeCount] = value;
                    codeCount++;
                    value = 0;

                    if (codeCount >= MAX_IR_VALUES)
                    {
                        logError("Too much IR Code");
                        goto End;
                    }
                }
            }
        }
        else
        {
            numberFound = true;
            value = (value << 4) | getHex(strBuf[i]);
            if (value > 0xFFFF)
            {
                logError("Invalid irCode: %s", strBuf);
                goto End;
            }
        }
    }
    if (numberFound == true)
    {
        numberFound = false;
        code[codeCount] = value;
        codeCount++;
        value = 0;      
    }

    /* check the IR Code. */
    header = (ProntoCodeHeaderType *)code;

    if (header->m_type != IR_PRONTO_RAW_OSCILLATED_CODE)
    {
        logError("Invalid Pronto Code Format: %x", header->m_type);
        goto End;
    }

    if (header->m_freq > 0xFF)
    {
        logError("Invalid Carrier frequency: 0x%x", header->m_freq);
        goto End;
    }

    if ((header->m_seq1Count * 2 + header->m_seq2Count * 2 + 4) != codeCount)
    {
        logError("seq1 count(%d) + seq2 count(%d) + header count(4) != %d", header->m_seq1Count, header->m_seq2Count, codeCount);
        goto End;
    }

    if (header->m_seq1Count > MAX_IR_VALUES)
    {
        logError("Invalid sequence1 count: %d", header->m_seq1Count);
        goto End;
    }

    if (header->m_seq2Count > MAX_IR_VALUES)
    {
        logError("Invalid sequence2 count: %d", header->m_seq2Count);
        goto End;
    }
    
    // Record the IR Code.
    logDebug("Pronco Code Header: %d, %d, %d, %d", header->m_type, header->m_freq, header->m_seq1Count, header->m_seq2Count);
    this->m_carryFreq = header->m_freq;
    this->m_periodInUs = (double)m_carryFreq * 0.241246;
    logDebug("IR pulse period: %d us", (int)(this->m_periodInUs));
    logDebug("IR carrier: %d", freqToHz(this->m_carryFreq));

    // Read sequence #1
    logDebug("Sequence 1: ");
    seq1Code = &code[4];
    for (i = 0; i < header->m_seq1Count * 2; i++) 
    {
        this->m_seq1[i] = seq1Code[i];
        this->m_seq1Out[i] = round(this->m_periodInUs * seq1Code[i]);
        logDebug(" %d: %x - %d", i, this->m_seq1[i], this->m_seq1Out[i]);
    }
    this->m_seq1Count = header->m_seq1Count;

    // Read sequence #2
    logDebug("Sequence 2: ");
    seq2Code = &code[4 + header->m_seq1Count];
    for (i = 0; i < header->m_seq2Count * 2; i++) 
    {
        this->m_seq2[i] = seq2Code[i];
        this->m_seq2Out[i] = round(this->m_periodInUs * seq2Code[i]);
        logDebug(" %d: %x - %d", i, this->m_seq2[i], this->m_seq2Out[i]);
    }
    this->m_seq2Count = header->m_seq2Count;

    ret = STATUS_OK;

End:
    free(strBuf);
    return ret;
}


/*---------------------------------------------------------------------------*
 * Routine: freqToHz
 *---------------------------------------------------------------------------*/
/**
 *  change the pronto freq code to freuency in Hz 
 *
 *  @param [in] prontoCodeFreq -- the frequency code in pronto. For example:  0x67 means 40KHz; 0x6d = 38KHz.
 *
 *  @return frequency in Hz, for example, freqToHz(0x67) = 40244;  freqToHz(0x6d) = 38029
 */
/*---------------------------------------------------------------------------*/
int ProntoCodeType::freqToHz(UINT8 prontoCodeFreq)
{
    double perioduS = (double)prontoCodeFreq * 0.241246;
    int carryFreqHz = round((double)1000000 / perioduS);
    return (carryFreqHz);
}

void ProntoCodeType::dump()
{
#ifdef _DEBUG_
    int i;
    logDebug("carryFreq = %d Hz", freqToHz(m_carryFreq));
    logDebug("period = %f us", m_periodInUs);
    logDebug("seq1Count = %d ", m_seq1Count);
    for (i = 0; i < m_seq1Count; i++)
    {
        logDebug("  %d: +%d -%d / +%d - %d", i + 1,  m_seq1[2 * i], m_seq1[2 *i + 1], m_seq1Out[2 * i], m_seq1Out[2 * i + 1]);
    }
    logDebug("seq2Count = %d ", m_seq2Count);
    for (i = 0; i < m_seq2Count; i++)
    {
        logDebug("  %d: +%d -%d / +%d - %d", i + 1,  m_seq2[2 * i], m_seq2[2 *i + 1], m_seq2Out[2 * i], m_seq2Out[2 * i + 1]);
    }
#endif
}

/*---------------------------------------------------------------------------*
 * Routine: createIRBuffer
 *---------------------------------------------------------------------------*/
/**
 *  change the pronto code to a standard IR pulse and space width code. 
 *
 *  @return the standard IR pulse and space width code, which can be used directly for IR Tx driver.
 */
/*---------------------------------------------------------------------------*/
MsgType* ProntoCodeType::createIRBuffer()
{
    int msgLen = sizeof(MsgHeaderType) + ((m_seq1Count + m_seq2Count) * 2) * 4;
    UINT8 *newBuffer = new UINT8[msgLen];

    if (newBuffer == NULL)
    {
        return (NULL);
    }
    
    /* Fill the msg header */
    MsgType *msg = (MsgType *)newBuffer;
    IRRawHeaderType *header = (IRRawHeaderType *)newBuffer;

    header->m_sendModuleID = MODULE_ID_WEB_SERVER;
    header->m_recvModuleID = MODULE_ID_IR_SEND_DRIVER;
    header->m_msgID = MSG_ID_IR_RAW_ONCE;
    header->m_freq = m_carryFreq;
    header->m_msgDataLength = ((m_seq1Count + m_seq2Count) * 2 -1) * 4;
    
    int index = 0;
    UINT32 *data = (UINT32 *)(msg->m_data);
    
    /* Fill the seq1 */
    for (int i = 0; i < m_seq1Count; i++)
    {
        data[index] = m_seq1Out[2 * i];
        index++;
        
        data[index] = m_seq1Out[2 * i + 1];
        index++;
    }
    
    // We can add RepeatHoladOff here 
    if ((m_seq1Count != 0) && (m_seq2Count != 0))
    {
        index--;
        // Change m_timeRepeatHoldoff from ms to us.
        data[index] += (g_irSetting.getRepeatHoldOff() * 1000);
        index++;
    }

    /* Fill the seq2 */
    for (int i = 0; i < m_seq2Count; i++)
    {
        data[index] = m_seq2Out[2 * i];
        index++;
        
        data[index] = m_seq2Out[2 * i + 1];
        index++;
    }
    
    /* Calc Checksum */
    UINT8 checkSum = 0;
    for (int i = 0; i < header->m_msgDataLength; i++)
    {
        checkSum ^= msg->m_data[i];
    }
    header->m_msgDataCheckSum = checkSum;   
    
    return (msg);
}

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

UINT32 getTotalTime(MsgType *msg)
{
    MsgHeaderType *rpmsg = (MsgHeaderType *)msg; 
    UINT32 len = rpmsg->m_msgDataLength;
    UINT32 time = 0;
    UINT32 *data = (UINT32 *)msg->m_data;
    for (UINT32 i = 0; i < (len / 4); i++)
    {
        time += data[i];
    }
    return time;
}

void dumpIRRaw(MsgType* msg)
{
    logInfo("Toal words number = %d, Total time = %d us", msg->m_header.m_msgDataLength / 4, getTotalTime(msg));
    dumpDecimal(msg->m_data, msg->m_header.m_msgDataLength, sizeof(UINT32));
}
