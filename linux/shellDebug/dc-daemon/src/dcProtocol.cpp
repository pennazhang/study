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

#include "commonInterface.h"
#include <cstdio>
#include <iostream>
#include <signal.h>
#include <stdlib.h> // exit()
#include <string.h> // memset()
#include "utility.h"
#include "byteArray.h"
#include "dcProtocol.h"
#include "dcDaemonServer.h"

using namespace std;

DCProtocol::DCProtocol()
{
    m_efficientLen = 0;
}

DCProtocol::~DCProtocol()
{
}

/*---------------------------------------------------------------------------*
 * Class: UDPSocket
 *---------------------------------------------------------------------------*/
/**
 * A UDPSocket is merely a Socket that was created for IPv4 or IPv6 based
 * UDP communications.
 *
 * <!------------------------------------------------------------------------*/
//append the buffer received by the socket
void DCProtocol::appendCommand(UINT8 *buffer, int len)
{
    if (m_efficientLen + len < BUFFER_SIZE)
    {
        memcpy(m_protocolBuffer + m_efficientLen, buffer, len);
        m_efficientLen += len;
    }
    else
    {
        logFatal("DCProtocol::appendCommand overflow: efficientLen = %d, len = %d", m_efficientLen, len);
    }
}

// check the start character in command, normally it should be Alphabet or '?'
bool DCProtocol::isValidStartChar(char c)
{
    if ((c >= 'a') && (c <= 'z'))
    {
        return (true);
    }
    if ((c >= 'A') && (c <= 'Z'))
    {
        return (true);
    }
    else if (c == '?')
    {
        return (true);
    }

    return (false);
}

// check the end flag of a command, normally it should be '\r', '\n' or ';'
bool DCProtocol::isValidEndChar(char c)
{
    if ((c == '\r') || (c == '\n') || (c == ';'))
    {
        return (true);
    }
    else
    {
        return (false);
    }
}

/*---------------------------------------------------------------------------*
 * Routine: DCProtocol::getNextCommand
 *---------------------------------------------------------------------------*/
/**
 * Get a command from command buffer.
 * we need to check where is the begining of the command and where is the end of the command.
 *
 * @param [out] byteArray -- Pointer to command get from the command buffer.
 *
 * @return TRUE if we can get a command from the command buffer, or FALSE if there is no command in the command buffer.
 * <!------------------------------------------------------------------------*/
BOOL DCProtocol::getNextCommand(ByteArray &byteArray)
{
    if (m_efficientLen == 0)
    {
        return (false);
    }

    // delete the \r\n at the header
    int startIndex;
    for (startIndex = 0; startIndex < m_efficientLen; startIndex++)
    {
        if (isValidStartChar((char)m_protocolBuffer[startIndex]))
        {
            break;
        }
    }

    if (startIndex == m_efficientLen)
    {
        m_efficientLen = 0;
        return (false);
    }

    /* find the end mark: \r\n; */
    int endIndex;
    for (endIndex = startIndex + 1; endIndex < m_efficientLen; endIndex++)
    {
        if (isValidEndChar((char)m_protocolBuffer[endIndex]))
        {
            break;
        }
    }

    // the command must end with '\r', '\n', or ';' 
    // If we can't find the three character, we won't think it is the real end of a command.
    if (endIndex == m_efficientLen)
    {
        return (false);
    }

    /* copy the command to byteArray */
    int size = endIndex - startIndex;
    byteArray.resize(size + 1);
    UINT8 *data = byteArray.data();
    memcpy(data, m_protocolBuffer + startIndex, size);
    data[size] = 0;
    //    logInfo("Recv Network: %s", (char *)data);

    /* Find the next header  */
    for (; endIndex < m_efficientLen; endIndex++)
    {
        if (isValidStartChar(m_protocolBuffer[endIndex]))
        {
            break;
        }
    }

    /* Remove the command from the command buffer */
    for (int i = endIndex; i < m_efficientLen; i++)
    {
        m_protocolBuffer[i - endIndex] = m_protocolBuffer[i];
    }
    m_efficientLen -= endIndex;
    return (true);
}

typedef int (DCProtocol::*CommandDispatchFunc)(char *argument, std::string &outputInfo);

