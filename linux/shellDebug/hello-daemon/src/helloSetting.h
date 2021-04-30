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
#include <iostream>
#include <chrono>
#include <unistd.h>
#include "utility.h"



int setIRCommandID(const std::string& cmdArg, std::string& outputInfo);

int setIRCommands(const std::string& cmdArg, std::string& outputInfo);

// cmdArg = "25"
int setRepeatHoldOff(const std::string& cmdArg, std::string& outputInfo);

// cmdArg = "25"
int setCommandHoldOff(const std::string& cmdArg, std::string& outputInfo);

// cmdArg = "0000 006a 0013 0013 0141 00a1 0014 0014 0014 0014 0014 0015 0014 0015 0014 003d 0014 003d 0014 003d 0014 003c 0014 00a1 0014 0014 0014 0014 0014 0014 0014 0014 0014 0014 0014 0015 0014 003d 0014 0014 0014 03ae 0141 00a1 0014 0014 0014 0014 0014 0014 0014 0015 0014 003d 0014 003d 0014 003d 0014 003c 0014 00a1 0014 0014 0014 0014 0014 0014 0014 0014 0014 0014 0014 0015 0014 003d 0014 0014 0014 03b8";
int sendIRRaw(const std::string& cmdArg, std::string& outputInfo);

// cmdArg = "toshibaPowerOn"
int sendIR(const std::string& cmdArg, std::string& outputInfo);

// cmdArg = ""
int testIR(const std::string& cmdArg, std::string& outputInfo);

// cmdArg = "0000 006a 0013 0013 0141 00a1 0014 0014 0014 0014 0014 0015 0014 0015 0014 003d 0014 003d 0014 003d 0014 003c 0014 00a1 0014 0014 0014 0014 0014 0014 0014 0014 0014 0014 0014 0015 0014 003d 0014 0014 0014 03ae 0141 00a1 0014 0014 0014 0014 0014 0014 0014 0015 0014 003d 0014 003d 0014 003d 0014 003c 0014 00a1 0014 0014 0014 0014 0014 0014 0014 0014 0014 0014 0014 0015 0014 003d 0014 0014 0014 03b8";
// repeat the ProntoCode in cmdArg until stopIR is called or timeout (timeout = 5seconds).
int repeatIR(const std::string& cmdArg, std::string& outputInfo);

// cmdArg = "", just set g_irSetting.m_repeatFlag to FALSE.
int stopIR(const std::string& cmdArg, std::string& outputInfo);

//- set time for an IR to hold off between commands in ms
int setIRCmdTime(const std::string& cmdArg, std::string& outputInfo);

//- set time for an IR to hold off between command and repeat.
int setIRRepeatTime(const std::string& cmdArg, std::string& outputInfo);

// cmdArg = "0" or "1"
int enablePassThrough(const std::string& cmdArg, std::string& outputInfo);

// cmdArg = "192.168.1.110"
int setPeerIPAddress(const std::string& cmdArg, std::string& outputInfo);
