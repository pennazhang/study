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

#include <stdio.h>
#include <string.h>
#include "CmdHandlerInterface.h"
#include "CmsfsIrInterface.h"
#include "config.h"
#include "jsIrUtils.h"
#include "typedef.h"
#include "MsgQueue.h"
#include "ServerSetting.h"
#include "AppConfigAPI.h"
#include <json/json.h>
#include <iostream>

static const char* ADAPTER_PATH = "/com/harman/videoworx/CmdHandlerManager";
static const char* SERVICE_NAME = "com.harman.videoworx.CmdHandlerManager";

AppConfigAPI irApp("ir", "web_server");

CmdHandlerInterface::CmdHandlerInterface(DBus::Connection &connection)
    : DBus::ObjectProxy(connection, ADAPTER_PATH, SERVICE_NAME)
{
    SVSI_SYSLOG();
}

// cmdArg = "ircmdtime: 10"  or "irc2rtime: 20"
void CmdHandlerInterface::processSettings(const std::string& cmdStr, const uint8_t& cmdType, const std::string& cmdArg)
{
    logDebug("-%s : %d : %s", cmdStr.c_str(), cmdType, cmdArg.c_str());
    char commandArg[MAX_COMMAND_LENGTH];
    strcpy(commandArg, cmdArg.c_str());
    char *command = strtok(commandArg, ": ");
    char *value = strtok(NULL,"\r\n");

    if ((command == NULL) || (value == NULL))
    {
        logError("Invalid cmdArg: %s", cmdArg.c_str());
        return;
    }
    
    command = strstrip(command);
    value = strstrip(value);
    if ((command == NULL) || (value == NULL))
    {
        logError("Invalid cmdArg: %s", cmdArg.c_str());
        return;
    }
    int time = (int)strtol(value, NULL, 10);

    if (strncmp(command, "ircmdtime", strlen(command)) == 0) 
    {
        g_irSetting.setCommandHoldOff(time);
        logInfo("set CommandHoldOff to %d ms", time);
    }
    else if (strncmp(command, "irc2rtime", strlen(command)) == 0) 
    {
        g_irSetting.setRepeatHoldOff(time);
        logInfo("set RepeatHoldOff to %d ms", time);
    }
    return;
}


// For example: sendIR: samsung_tv_volume_+
void CmdHandlerInterface::sendIR(const std::string& cmdStr, const uint8_t& cmdType, const std::string& cmdArg)
{
    UINT32 i;

    logDebug("sendIR: %s", cmdArg.c_str());

    // First, we need to change the IR commandName to Pronto Code.
    Json::Value jsonIrCommands = Json::arrayValue;
    std::map<std::string, std::string> retValue = irApp.getAll();

    for (const auto &item: retValue)
    {
// The following code can be refered for debugging...        
//        if (i.first == "irCommandID")
//        {
//            int cmdCount = std::stol(i.second, nullptr, 0);
//            logDebug("cmdCount = %d");
            // Skip to register the element for ID number.
//            continue;
//        }
//        else 
        if (item.first == "irCommands")
        {
//            logDebug("second = %s", i.second.c_str());
            // Only convert the strings if the value is not empty.
            if ( !item.second.empty() )
            {
                Json::CharReaderBuilder reader;
                JSONCPP_STRING jerrs;
                std::istringstream isstream(item.second);

                if(Json::parseFromStream(reader, isstream, &jsonIrCommands, &jerrs) == false) 
                {
                    logError("Failed to parse the irCommands");
                    return;
                }
            }
        }
    }

    char commandName[100];
    strncpy(commandName, cmdArg.c_str(), 99);

    // All the commandsName and code are in jsonIrCommands now. 
    // We will find the exact IR commandName from jsonIrCommands.
    for (i = 0; i < jsonIrCommands.size(); i++)
    {
        char name2[100];
        strncpy(name2, jsonIrCommands[i]["irCommandname"].asString().c_str(), 99);
        if (strcmp(strstrip(commandName), strstrip(name2)) == 0)
        {
            logDebug("name = %s", jsonIrCommands[i]["irCommandname"].asString().c_str());
            logDebug("code = %s", jsonIrCommands[i]["irCommandCode"].asString().c_str());
            break;
        }
    }

    if (i >= jsonIrCommands.size())
    {
        logError("Can't find %s", strstrip(commandName));
        return;
    }


    // Send IR Raw
    ProntoCodeType irCode;
    logDebug("ExecuteIRCommand: %s", jsonIrCommands[i]["irCommandCode"].asString().c_str());

    // See if valid IR command is loaded before attempting to send
    if (irCode.loadIRCommand(jsonIrCommands[i]["irCommandCode"].asString().c_str()) != STATUS_OK) 
    {
        logError("invalid IR data");
//        return STATUS_ERROR;
        return;
    }
    irCode.dump();
    
    MsgType *irBuffer = irCode.createIRBuffer();
    if (irBuffer == NULL)
    {
//        return STATUS_ERROR;
        return;
    }
    
    MsgQueue *msgQueue = g_server.getMsgQueue();
    msgQueue->putMsg((UINT8 *)irBuffer, 0, irBuffer->m_header.m_msgDataLength + sizeof(MsgHeaderType), true);   
//    return (STATUS_OK);
    return;

}

