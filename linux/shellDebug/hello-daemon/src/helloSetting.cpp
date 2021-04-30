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
#include "helloSetting.h"
#include <fcntl.h>
#include "utility.h"	// mSleep
#include <stdlib.h>     // abs 


int setIRCommandID(const std::string& cmdArg, std::string& outputInfo)
{
    logInfo("setIRCommandID called here, cmdArg = %s", cmdArg.c_str());
    outputInfo = "Timeout!";
    return (STATUS_ERROR);
}

int setIRCommands(const std::string& cmdArg, std::string& outputInfo)
{
    logInfo("setIRCommands called here, cmdArg = %s", cmdArg.c_str());
    outputInfo = "Timeout!";
    return (STATUS_ERROR);
}

// cmdArg = "25"
int setCommandHoldOff(const std::string& cmdArg, std::string& outputInfo)
{
    logInfo("setCommandHoldOff called here, cmdArg = %s", cmdArg.c_str());
    outputInfo = "Timeout!";
    return (STATUS_ERROR);
}

// cmdArg = "25"
int setRepeatHoldOff(const std::string& cmdArg, std::string& outputInfo)
{
    logInfo("setRepeatHoldOff called here, cmdArg = %s", cmdArg.c_str());
    outputInfo = "Timeout!";
    return (STATUS_ERROR);
}

int sendIRRaw(const std::string& cmdArg, std::string& outputInfo)
{
    logInfo("sendIRRaw called here, cmdArg = %s", cmdArg.c_str());
    outputInfo = "Timeout!";
    return (STATUS_ERROR);
}

int sendIR(const std::string& cmdArg, std::string& outputInfo)
{
    logInfo("sendIR called here, cmdArg = %s", cmdArg.c_str());
    outputInfo = "Timeout!";
    return (STATUS_ERROR);
}

int testIR(const std::string& cmdArg, std::string& outputInfo)
{
    logInfo("testIR called here, cmdArg = %s", cmdArg.c_str());
    outputInfo = "Timeout!";
    return (STATUS_ERROR);
}

int repeatIR(const std::string& cmdArg, std::string& outputInfo)
{
    logInfo("repeatIR called here, cmdArg = %s", cmdArg.c_str());
    outputInfo = "Timeout!";
    return (STATUS_ERROR);
}

int stopIR(const std::string& cmdArg, std::string& outputInfo)
{
    logInfo("stopIR called here, cmdArg = %s", cmdArg.c_str());
    outputInfo = "Timeout!";
    return (STATUS_ERROR);
}

//- set time for an IR to hold off between commands in ms
int setIRCmdTime(const std::string& cmdArg, std::string& outputInfo)
{
    logInfo("setIRCmdTime called here, cmdArg = %s", cmdArg.c_str());
    outputInfo = "Timeout!";
    return (STATUS_ERROR);
}

//- set time for an IR to hold off between command and repeat.
int setIRRepeatTime(const std::string& cmdArg, std::string& outputInfo)
{
    logInfo("setIRRepeatTime called here, cmdArg = %s", cmdArg.c_str());
    outputInfo = "Timeout!";
    return (STATUS_ERROR);
}

//- set time for an IR to hold off between command and repeat.
int enablePassThrough(const std::string& cmdArg, std::string& outputInfo)
{
    logInfo("enablePassThrough called here, cmdArg = %s", cmdArg.c_str());
    outputInfo = "Timeout!";
    return (STATUS_ERROR);
}


int setPeerIPAddress(const std::string& cmdArg, std::string& outputInfo)
{
    logInfo("setPeerIPAddress called here, cmdArg = %s", cmdArg.c_str());
    outputInfo = "Timeout!";
    return (STATUS_ERROR);
}
