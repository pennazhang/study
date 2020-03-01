#ifndef _SHELL_FRAME_H_
#define _SHELL_FRAME_H_

#include "typedef.h"
#include "shellFrame.h"
#include "tcpServerFrame.h"

#define MAX_SYMBOL_COUNT 20
#define MAX_SYMBOL_LENGTH 60

extern long d0, d1, d2, d3, d4, d5, d6, d7, d8, d9;
extern void *p0, *p1, *p2, *p3, *p4, *p5, *p6, *p7, *p8, *p9;

typedef SYSWORD (*pFuncPtr)(SYSWORD ulData1, SYSWORD ulData2, SYSWORD ulData3, SYSWORD ulData4, SYSWORD ulData5, SYSWORD ulData6, SYSWORD ulData7, SYSWORD ulData8, SYSWORD ulData9, SYSWORD ulData10);

typedef enum
{
	SYMBOL_FUNCTION = 0,	// void Key();
	SYMBOL_CONTENT = 1,		// *Key = 
	SYMBOL_ADDRESS = 2,		// &Key = 
	SYMBOL_VALUE = 3,		// Key = 
	SYMBOL_STRING = 4,		// "Key"
	SYMBOL_NUMBER = 5,		// 0x
	SYMBOL_OPERATION = 6,		// ¶¨ÒåÎª + - * /
}SYMBOL_TYPE;


typedef struct
{
	pFuncPtr m_pFunc;
	SYSWORD m_nData1;
	SYSWORD m_nData2;
	SYSWORD m_nData3;
	SYSWORD m_nData4;
	SYSWORD m_nData5;
	SYSWORD m_nData6;
	SYSWORD m_nData7;
	SYSWORD m_nData8;
	SYSWORD m_nData9;
	SYSWORD m_nData10;
}THREAD_DATA;

typedef struct
{
	char m_lpszName[40];
	void * m_pAddr;
}DATA_MAP;

void help(char *lpszFunctionName);

SYSWORD ShowRegArray(char *lpszArray);
void ShowRegData(void);
void ShowRegFunc(void);

void CmdLoop();

int sysRun(char *lpszCmd);



void lkup(char *lpszFuncName);
//pthread_t sp(pFuncPtr pFunc, SYSWORD nData1, SYSWORD nData2, SYSWORD nData3, SYSWORD nData4, SYSWORD nData5, SYSWORD nData6, SYSWORD nData7, SYSWORD nData8, SYSWORD nData9, SYSWORD nData10);


STATUS explainCmd(char *lpszBuffer, SYSWORD * pulResult);
STATUS ExplainSignleCmd(char *lpszBuffer, SYSWORD * pulResult);

STATUS ExecuteScript(char *lpszFileName);
int ReadSymbolTable(char *lpszCommand, char lpszSymbol[MAX_SYMBOL_COUNT][MAX_SYMBOL_LENGTH]);


STATUS RunCommand(UINT ulCommandID, char *lpszCommandName, SYSWORD ulParam1, SYSWORD ulParam2,
            SYSWORD ulParam3, SYSWORD ulParam4, SYSWORD ulParam5, SYSWORD ulParam6, SYSWORD ulParam7, 
            SYSWORD ulParam8, SYSWORD ulParam9, SYSWORD ulParam10);

void SetUINT(UINT *pulAddr, UINT ulData);

void SetUINT8(UINT8 *pucAddr, UINT8 ucData);

UINT GetUINT(UINT *pulAddr);


UINT8 GetUINT8(UINT8 *pucAddr);

void version(void);

extern bool startShellThread(int tcpPort);

extern int endShellThread();

void addBasicFuncMap();


#endif