struct CommandEntry
{
    const char *m_command;
    CommandDispatchFunc m_cmdFunction;  /* Dispatch function. If not NULL, then we will call the dispatch function directly */
    CommonInterface **m_pInterface;     /* DBus CommonInterface. If (m_cmdFunction == NULL), and (m_pInterface != NULL), then we will call the DBus CommonInterface */
    const char *m_setKey;     /* DBus CommonInterface - . If (m_cmdFunction == NULL), and (m_pInterface != NULL), then we will call the DBus CommonInterface */
                                /*  If m_setKey == NULL, then we will use daemonControl function; */
                                /*  If m_setKey != NULL, then we will use set function, with key = m_setKey .*/
    const char *m_helpString;   /* Help information */
};

// - According to: https://confluence.harman.com/confluence/display/HPROVC/%5BREF%5D+NetLinx+and+Direct+Control+Command+Comparison
CommandEntry s_commandList[] = 
{

#if 0
    /* The following command is just for IR Daemon */
    {"sendir", NULL, &g_irCommonInterface, NULL, "sendir > Send IR command (example: sendir:toshibaPowerOn\n\r"},
    {"sendirraw", NULL, &g_irCommonInterface, NULL, " sendirraw > Send IR command (example: sendirraw:0000 12E4 ...)\n\r"},
    {"irrepeat", NULL, &g_irCommonInterface, NULL, " irrepeat > Send IR command continuously until irstop (example: sendirrepeat:0000 12E4 ...)\n\r"},
    {"irstop", NULL, &g_irCommonInterface, NULL, " irstop > Stop last irrepeat command\n\r"},
    {"testIR", NULL, &g_irCommonInterface, NULL, "testIR > Test the infrared interface with loopback connected. Returns 0, or -1\n\r"},

    /* The following command is just for AV Daemon */
    {"set", NULL, &g_avCommonInterface, NULL, "set > set output video source stream. set:1 \n\r"},
    {"seta", NULL, &g_avCommonInterface, NULL, "seta > set output audio source stream. seta:1 \n\r"},
    {"vmute", NULL, &g_avCommonInterface, NULL, "vmute > mute video output. vmute \n\r"},
    {"vunmute", NULL,  &g_avCommonInterface, NULL, "unmute > unmute video output. vunmute \n\r"},
    {"mute", NULL, &g_avCommonInterface, NULL, "mute > mute audio output. mute \n\r"},
    {"unmute", NULL,  &g_avCommonInterface, NULL, "unmute > unmute audio output. unmute \n\r"},
    {"txdisable", NULL, &g_avCommonInterface, NULL, "txdisable > disable audio stream. txdisable \n\r"},
    {"txenable", NULL, &g_avCommonInterface, NULL, "txenable > enable audio stream output. txenable \n\r"},
    {"lovol", NULL, &g_avCommonInterface, NULL, "lovol > linout volume (0-100%). lovol:50 \n\r"},
    {"lovolleft", NULL, &g_avCommonInterface, NULL, "lovolleft > linout left volume (0-100%). lovolleft:50 \n\r"},
    {"lovolright", NULL, &g_avCommonInterface, NULL, "lovolright > linout right volume (0-100%). lovolright:50 \n\r"},
    {"lovolup", NULL, &g_avCommonInterface, NULL, "lovol > increase lineout volume by x lovolup:x\n\r"},
    {"lovoldown", NULL, &g_avCommonInterface, NULL, "lovol > decrease lineout volume by x lovoldown:x\n\r"},
    {"live", NULL, &g_avCommonInterface, NULL, "live > Start live video. live\n\r"},
    {"cropref", NULL, &g_avCommonInterface, NULL, "crop > Set crop parameters(based on 1080p reference): cropref:xUL,yUL,xLR,yLR\n\r"},            //TX_CMD |
    {"cropref4K", NULL, &g_avCommonInterface, NULL, "crop > Set crop parameters(based on 2160p reference): cropref:xUL,yUL,xLR,yLR\n\r"},          //TX_CMD |
    {"croppreset", NULL, &g_avCommonInterface, NULL, "croppreset > Set crop parameters based on a preset configuration: croppreset:presetid\n\r"}, //TX_CMD |
    {"modeset", NULL, &g_avCommonInterface, NULL, "modeset > Set output resolution modeset:modeName (example: modeset:1080p)\n\r"},
    {"scalerdisable", NULL, &g_avCommonInterface, NULL, "scalerdisable > Disabled output scaling (example: scalerdisable\n\r"},
    {"scalerenable", NULL, &g_avCommonInterface, NULL, "scalerenable > Enable Output scaling (example: scalerenable\n\r"},
    {"rxforcecp", NULL, &g_avCommonInterface, NULL, "rxforcecp > Force HDCP activity\n\r"},
    {"txforcecp", NULL, &g_avCommonInterface, NULL, "txforcecp > Force HDCP activity\n\r"},
    {"isstreaminuse", NULL, &g_avCommonInterface, NULL, "isstreaminuse > Returns STREAMINUSE:<stream> if in use\n\r"},
    {"streamisinuse", NULL, &g_avCommonInterface, NULL, "streamisinuse > Tell encoder the stream is in use\n\r"},
    {"vidsrc", NULL, &g_avCommonInterface, NULL, "vidsrc > Select the video source of hdmivga, vgahdmi, hdmionly, or vgaonly \n\r"},
    {"aes67Audio", NULL, &g_avCommonInterface, NULL, "aes67Audio > enables/disables AES67 audio. aes67Audio:[on:off]\n\r"},
    {"aes67MCIPAddr", NULL, &g_avCommonInterface, NULL, "aes67MCIPAddr > sets the AES67 multicast address. aes67MCIPAddr:[ip]\n\r"},
    {"aes67Port", NULL, &g_avCommonInterface, NULL, "aes67Port > sets the AES67 port. aes67Port:[port]\n\r"},
    {"aes67Channels", NULL, &g_avCommonInterface, NULL, "aes67Channels > sets the number of channels in the AES67 stream. aes67Channels:[channels]\n\r"},
    {"hdmiOff", NULL, &g_avCommonInterface, NULL, "hdmiOff > turns off output HDMI \n\r"},
    {"hdmiOn", NULL, &g_avCommonInterface, NULL, "hdmiOn > turns on output HDMI \n\r"},
    {"forceRestartHDMI", NULL, &g_avCommonInterface, NULL, "forceRestartHDMI > Restarts the HDMI interface.\n\r"},

    {"serSet", NULL, &g_serialCommonInterface, NULL, "serSet > set serial port serSet:baudrate,databits,parity,stop\n\r"},
#endif    
    {"add", NULL, &g_helloCommonInterface, NULL, "add:a b > return the sum of a and b\n\r"},
    {"set", &DCProtocol::setParam, NULL, NULL, "set:variable:param > set parameter to the variable\n\r"},
    {"get", &DCProtocol::getParam, NULL, NULL, "get:variable > get the value of the variable\n\r"},

    /* The following command is just for Global Settings */
    {"help", &DCProtocol::help, NULL, NULL, "\n\rhelp > shows this help\n\r"},
    {"?", &DCProtocol::help, NULL, NULL, "? > shows this help\n\r"},


};

