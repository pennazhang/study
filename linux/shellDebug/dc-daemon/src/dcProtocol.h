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

#include "typedef.h"
#include "utility.h"
#include "byteArray.h"

/*---------------------------------------------------------------------------*
 * Class: DCProtocol
 *---------------------------------------------------------------------------*/
/**
 * A DCProtocol is only used to dispatch Direct Control Protocol.
 * The format of direct Control command is :  Command [:parameter]
 * For example:
 *              sendir:toshibaPowerOn
 *              testIR
 *
 * A command should start with [a:z | A:Z] and end with [\r|\n|;]. This is very important!!!
 * 
 * <!------------------------------------------------------------------------*/
class DCProtocol
{
public:
    DCProtocol();
    ~DCProtocol();

    // append the commands received from socket to protocol buffer.
    void appendCommand(UINT8 *buffer, int len);

    // get a command from protocol buffer.
    BOOL getNextCommand(ByteArray& byteArray);

    // dispatch a command read from protocol buffer.
    int dispatchCommand(ByteArray& byteArray, int& result, std::string& outputInfo);

    // to judge the start character of a direct control command.
    bool isValidStartChar(char c);

    // to judge a end character of a direct control command.
    bool isValidEndChar(char c);

    // Protocol dispatch function:
    //- argument: linout volume (0-100)
    int lineOutVolume(char *argument, std::string& outputInfo);

    //- argument: None
    int help(char *argument, std::string& outputInfo);

    //- Update firmware. 
    int updateUnit(char *parameter, std::string &outputInfo);

    int setParam(char *argument, std::string& outputInfo);

    int getParam(char *argument, std::string& outputInfo);
    
    void setHelp(char *argument, std::string& outputInfo);

protected:
    // used to store all commands.
    UINT8 m_protocolBuffer[BUFFER_SIZE];
    int m_efficientLen;    
};