void CmdHandlerInterface::sendIRRaw(const std::string& cmdStr, const uint8_t& cmdType, const std::string& cmdArg)
{
    ProntoCodeType irCode;

//    para = "0000 006a 0013 0013 0141 00a1 0014 0014 0014 0014 0014 0015 0014 0015 0014 003d 0014 003d 0014 003d 0014 003c 0014 00a1 0014 0014 0014 0014 0014 0014 0014 0014 0014 0014 0014 0015 0014 003d 0014 0014 0014 03ae 0141 00a1 0014 0014 0014 0014 0014 0014 0014 0015 0014 003d 0014 003d 0014 003d 0014 003c 0014 00a1 0014 0014 0014 0014 0014 0014 0014 0014 0014 0014 0014 0015 0014 003d 0014 0014 0014 03b8";

    logDebug("sendIRRaw: %s", cmdArg.c_str());

    // See if valid IR command is loaded before attempting to send
    if (irCode.loadIRCommand(cmdArg.c_str()) != STATUS_OK) 
    {
        logError("invalid IR data");
//        return STATUS_ERROR;
        return;
    }
    irCode.dump();
    
    MsgType *irBuffer = irCode.createIRBuffer();
    if (irBuffer == NULL)
    {
//        return STATUS_ERROR;
        return;
    }
    
    MsgQueue *msgQueue = g_server.getMsgQueue();
    msgQueue->putMsg((UINT8 *)irBuffer, 0, irBuffer->m_header.m_msgDataLength + sizeof(MsgHeaderType), true);   
//    return (STATUS_OK);
    return;
}

