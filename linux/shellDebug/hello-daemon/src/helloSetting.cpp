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

int add(const std::string& cmdArg, std::string& outputInfo)
{
    std::vector<std::string> params = split(cmdArg, ',');
   	for(UINT32 i = 0; i < params.size(); i++)
    {
        printf("param[%d] = [%s]\n", i, params[i].c_str());
    } 
    return (params.size());
}

int onSetDelayTime(const std::string &cmdArg, std::string &outputInfo)
{
    int value = atoi(cmdArg.c_str());
//    logInfo("cmdArg = %s, value = %d", cmdArg.c_str(), value);
    if (value > 10)
    {
        return (STATUS_OK);
    }
    else
    {
        outputInfo = std::string("Invalid delayTime: ") + cmdArg + std::string(" : ASSERT(delayTime >= 10)");
        return (STATUS_ERROR);
    }
}

int onSetMuteFlag(const std::string &cmdArg, std::string &outputInfo)
{
//    logInfo("cmdArg = %s", cmdArg.c_str());
    if ((cmdArg == "true") || (cmdArg == "false"))
    {
        return (STATUS_OK);
    }
    else
    {
        outputInfo = std::string("Invalid muteFlag: ") + cmdArg + std::string(" : ASSERT((muteFlag == true) || (muteFlag == false))");
        return (STATUS_ERROR);
    }
}