/*---------------------------------------------------------------------------*
 * Routine: DCProtocol::dispatchCommand
 *---------------------------------------------------------------------------*/
/**
 * This is the main process function to dispatch a direct control command
 *
 * @param [in] byteArray -- Pointer to the direct control command
 * @param [out] outputInfo -- output information when there is something wrong.
 *
 * @return Returned STATUS_OK is the command is executed successfully, or STATUS_ERROR if there is some error.
 * <!------------------------------------------------------------------------*/
int DCProtocol::dispatchCommand(ByteArray &byteArray, int& result, std::string &outputInfo)
{
    int index;
    char *commandLine = (char *)byteArray.data();
    char *command = commandLine;
    char *argument = NULL;
    for (index = 0; index < byteArray.size(); index++)
    {
        if (commandLine[index] == ':')
        {
            commandLine[index] = 0;
            argument = &commandLine[index + 1];
            break;
        }
    }

    bool commandFound = false;
    ostringstream ss;

    // We need to search the command in the s_commandList.
    for (index = 0; index < (int)sizeof(s_commandList) / (int)sizeof(CommandEntry); index++)
    {
        /*same name and same length*/
        if ((strlen(command) == strlen(s_commandList[index].m_command)) && !(strncmp(command, s_commandList[index].m_command, strlen(command))))
        {
            commandFound = true;
            break;
        }
    }

    if (commandFound == true)
    {
        // We found the command in the s_commandList.
        if (s_commandList[index].m_cmdFunction != NULL)
        {
            // If there is command dispatch function, then we just need to run the dispatch function.
            CommandDispatchFunc func = s_commandList[index].m_cmdFunction;
            result = (this->*func)(argument, outputInfo);
            return (STATUS_OK);
        }
        else if (s_commandList[index].m_pInterface != NULL)
        {
            if (s_commandList[index].m_setKey == NULL)
            {
                // If m_setKey == NULL, then we will call CommonInterface::daemonControl function.
                std::string resultString;
                Json::Value param;
                if (argument != NULL)
                {
                    param["parameter"] = argument;
                }
                else
                {
                    param["parameter"] = "";
                }
                //    cout << param.toStyledString() << endl;
                CommonInterface *pInterface = *s_commandList[index].m_pInterface;
                if (pInterface == NULL)
                {
                    outputInfo = std::string("Interface is not initialized.");
                    logError("%s", outputInfo.c_str());
                    return (STATUS_ERROR);
                }

                resultString = pInterface->daemonControl(command, param.toStyledString());
                getIntFromJsonString(resultString, "result", result);
                getStringFromJsonString(resultString, "resultInfo", outputInfo);
                logInfo("DBus: return = %d - %s", result, outputInfo.c_str());
                return (STATUS_OK);
            }
            else
            {
                // If m_setKey != NULL, then we will call CommonInterface::set function.
                std::string resultString;
                Json::Value param;
                if (argument == NULL)
                {
                    logError("Invalid set parameter: no param!");
                    return (STATUS_ERROR);
                }
                param[s_commandList[index].m_setKey] = argument;
                CommonInterface *pInterface = *s_commandList[index].m_pInterface;
                if (pInterface == NULL)
                {
                    outputInfo = std::string("Interface is not initialized.");
                    logError("%s", outputInfo.c_str());
                    return (STATUS_ERROR);
                }

                resultString = pInterface->set(param.toStyledString());
                getIntFromJsonString(resultString, "result", result);
                getStringFromJsonString(resultString, "resultInfo", outputInfo);
                return (STATUS_OK);
            }
        }
        else
        {
            // No dispatch function, no DBus CommonInterface, we don't know how to dispatch this command.
            logFatal("Can't dispatch command: %s, please re-check the dispatcher function!", command);
            outputInfo = std::string("Invalid dispatch function!");
            return (STATUS_ERROR);
        }
    }
    else
    {
        // If we can't find the command in s_commandList, then just return STATUS_ERROR.
        ss << "Can't found command: " << command << "\r\n";
        outputInfo = ss.str();
        return (STATUS_ERROR);
    }
}

