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

#include <dbus-c++-1/dbus-c++/dbus.h>
#include "CmsfsIrProxy-glue.h"
#include "CmsfsUtils.h"
#include "typedef.h"

#define     IR_COMMAND_HOLD_OFF                 "irCommandHoldoff"
#define     IR_REPEAT_HOLD_OFF                  "irRepeatHoldoff"
#define     EXECUTE_IR_COMMAND                  "ExecuteIRCommand"
#define     IR_ENABLE_PASSTHROUGH                  "irEnablePassThrough"
#define     IR_PASSTHROUGH_PEER_IP                 "irPassThroughPeerIP"


/**
 * This is a DBUS Proxy class that provides the interface to
 * the CmsfsIr interface.
 */
class CmsfsIrInterface
    : public CmsfsFuncCallbacks,
      public com::harman::video::fs1_proxy, public DBus::ObjectProxy
{
public:
    /**
     * \brief CmsfsIrInterface constructor.
     * \param connection The dbus connection object.
     */
    CmsfsIrInterface(DBus::Connection &connection);
    /**
     * \brief Callback function to handle the paramenter for the DataChanged signal.
     * \return None
     */
    void RegisterFunctions(void);
    /**
     * \brief Signal handler for DataChanged signal
     * \param interface String with the interface identifier
     * \param jsonvalues Key/Value Pair map of values that changed
     * \return None
     */
    void DataChanged(const std::string& interface, const std::map< std::string, std::string >& jsonvalues);
    /**
     * \brief Set ready to run.
     * \param readytorun Boolean to indicate ready to handle signals.
     */
    void SetReadyRun(bool readytorun);
private:
    bool _readytorun;

    void CommandHoldOff(std::string para);
    void RepeatHoldOff(std::string para);
    STATUS ExecuteIRCommand(std::string para);
    void EnablePassThrough(std::string para);
    void setPeerIPAddress(std::string para);
};

extern CmsfsIrInterface* g_pCmsfsIrInterface;