void CmdHandlerInterface::TestIRCommand(const std::string& cmdStr, const uint8_t& cmdType)
{
//    STATUS ret = STATUS_ERROR;
    logDebug("testIR ....");
    ProntoCodeType irCode;
    const char code[] = "0000 006C 0022 0000 015B 00AD 0016 0016 0016 0016 0016 0041 0016 0016 0016 0016 0016 0016 0016 0016 0016 0041 0016 0041 0016 0016 0016 0016 0016 0041 0016 0041 0016 0041 0016 0041 0016 0016 0016 0041 0016 0041 0016 0041 0016 0016 0016 0016 0016 0016 0016 0016 0016 0016 0016 0016 0016 0016 0016 0016 0016 0041 0016 0041 0016 0041 0016 0041 0016 0041 0016 0622";

    /* The last test is not finished yet. */
    if (g_irSetting.getTestStatus() != IR_TEST_END)
    {
//        return (STATUS_ERROR);
        return;
    }

    // See if valid IR command is loaded before attempting to send
    irCode.loadIRCommand(code); 
    MsgType *irBuffer = irCode.createIRBuffer();
    irBuffer->m_header.m_msgID = MSG_ID_IR_RAW_TEST;

    logInfo("Send test IR:");
    dumpIRRaw(irBuffer);

    g_irSetting.setTestStatus(IR_TEST_START);
    MsgQueue *msgQueue = g_server.getMsgQueue();
    msgQueue->putMsg((UINT8 *)irBuffer, 0, irBuffer->m_header.m_msgDataLength + sizeof(MsgHeaderType), true);

    // We will block the DBus for at most 5 seconds.
    TimePoint currentTime;
    for (;;)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));

        // Total test must be finished in 5000ms.
        if (currentTime.isTimeExpired(5000, false))
        {
            // TimeOut with result.
            logError("IRTest timeout without any response, status = %d", g_irSetting.getTestStatus());
            break;
        }

        if (g_irSetting.getTestStatus() == IR_TEST_RECV_IR)
        {
            UINT32 wordCount, totalTime;
            g_irSetting.getTestResult(wordCount, totalTime);
            if ((wordCount == 67) && (abs(totalTime - 67608) < 2000))
            {
                logInfo("IR test Ok! word Len = %d, total time = %d us (standard 67608 us)", wordCount, totalTime);
            //    ret = STATUS_OK;
            }
            else
            {
                logInfo("IR test Failed! word Len = %d, total time = %d us (standard 67608 us)", wordCount, totalTime);
            }
            break;
        }
    }
    g_irSetting.setTestStatus(IR_TEST_END);

//    return (ret);
    return;

}

void CmdHandlerInterface::repeatIR(const std::string& cmdStr, const uint8_t& cmdType, const std::string& cmdArg)
{
    repeatIRUntil(cmdStr, cmdType, cmdArg);
// Do some unit test here...
//    repeatIRTime(cmdStr, cmdType, cmdArg);
//    processSettings(cmdStr, cmdType, cmdArg);
}

void CmdHandlerInterface::repeatIRUntil(const std::string& cmdStr, const uint8_t& cmdType, const std::string& cmdArg)
{
    ProntoCodeType irCode;
    logDebug("repeatIRUntil: %s", cmdArg.c_str());
//    para = "0000 006a 0013 0013 0141 00a1 0014 0014 0014 0014 0014 0015 0014 0015 0014 003d 0014 003d 0014 003d 0014 003c 0014 00a1 0014 0014 0014 0014 0014 0014 0014 0014 0014 0014 0014 0015 0014 003d 0014 0014 0014 03ae 0141 00a1 0014 0014 0014 0014 0014 0014 0014 0015 0014 003d 0014 003d 0014 003d 0014 003c 0014 00a1 0014 0014 0014 0014 0014 0014 0014 0014 0014 0014 0014 0015 0014 003d 0014 0014 0014 03b8";

    // See if valid IR command is loaded before attempting to send
    if (irCode.loadIRCommand(cmdArg.c_str()) != STATUS_OK) 
    {
        logError("invalid IR data");
//        return STATUS_ERROR;
        return;
    }
    irCode.dump();
    
    MsgType *irBuffer = irCode.createIRBuffer();
    if (irBuffer == NULL)
    {
//        return STATUS_ERROR;
        return;
    }
    irBuffer->m_header.m_msgID = MSG_ID_IR_RAW_REPEAT;

    MsgQueue *msgQueue = g_server.getMsgQueue();
    msgQueue->putMsg((UINT8 *)irBuffer, 0, irBuffer->m_header.m_msgDataLength + sizeof(MsgHeaderType), true);   
    
//    return (STATUS_OK);
    return;
}

