//####
//# Legal Notice :
//#    Copyright, AMX LLC, 2009
//#    Private, proprietary information, the sole property of AMX LLC.  The
//#    contents, ideas, and concepts expressed herein are not to be disclosed
//#    except within the confines of a confidential relationship and only
//#    then on a need to know basis.
//#
//#    Any entity in possession of this AMX Software shall not, and shall not
//#    permit any other person to, disclose, display, loan, publish, transfer
//#    (whether by sale, assignment, exchange, gift, operation of law or
//#    otherwise), license, sublicense, copy, or otherwise disseminate this
//#    AMX Software.
//#
//#    This AMX Software is owned by AMX and is protected by United States
//#    copyright laws, patent laws, international treaty provisions, and/or
//#    state of Texas trade secret laws.
//#
//#    Portions of this AMX Software may, from time to time, include
//#    pre-release code and such code may not be at the level of performance,
//#    compatibility and functionality of the final code. The pre-release code
//#    may not operate correctly and may be substantially modified prior to
//#    final release or certain features may not be generally released. AMX is
//#    not obligated to make or support any pre-release code. All pre-release
//#    code is provided "as is" with no warranties.
//#
//#    This AMX Software is provided with restricted rights. Use, duplication,
//#    or disclosure by the Government is subject to restrictions as set forth
//#    in subparagraph (1)(ii) of The Rights in Technical Data and Computer
//#    Software clause at DFARS 252.227-7013 or subparagraphs (1) and (2) of
//#    the Commercial Computer Software Restricted Rights at 48 CFR 52.227-19,
//#    as applicable.
//####

#include "userCommand.h"
#include <math.h>
#include <stdarg.h>
#include <stdlib.h>
#include "fsl_str.h"
#include "typeDef.h"
#include "shellFrame.h"
#include "commonLib.h"
#include "fsl_debug_console.h"


bool isDigit(int c)
{
	if (((char)c >= '0') && ((char)c <= '9'))
	{
		return (true);
	}
	else
	{
		return (false);
	}
}

bool isLower(int c)
{
	if (((char)c >= 'a') && ((char)c <= 'z'))
	{
		return (true);
	}
	else
	{
		return (false);
	}
}

bool isUpper(int c)
{
	if (((char)c >= 'A') && ((char)c <= 'Z'))
	{
		return (true);
	}
	else
	{
		return (false);
	}
}

bool isValidChar(int c)
{
	const char *validChars = "=*&()-+_\",. ;";
	if (isDigit(c) || isLower(c) || isUpper(c))
	{
		return (true);
	}
	if (strchr(validChars, c) != NULL)
	{
		return (true);
	}
	
	return (false);
}


int readLine(char *lpszBuffer, int nMaxLen)
{
	int inputChar;
	int index = 0;
	for (;;)
	{
		inputChar = GETCHAR();
		if (isValidChar(inputChar))
		{
			if (index == nMaxLen -1)
			{
				continue;
			}
			
			lpszBuffer[index] = inputChar;
			index++;
			PUTCHAR(inputChar);
		}
		else if ((inputChar == '\r') || (inputChar == '\n'))
		{
			lpszBuffer[index] = 0;
			PUTCHAR('\r');
			PUTCHAR('\n');
			return (index);
		}
		else if (inputChar == '\b')
		{
			if (index >= 1)
			{
				PUTCHAR('\b');
				PUTCHAR(' ');
				PUTCHAR('\b');
				index--;
			}
		}
	}
}

void userCommandTask(void *pvParameters)
{
	char cmdLine[MAX_COMMAND_LEN];
	SYSWORD ulResult;
	
//	char inputChar;
	addBasicFuncMap();
	PRINTF("Hello This is my first program.\r\n");
	
    for (;;)
    {
		PRINTF("\r\n-> ");

    	readLine(cmdLine, MAX_COMMAND_LEN);

		/* Explain the cmd */
		if (explainCmd(cmdLine, &ulResult) == STATUS_OK)
		{
//			Diag_Output("Result *(0x%x) = 0x%x\r\n", (UINT32)&ulResult, (UINT32)ulResult);
			PRINTF("\r\nExecute function successfully, Result = 0x%X = %d\r\n", (UINT32)ulResult, (signed int)ulResult);
		}
		else
		{
			PRINTF("\r\nExecute command failed for: %s\r\n", cmdLine);
		}
    }
}

