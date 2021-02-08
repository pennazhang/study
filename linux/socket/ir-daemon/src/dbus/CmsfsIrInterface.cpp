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
#include "jsIrUtils.h"
#include "ClientSetting.h"
#include "ServerSetting.h"

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
    registerFunc(IR_ENABLE_PASSTHROUGH,  std::bind(&CmsfsIrInterface::EnablePassThrough, this, _1 ));
    registerFunc(IR_PASSTHROUGH_PEER_IP,  std::bind(&CmsfsIrInterface::setPeerIPAddress, this, _1 ));
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
            logDebug("Config element: %s: %s ", x.first << x.second );
            if (findFuncExec(x.first, x.second))
            {

            }
            // Unknown or unhandled config element
            else
            {
                logError("CmsfsIrInterface Unknown or unhandled data element received: %s: %s", x.first, x.second);
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


/*---------------------------------------------------------------------------*
 * Routine: CommandHoldOff
 *---------------------------------------------------------------------------*/
/**
 *  Change the time of CommandHoldOff
 *
 *  @param [in] para -- time of CommandHoldOff in MillSecond. For example: "30" means 30ms.
 *
 *  @return none
 */
/*---------------------------------------------------------------------------*/
void CmsfsIrInterface::CommandHoldOff(std::string para)
{
    g_irSetting.setCommandHoldOff(std::stoi(para, nullptr, 0));
}

/*---------------------------------------------------------------------------*
 * Routine: RepeatHoldOff
 *---------------------------------------------------------------------------*/
/**
 *  Change the time of RepeatHoldOff
 *
 *  @param [in] para -- time of RepeatHoldOff in MillSecond. For example: "30" means 30ms.
 *
 *  @return none
 */
/*---------------------------------------------------------------------------*/
void CmsfsIrInterface::RepeatHoldOff(std::string para)
{
    g_irSetting.setRepeatHoldOff(std::stoi(para, nullptr, 0));
}

/*---------------------------------------------------------------------------*
 * Routine: ExecuteIRCommand
 *---------------------------------------------------------------------------*/
/**
 *  Load an IR command stored in Philips Pronto format from a file, and send it out through IR Tx Driver
 *
 *  @param [in] para -- the pronto code that will be sent out through IR Tx Driver.
 *                      Normally the format should be like this: "0000 006a 0013 0013 0141 00a1 0014 0014 0014 0014 0014 0015 0014 0015 0014 003d 0014 003d 0014 003d 0014 003c 0014 00a1 0014 0014 0014 0014 0014 0014 0014 0014 0014 0014 0014 0015 0014 003d 0014 0014 0014 03ae 0141 00a1 0014 0014 0014 0014 0014 0014 0014 0015 0014 003d 0014 003d 0014 003d 0014 003c 0014 00a1 0014 0014 0014 0014 0014 0014 0014 0014 0014 0014 0014 0015 0014 003d 0014 0014 0014 03b8";
 *
 *  @return send result. 0 = Send successfully, -1 = Send failed.
 */
/*---------------------------------------------------------------------------*/
STATUS CmsfsIrInterface::ExecuteIRCommand(std::string para)
{
    ProntoCodeType irCode;

//    para = "0000 006a 0013 0013 0141 00a1 0014 0014 0014 0014 0014 0015 0014 0015 0014 003d 0014 003d 0014 003d 0014 003c 0014 00a1 0014 0014 0014 0014 0014 0014 0014 0014 0014 0014 0014 0015 0014 003d 0014 0014 0014 03ae 0141 00a1 0014 0014 0014 0014 0014 0014 0014 0015 0014 003d 0014 003d 0014 003d 0014 003c 0014 00a1 0014 0014 0014 0014 0014 0014 0014 0014 0014 0014 0014 0015 0014 003d 0014 0014 0014 03b8";

    logDebug("ExecuteIRCommand: %s", para.c_str());

    // See if valid IR command is loaded before attempting to send
    if (irCode.loadIRCommand(para.c_str()) != STATUS_OK) 
    {
        logError("invalid IR data");
        return STATUS_ERROR;
    }
    irCode.dump();
    
    MsgType *irBuffer = irCode.createIRBuffer();
    if (irBuffer == NULL)
    {
        return STATUS_ERROR;
    }
    
    MsgQueue *msgQueue = g_server.getMsgQueue();
    msgQueue->putMsg((UINT8 *)irBuffer, 0, irBuffer->m_header.m_msgDataLength + sizeof(MsgHeaderType), true);   
    return (STATUS_OK);
}

/*---------------------------------------------------------------------------*
 * Routine: EnablePassThrough
 *---------------------------------------------------------------------------*/
/**
 *  Enable or disable PassThrough function.
 *
 *  @param [in] para -- "0" or "1".  "0" means disable PassThrough function.  "1" means enable PassThrough function.
 *
 *  @return none
 */
/*---------------------------------------------------------------------------*/
void CmsfsIrInterface::EnablePassThrough(std::string para)
{
    logDebug("EnablePassThrough called.");
    
//    int flag = std::stoi(para, nullptr, 0);
    if (para.compare("0") == 0)
    {
        g_irSetting.m_passThroughFlag = FALSE;
    }
    else if (para.compare("1") == 0)
    {
        g_irSetting.m_passThroughFlag = TRUE;
    }
    else
    {
        logError("EnablePassThrough failed for invalid para: %s", para.c_str());
    }

    g_client.reset(g_irSetting.m_passThroughFlag, g_irSetting.m_irPassThroughPeerIP);
    g_server.reset(g_irSetting.m_passThroughFlag, g_irSetting.m_irPassThroughPeerIP);    
}

/*---------------------------------------------------------------------------*
 * Routine: setPeerIPAddress
 *---------------------------------------------------------------------------*/
/**
 *  Set Peer IP Address for PassThrough function.
 *
 *  @param [in] para -- IP Address of Peer host.
 *
 *  @return none
 */
/*---------------------------------------------------------------------------*/
void CmsfsIrInterface::setPeerIPAddress(std::string para)
{
    UINT8 ipAddr[4];
    logDebug("setPeerIPAddress: %s.", para.c_str());
    int err = inet_pton(AF_INET, para.c_str(), ipAddr);
    if (err < 1) 
    {
        logError("Invalid IPv4 Address, set to default.");
        return;
    }
    memcpy(g_irSetting.m_irPassThroughPeerIP, ipAddr, 4);
    logDebug("Verify IP = %d.%d.%d.%d", g_irSetting.m_irPassThroughPeerIP[0], g_irSetting.m_irPassThroughPeerIP[1],
                 g_irSetting.m_irPassThroughPeerIP[2], g_irSetting.m_irPassThroughPeerIP[3]);

    g_client.reset(g_irSetting.m_passThroughFlag, g_irSetting.m_irPassThroughPeerIP);
    g_server.reset(g_irSetting.m_passThroughFlag, g_irSetting.m_irPassThroughPeerIP);   
}
