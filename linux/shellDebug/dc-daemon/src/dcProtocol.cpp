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
#include "dcUtility.h"

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
CommandEntry s_commandList[] = {

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
    {"dviOff", NULL, NULL, NULL, "dviOff > turns off output DVI \n\r"},
    {"dviOn", NULL, NULL, NULL, "dviOn > turns on output DVI \n\r"},
    {"veron", NULL, NULL, NULL, "veron > verbose mode on\n\r"},
    {"veroff", NULL, NULL, NULL, "veroff > verbose mode off\n\r"},
    {"rxdisable", NULL, NULL, NULL, "rxdisable > disable RX\n\r"},
    {"rxenable", NULL, NULL, NULL, "rxenable > enable RX\n\r"},
    {"setSettings", &DCProtocol::setSettings, NULL, NULL, "\n\r"},
    {"local", NULL, NULL, NULL, "local > start local play list 0-7. local:4\n\r"},
    {"convertimg", NULL, NULL, NULL, "convertimg > covert an image file to proper format convertimg:file.jpg\n\r"},
    {"deleteimg", NULL, NULL, NULL, "deleteimg > delete an image file from unit deleteimg:filename\n\r"},
    {"processlplist", NULL, NULL, NULL, "processlplist > process a lp list uploaded to unit processlplist:filename\n\r"},
    {"processlpaudiolist", NULL, NULL, NULL, "processlpaudiolist > process a lp audio list uploaded to unit processlpaudiolist:filename\n\r"},
    {"setjpegoptions", NULL, NULL, NULL, "setjpegoptions > set JPEG covert options setjpegoptions:targetWidth,targetHeight,maintainAspectFlag,fillColorRed,fillColorGreen,fillColorBlue\n\r"},
    {"fetchLPlist", NULL, NULL, NULL, "fetchLPlist > fetch and process a lp list fetchLPlist:filename\n\r"},
    {"fetchLPimages", NULL, NULL, NULL, "fetchLPimages > fetch a list of images to download to DB fetchLPimages:filename\n\r"},
    {"fetchLPAudio", NULL, NULL, NULL, "fetchLPAudio > fetch an audio file fetchLPAudio:filename\n\r"},
    {"resetData", NULL, NULL, NULL, "resetData > Reset data object groups\r"},
    {"idOn", NULL, NULL, NULL, "idOn > Turn on ID mode for a short period of time\r"},
    {"setbrightness", NULL, NULL, NULL, "setbrightness > set RGB brightness setbrightness:red,green,blue\n\r"},
    {"deleteaudio", NULL, NULL, NULL, "deleteaudio > delete an audio file from unit deleteaudio:filename\n\r"},
    {"fetchLPAudiolist", NULL, NULL, NULL, "fetchLPAudiolist > fetch and process an audio list fetchLPAudioListCommand:filename\n\r"},
    {"ndreset", NULL, NULL, NULL, "ndreset > Network diagnostics reset\n\r"},
    {"getNetStatus", NULL, NULL, NULL, "getNetStatus > Network diagnostics output\n\r"},
    {"cmdhistreset", NULL, NULL, NULL, "cmdhistreset > Reset the command history\n\r"},
    {"resetport", NULL, NULL, NULL, "resetport:<type>,<port number> > Reset a port of type TCP or ALL\n\r"},
    {"sendser", NULL, NULL, NULL, "sendser > Send serial command (example: sendser:PowerOn\n\r"},
    {"showinfo", NULL, NULL, NULL, "showinfo:<on|off> > Show the info page\n\r"},
    {"showinfomsg", NULL, NULL, NULL, "showinfomsg:<msg number> > Show the info page's message by number\n\r"},
    {"kvmfetch", NULL, NULL, NULL, "kvmfetch:<url> > Fetch the KVM show info page .png from the given url\n\r"},
    {"kvmimport", NULL, NULL, NULL, "kvmimport:<urlOrFile> > Import a KVM csv configuration file from the given url or file\n\r"},
    {"kvmgenimg", NULL, NULL, NULL, "kvmgenimg > Regenerate the standard KVM image as designed by the webpage\n\r"},
    {"encryptPassword", NULL, NULL, NULL, "encryptPassword:<password> > Sets the encryption password when encryption is on\n\r"},
    {"resetkvmconfig", NULL, NULL, NULL, "kvmresetconfig > Resets KVM functionality to factory state\n\r"},
    {"encryptedStream", NULL, NULL, NULL, "encryptedStream:<on/off> > Enables encrypting of the video stream\n\r"},
    {"forceHTTPS", NULL, NULL, NULL, "forceHTTPS:<on/off> > Force to require HTTPS on web pages\n\r"},
    {"settingslock", NULL, NULL, NULL, "settingslock:<on/off> > Enables locking of settings from being changed.\n\r"},
    {"forceRestartDVI", NULL, NULL, NULL, "forceRestartDVI > Restarts the DVI interface.\n\r"},
    {"KVMMasterIP", NULL, NULL, NULL, "KVMMasterIP > Connects KVM to given IP address\n\r"},
    {"testusb", NULL, NULL, NULL, "testusb > Test the existence of the usb loopback on ports.  Returns 0, 1, or 2\n\r"},
    {"readsyslog", NULL, NULL, NULL, "readsyslog > Read enable syslog.\n\r"},
    {"resetsyslog", NULL, NULL, NULL, "resetsyslog > Reset syslog.\n\r"},
    {"lpnext", NULL, NULL, NULL, "lpnext > Immediately cycle to the next local/host play image.\n\r"},
    {"debugLog", NULL, NULL, NULL, "debugLog > Prepare a debug log package.\n\r"},
    {"analogTableChanged", NULL, NULL, NULL, "analogTableChanged > The analog mode table has changed.  Reload and restart DVI.\n\r"},
    {"switchStatsUpdate", NULL, NULL, NULL, "switchStatsUpdate > Request an update of the switch stats\n\r"},
    {"switchStatsReset", NULL, NULL, NULL, "switchStatsReset > Request a reset of the switch stats\n\r"},
    {"abortimgconvert", NULL, NULL, NULL, "abortimgconvert > Abort any active image conversions\n\r"},
    {"eventsChanged", NULL, NULL, NULL, "eventsChanged > N-Act events file has changed, reload settings\n\r"},
    {"regsdump", NULL, NULL, NULL, "eventsChanged > Internal command to dump register\n\r"},
    {"securePortsOnly", NULL, NULL, NULL, "securePortsOnly > enables/disables clear command ports. securePortsOnly:[on:off]\n\r"},
    {"commandPassword", NULL, NULL, NULL, "commandPassword > Sets secure command password. commandPassword:password\n\r"},
    {"regenCert", NULL, NULL, NULL, "regenCert > Regenerate SSL certificates\n\r"},
    {"manifest", NULL, NULL, NULL, "manifest > Create a migration manifest document\n\r"},
    {"mget", NULL, NULL, NULL, "mget > Get a file that's in the current manifest\n\r"},
    {"mput", NULL, NULL, NULL, "mput > Put a file that's in the current manifest onto the drive\n\r"},


    {"webPageEnable", NULL, NULL, NULL, "webPageEnable > enable the web page (secure socket command only)\n\r"},
    {"webPageDisable", NULL, NULL, NULL, "webPageDisable > disable the page (secure socket command only)\n\r"},

    {"xml", NULL, NULL, NULL, "xml:<type> > Return the xml of the given type \n\r"},
    {"chg_admin_pwd", NULL, NULL, NULL, "chg_admin_pwd:<pwd> > Set a new admin password.  <pwd> is base64 encoded password\n\r"},

    /* The following command is just for Global Settings */
    {"getStatus", &DCProtocol::getStatus, NULL, NULL, "getStatus returns status string.\n\r"},
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
int DCProtocol::dispatchCommand(ByteArray &byteArray, std::string &outputInfo)
{
    int index, result;
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
//                logInfo("DBus: return = %d - %s", result, outputInfo.c_str());
                return (result);
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
//                logInfo("DBus: return = %d - %s", command, result, outputInfo.c_str());
                return (result);
            }
        }
        else
        {
            // No dispatch function, no DBus CommonInterface, we don't know how to dispatch this command.
            logFatal("Can't dispatch command: %s, please re-check the dispatcher function!", command);
            outputInfo = std::string("Invalid dispatch function!");
            result = STATUS_ERROR;
        }
        return (result);
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
    {"ircmdtime",  NULL,     &g_irCommonInterface,  "irCommandHoldoff",   "set time for an IR to hold off between commands in ms(example: ircmdtime:90)\n\r"},
    {"irc2rtime",  NULL,     &g_irCommonInterface,  "irRepeatHoldoff",   "set time for an IR to hold off between command and repeat(example: irc2rtime:25)\n\r"},

    /* The following command is just for kvm Daemon */
    {"kvmIP",  NULL,     &g_kvmCommonInterface, "kvmIPAddress", "updateUnit > updateUnit remoteIP(example: updateUnit:192.168.1.100)\n\r"},

    /* The following command is just for axmupdate */
    {"updateUnit",  &DCProtocol::updateUnit,     NULL, NULL, "updateUnit > updateUnit remoteIP(example: updateUnit:192.168.1.100)\n\r"},

};