void CmdHandlerInterface::repeatIRCount(const std::string& cmdStr, const uint8_t& cmdType, const std::string& cmdArg)
{
    ProntoCodeType irCode;

//    para = "0000 006a 0013 0013 0141 00a1 0014 0014 0014 0014 0014 0015 0014 0015 0014 003d 0014 003d 0014 003d 0014 003c 0014 00a1 0014 0014 0014 0014 0014 0014 0014 0014 0014 0014 0014 0015 0014 003d 0014 0014 0014 03ae 0141 00a1 0014 0014 0014 0014 0014 0014 0014 0015 0014 003d 0014 003d 0014 003d 0014 003c 0014 00a1 0014 0014 0014 0014 0014 0014 0014 0014 0014 0014 0014 0015 0014 003d 0014 0014 0014 03b8";
    logDebug("repeatIRCount: %s", cmdArg.c_str());

    // See if valid IR command is loaded before attempting to send
    if (irCode.loadIRCommand(cmdArg.c_str()) != STATUS_OK) 
    {
        logError("invalid IR data");
//        return STATUS_ERROR;
        return;
    }
    irCode.dump();
    
    MsgType *irBuffer = irCode.createIRBuffer();
    if (irBuffer == NULL)
    {
//        return STATUS_ERROR;
        return;
    }
    irBuffer->m_header.m_msgID = MSG_ID_IR_RAW_REPEAT_COUNT;
    IRRepeatCountHeaderType *header = (IRRepeatCountHeaderType *)irBuffer;
    header->m_repeatCount = DEFAULT_IR_REPEAT_COUNT;

    MsgQueue *msgQueue = g_server.getMsgQueue();
    msgQueue->putMsg((UINT8 *)irBuffer, 0, irBuffer->m_header.m_msgDataLength + sizeof(MsgHeaderType), true);   
    
//    return (STATUS_OK);
    return;
}

void CmdHandlerInterface::repeatIRTime(const std::string& cmdStr, const uint8_t& cmdType, const std::string& cmdArg)
{
    ProntoCodeType irCode;

//    para = "0000 006a 0013 0013 0141 00a1 0014 0014 0014 0014 0014 0015 0014 0015 0014 003d 0014 003d 0014 003d 0014 003c 0014 00a1 0014 0014 0014 0014 0014 0014 0014 0014 0014 0014 0014 0015 0014 003d 0014 0014 0014 03ae 0141 00a1 0014 0014 0014 0014 0014 0014 0014 0015 0014 003d 0014 003d 0014 003d 0014 003c 0014 00a1 0014 0014 0014 0014 0014 0014 0014 0014 0014 0014 0014 0015 0014 003d 0014 0014 0014 03b8";
    logDebug("repeatIRTime: %s", cmdArg.c_str());

    // See if valid IR command is loaded before attempting to send
    if (irCode.loadIRCommand(cmdArg.c_str()) != STATUS_OK) 
    {
        logError("invalid IR data");
//        return STATUS_ERROR;
        return;
    }
    irCode.dump();
    
    MsgType *irBuffer = irCode.createIRBuffer();
    if (irBuffer == NULL)
    {
//        return STATUS_ERROR;
        return;
    }
    irBuffer->m_header.m_msgID = MSG_ID_IR_RAW_REPEAT_TIME;
    IRRepeatTimeHeaderType *header = (IRRepeatTimeHeaderType *)irBuffer;
    header->m_repeatTime = DEFAULT_IR_REPEAT_TIME;

    MsgQueue *msgQueue = g_server.getMsgQueue();
    msgQueue->putMsg((UINT8 *)irBuffer, 0, irBuffer->m_header.m_msgDataLength + sizeof(MsgHeaderType), true);   
    
//    return (STATUS_OK);
    return;
}

void CmdHandlerInterface::stopIR(const std::string& cmdStr, const uint8_t& cmdType)
{
    logDebug("-%s : %d", cmdStr.c_str(), cmdType);
    if (g_irSetting.getRepeatFlag() == FALSE)
    {
        logError("IR is not repeatable!");
//        return (STATUS_ERROR);
    }
    else
    {
        g_irSetting.setRepeatFlag(FALSE);
    }
}