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
#include <arpa/inet.h>
#include <boost/lexical_cast.hpp>
#include "CmsfsIrInterface.h"
#include "config.h"

static const char* ADAPTER_PATH = "/com/harman/video/fs1/ir";
static const char* SERVICE_NAME = "com.harman.video.fs1";

using namespace std::placeholders;

//
// Class CmsfsIrInterface
//
CmsfsIrInterface::CmsfsIrInterface(DBus::Connection &connection)
: DBus::ObjectProxy(connection, ADAPTER_PATH, SERVICE_NAME)
{
    SVSI_SYSLOG();
    _readytorun = false;
    RegisterFunctions();
}

void CmsfsIrInterface::RegisterFunctions(void)
{
    registerFunc(IR_COMMAND_HOLD_OFF, std::bind(&CmsfsIrInterface::CommandHoldOff, this, _1 ));
    registerFunc(IR_REPEAT_HOLD_OFF,  std::bind(&CmsfsIrInterface::RepeatHoldOff,  this, _1 ));
    registerFunc(EXECUTE_IR_COMMAND,  std::bind(&CmsfsIrInterface::ExecuteIRCommand, this, _1 ));
}

void CmsfsIrInterface::DataChanged(const std::string& interface, const std::map< std::string, std::string >& jsonvalues)
{
    SVSI_SYSLOG("DataChanged called");

    // Only process datachanged signals if the daemon is ready to handle them
    // and we were not the one to cause the change
    //if(_readytorun && (DAEMON_IFNAME != interface))
    if(_readytorun)
    {
        // Iterate over the map of changed config pairs
        for (auto const& x : jsonvalues)
        {
            // Temp DEBUG
            std::cout << "Config element: " << x.first  // string (key)
                    << ':' 
                    << x.second // string's value 
                    << std::endl ;
            if (findFuncExec(x.first, x.second))
            {

            }
            // Unknown or unhandled config element
            else
            {
                SVSI_SYSLOG("CmsfsIrInterface Unknown or unhandled data element received: %s\n", x.first); 
                std::cout << x.first  // string (key)
                    << ':' 
                    << x.second // string's value 
                    << std::endl ;
            }
        }
    }
    else 
    {
        SVSI_SYSLOG("CmsfsIrInterface DataChanged not handled as daemon not readtorun"); 
    }
}

void CmsfsIrInterface::SetReadyRun(bool readytorun)
{
    SVSI_SYSLOG();
    _readytorun = readytorun;
}


//
// Callback functions
//
void CmsfsIrInterface::CommandHoldOff(std::string para)
{
    G_irCommandFIFO.timeCommandHoldoff = std::stoi(para, nullptr, 0);
}
void CmsfsIrInterface::RepeatHoldOff(std::string para)
{
    G_irCommandFIFO.timeRepeatHoldoff = std::stoi(para, nullptr, 0);
}
void CmsfsIrInterface::ExecuteIRCommand(std::string para)
{
    SVSI_SYSLOG(": %s", para);
}