int DCProtocol::setSettings(char *parameter, std::string& outputInfo)
{
    char *commandLine = parameter;
    char *command = commandLine;
    char *argument = NULL;
    UINT32 index;
    bool commandFound = false;
    STATUS result;

//    logInfo("setSettings called here, parameter = %s", parameter);
    for (UINT32 index = 0; index < strlen(commandLine); index++)
    {
        if (commandLine[index] == ':')
        {
            commandLine[index] = 0;
            argument = &commandLine[index + 1];
            break;
        }
    }
    logInfo("setSettings called with command = %s, param = %s", command, argument);

    // We need to search the command in the s_setSettingsList.
    for (index = 0; index < sizeof(s_setSettingsList) / sizeof(CommandEntry); index++)
    {
        /*same name and same length*/
        if ((strlen(command) == strlen(s_setSettingsList[index].m_command)) && !(strncmp(command, s_setSettingsList[index].m_command, strlen(command))))
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
            result = (this->*func)(argument, outputInfo);
            return (result);
        }

        else if (s_setSettingsList[index].m_pInterface != NULL)
        {
            if (s_setSettingsList[index].m_setKey == NULL)
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
//                logInfo("DBus: return = %d - %s", command, result, outputInfo.c_str());
                return (result);
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
                param[s_setSettingsList[index].m_setKey] = argument;
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
//                logInfo("DBus: return = %d - %s", command, result, outputInfo.c_str());
                return (result);

            }
        }
        else
        {
            ostringstream ss;
            ss << "setSettings called with: " << parameter << ", Return Failed - No dispatch function for this command!";
            outputInfo = ss.str();
            return (STATUS_ERROR);
        }

    }

    ostringstream ss;
    ss << "setSettings called with: " << parameter << ", Return Failed!";
    outputInfo = ss.str();

    return (STATUS_ERROR);
}

int DCProtocol::getStatus(char *argument, std::string& outputInfo)
{
    DCSettings::getStatus(outputInfo);
//    logInfo("DCProtocol::getStatus = %s", outputInfo.c_str());
    return (STATUS_OK);
}

