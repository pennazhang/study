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

#ifndef _CMDHANDLERINTERFACE_H_
#define _CMDHANDLERINTERFACE_H_

#include <dbus-c++-1/dbus-c++/dbus.h>
#include "CmdHandlerManagerProxy-glue.h"

/**
 * This is a DBUS Proxy class that provides the interface to
 * the CmdHandlerManager interfaces.
 */
class CmdHandlerInterface
    : public com::harman::videoworx::CmdHandlerManager_proxy, public DBus::ObjectProxy
{
public:
    /**
     * \brief CmdHandlerInterface constructor.
     * \param connection The dbus connection object.
     */
    CmdHandlerInterface(DBus::Connection &connection);
    /**
     * \brief A notification indicates to run the serial commands.
     * \param cmdStr The commands in string type.
     * \param cmdType The command type.
     * \param cmdArg The string type of cmdArg.
     * \return None.
     */
    void processSettings(const std::string& cmdStr, const uint8_t& cmdType, const std::string& cmdArg);
    /**
     * \brief A notification indicates to run the serial commands.
     * \param cmdStr The commands in string type.
     * \param cmdType The command type.
     * \param cmdArg The string type of cmdArg.
     * \return None.
     */
    void sendIR(const std::string& cmdStr, const uint8_t& cmdType, const std::string& cmdArg);
    /**
     * \brief A notification indicates to run the serial commands.
     * \param cmdStr The commands in string type.
     * \param cmdType The command type.
     * \param cmdArg The string type of cmdArg.
     * \return None.
     */
    void sendIRRaw(const std::string& cmdStr, const uint8_t& cmdType, const std::string& cmdArg);
    /**
     * \brief A notification indicates to run the serial commands.
     * \param cmdStr The commands in string type.
     * \param cmdType The command type.
     * \return None.
     */
    void TestIRCommand(const std::string& cmdStr, const uint8_t& cmdType);
    /**
     * \brief A notification indicates to run the serial commands.
     * \param cmdStr The commands in string type.
     * \param cmdType The command type.
     * \param cmdArg The string type of cmdArg.
     * \return None.
     */
    void repeatIR(const std::string& cmdStr, const uint8_t& cmdType, const std::string& cmdArg);
    /**
     * \brief A notification indicates to run the serial commands.
     * \param cmdStr The commands in string type.
     * \param cmdType The command type.
     * \return None.
     */
    void stopIR(const std::string& cmdStr, const uint8_t& cmdType);
    /**
     * \brief Repeat IR as count = DEFAULT_IR_REPEAT_COUNT
     * \param cmdStr The commands in string type.
     * \param cmdType The command type.
     * \param cmdArg The string type of cmdArg.
     * \return None.
     */
    void repeatIRCount(const std::string& cmdStr, const uint8_t& cmdType, const std::string& cmdArg);
    /**
     * \brief Repeat IR as total time = DEFAULT_REPEAT_TIME_IN_MS
     * \param cmdStr The commands in string type.
     * \param cmdType The command type.
     * \param cmdArg The string type of cmdArg.
     * \return None.
     */
    void repeatIRTime(const std::string& cmdStr, const uint8_t& cmdType, const std::string& cmdArg);
    /**
     * \brief Repeat IR until g_irSetting.m_repeatFlag is set to FALSE.
     * \param cmdStr The commands in string type.
     * \param cmdType The command type.
     * \param cmdArg The string type of cmdArg.
     * \return None.
     */
    void repeatIRUntil(const std::string& cmdStr, const uint8_t& cmdType, const std::string& cmdArg);






};

#endif /* _CMDHANDLERINTERFACE_H_ */