// Just a test.
// parameter = "10". normally it should be [0-100].
int DCProtocol::lineOutVolume(char *parameter, std::string &outputInfo)
{
    ostringstream ss;
    ss << "lineOutVolume called with Param = " << parameter << ", Return OK!";
    outputInfo = ss.str();
    return (STATUS_OK);
}

// parameter="". we need to output the help info into outputInfo.
int DCProtocol::help(char *parameter, std::string &outputInfo)
{
    int index;
    outputInfo = std::string("\n\r*** Help ***\n\r") + std::string("*** Control ***\n\r") + std::string("General form for> set:setting1, setting2,...\n\r");
    for (index = 0; index < (int)sizeof(s_commandList) / (int)sizeof(CommandEntry); index++)
    {
        outputInfo += std::string(s_commandList[index].m_helpString);
    }

    return (STATUS_OK);
}

// parameter="". we need to output the help info into outputInfo.
int DCProtocol::updateUnit(char *parameter, std::string &outputInfo)
{
    logInfo("updateUnit: %s", parameter);
    g_pDcDaemonServer->firmwareUpgradeStart(std::string(parameter));

    return (STATUS_OK);
}

CommandEntry s_setSettingsList[] = 
{
    /* The following command is just for IR Daemon */
    {"muteFlag",  NULL,     &g_helloCommonInterface,  "muteFlag",   "set muteFlag to true or false (example: muteFlag:true)"},
    {"delayTime",  NULL,     &g_helloCommonInterface,  "delayTime",   "set delayTime in ms (example: delayTime:90)"},

    /* The following command is just for kvm Daemon */
//    {"kvmIP",  NULL,     &g_kvmCommonInterface, "kvmIPAddress", "updateUnit > updateUnit remoteIP(example: updateUnit:192.168.1.100)\n\r"},

    /* The following command is just for axmupdate */
    {"updateUnit",  &DCProtocol::updateUnit,     NULL, NULL, "updateUnit > updateUnit remoteIP(example: updateUnit:192.168.1.100)\n\r"},

};

