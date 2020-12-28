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

#ifndef _SHELL_FRAME_H_
#define _SHELL_FRAME_H_

#include "typeDef.h"

#include "fsl_debug_console.h"

#define MAX_SYMBOL_COUNT 		10
#define MAX_SYMBOL_LENGTH 	60
#define MAX_COMMAND_LEN 		120

#define Diag_Output	PRINTF

extern long d0, d1, d2, d3, d4, d5, d6, d7, d8, d9;
extern void *p0, *p1, *p2, *p3, *p4, *p5, *p6, *p7, *p8, *p9;

#define MAX_PARAM_COUNT		8
typedef SYSWORD (*pFuncPtr)(SYSWORD ulData1, SYSWORD ulData2, SYSWORD ulData3, SYSWORD ulData4, SYSWORD ulData5, SYSWORD ulData6, SYSWORD ulData7, SYSWORD ulData8);

typedef enum
{
	SYMBOL_FUNCTION = 0,	// void Key();
	SYMBOL_CONTENT = 1,		// *Key = 
	SYMBOL_ADDRESS = 2,		// &Key = 
	SYMBOL_VALUE = 3,		// Key = 
	SYMBOL_STRING = 4,		// "Key"
	SYMBOL_NUMBER = 5,		// 0x
	SYMBOL_OPERATION = 6,		// a + b
}SymbolType;


typedef struct
{
	const char *m_lpszName;
	void * m_pAddr;
}DataMapType;

typedef struct
{
	const char *m_lpszName;
	void * m_pAddr;
	const char *m_lpszHelp;
}FuncMapType;


void help(char *lpszFunctionName);

SYSWORD ShowRegArray(char *lpszArray);
void ShowRegData(void);
void ShowRegFunc(void);

void CmdLoop();

int sysRun(char *lpszCmd);



void lkup(char *lpszFuncName);


STATUS explainCmd(char *lpszBuffer, SYSWORD * pulResult);
STATUS ExplainSignleCmd(char *lpszBuffer, SYSWORD * pulResult);

STATUS ExecuteScript(char *lpszFileName);
int ReadSymbolTable(char *lpszCommand, char *lpszSymbol[MAX_SYMBOL_COUNT]);


STATUS RunCommand(UINT ulCommandID, char *lpszCommandName, SYSWORD ulParam1, SYSWORD ulParam2,
            SYSWORD ulParam3, SYSWORD ulParam4, SYSWORD ulParam5, SYSWORD ulParam6, SYSWORD ulParam7, 
            SYSWORD ulParam8, SYSWORD ulParam9, SYSWORD ulParam10);

void set32(UINT32 *pulAddr, UINT32 ulData);
void set16(UINT16 *pulAddr, UINT16 ulData);
void set8(UINT8 *pucAddr, UINT8 ucData);

UINT32 get32(UINT32 *pulAddr);
UINT16 get16(UINT16 *pusAddr);
UINT8 get8(UINT8 *pucAddr);

void version(void);

extern BOOL startShellThread(int tcpPort);

extern int endShellThread();

void addBasicFuncMap();

void addFunctionMap(FuncMapType *pFuncMap, int nCount);


#endif