void DCProtocol::setHelp(char *argument, std::string& outputInfo)
{
    printf("g_helloCommonInterface = %p\n", g_helloCommonInterface);
    printf("&g_helloCommonInterface = %p\n", &g_helloCommonInterface);

    for (UINT32 i = 0; i < sizeof(s_setSettingsList)/ sizeof(CommandEntry); i++)
    {
        printf("%s - 0x%p - 0x%p - %s  - %s - %s\n", s_setSettingsList[i].m_command, 
        (void *)s_setSettingsList[i].m_cmdFunction, (void *)s_setSettingsList[i].m_pInterface, s_setSettingsList[i].m_helpString, 
        s_setSettingsList[i].m_setKey, s_setSettingsList[i].m_helpString);
    }
}

int DCProtocol::setParam(char *parameter, std::string& outputInfo)
{
    char *commandLine = parameter;
    char *keyString = commandLine;
    char *valueString = NULL;
    UINT32 index;
    bool commandFound = false;
    STATUS result;

    if ((parameter == NULL) || (strlen(parameter) == 0))
    {
        outputInfo = "Invalid format for set function!\r\n Valid Usage = set:$(key):$(value).  For example: set:delayTime:30";
        return (STATUS_ERROR);
    }

    for (UINT32 index = 0; index < strlen(commandLine); index++)
    {
        if (commandLine[index] == ':')
        {
            commandLine[index] = 0;
            valueString = &commandLine[index + 1];
            break;
        }
    }

    if (valueString == NULL)
    {
        outputInfo = "Invalid format for set function!\r\n Valid Usage = set:$(key):$(value).  For example: set:delayTime:30";
        return (STATUS_ERROR);
    }

    string key = trim(string(keyString));
    string value = trim(string(valueString));
    if (key.empty() || value.empty())
    {
        outputInfo = "Invalid format for set function!\r\n Valid Usage = set:$(key):$(value).  For example: set:delayTime:30";
        return (STATUS_ERROR);
    }

    // We need to search the command in the s_setSettingsList.
    for (index = 0; index < sizeof(s_setSettingsList) / sizeof(CommandEntry); index++)
    {
        /*same name and same length*/
        if (key == std::string(s_setSettingsList[index].m_command))
        {
            commandFound = true;
            break;
        }
    }

    if (commandFound == true)
    {
        // We found the command in the s_setSettingsList.
        if (s_setSettingsList[index].m_cmdFunction != NULL)
        {
            // If there is command dispatch function, then we just need to run the dispatch function.
            CommandDispatchFunc func = s_setSettingsList[index].m_cmdFunction;
            result = (this->*func)((char *)value.c_str(), outputInfo);
            return (result);
        }

        else if (s_setSettingsList[index].m_pInterface != NULL)
        {
            if (s_setSettingsList[index].m_setKey == NULL)
            {
                outputInfo = std::string("setParam called with: ") + key + std::string(" Failed: - Invalid interface parameter.");
                return (STATUS_ERROR);
            }
            else
            {
                // If m_setKey != NULL, then we will call CommonInterface::set function.
                std::string resultString;
                Json::Value param;
                param[s_setSettingsList[index].m_setKey] = value;
                printf("index = %d, m_pInerface = %p\n", index, (void *)s_commandList[index].m_pInterface);
                printf("index = %d, *m_pInerface = %p\n", index, (void*)*(s_commandList[index].m_pInterface));

                CommonInterface *pInterface = *(s_commandList[index].m_pInterface);
                resultString = pInterface->set(param.toStyledString());
                getIntFromJsonString(resultString, "result", result);
                getStringFromJsonString(resultString, "resultInfo", outputInfo);

//                logError("result = %d, resultInfo = %s", result, outputInfo.c_str());
                return (result);
            }
        }
        else
        {
            ostringstream ss;
            ss << "setParam called with: " << key << " Failed: - Invalid interface.";
            outputInfo = ss.str();
            return (STATUS_ERROR);
        }
    }

    ostringstream ss;
    ss << "setParam called with: " << key << " Failed: No key matched!";
    setHelp((char *)"", outputInfo);
    outputInfo = ss.str();

    return (STATUS_ERROR);
}

int DCProtocol::getParam(char *parameter, std::string& outputInfo)
{
//    UINT32 index;
//    bool commandFound = false;
//    STATUS result;

    if ((parameter == NULL) || (strlen(parameter) == 0))
    {
        outputInfo = "Invalid format for get function!\r\n Valid Usage = get:$(key).  For example: get:delayTime";
        return (STATUS_ERROR);
    }



    return (STATUS_OK);
}
