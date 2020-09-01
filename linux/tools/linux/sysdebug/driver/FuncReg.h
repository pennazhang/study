#ifndef _FUNC_REG_H_
#define _FUNC_REG_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "diag.h"
/*
#include "diag_function.h"
*/

#define MAX_SYMBOL_COUNT 20
#define MAX_SYMBOL_LENGTH 60

extern int d0, d1, d2, d3, d4, d5, d6, d7, d8, d9;
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
	char *m_lpszName;
	void * m_pAddr;
	char *m_lpszHelp;
}FUNC_MAP;

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


#if 0
void CmdLoop();
#endif
void lkup(char *lpszFuncName);
#if 0
pthread_t sp(pFuncPtr pFunc, int nData1, int nData2, int nData3, int nData4, int nData5, int nData6, int nData7, int nData8, int nData9, int nData10);
#endif

VOS_STATUS ExplainCmd(char *lpszBuffer, SYSWORD * pulResult);
VOS_STATUS ExecuteScript(char *lpszFileName);
int ReadSymbolTable(char *lpszCommand, char lpszSymbol[MAX_SYMBOL_COUNT][MAX_SYMBOL_LENGTH]);


VOS_STATUS RunCommand(UINT32 ulCommandID, char *lpszCommandName, SYSWORD ulParam1, SYSWORD ulParam2,
            SYSWORD ulParam3, SYSWORD ulParam4, SYSWORD ulParam5, SYSWORD ulParam6, SYSWORD ulParam7, 
            SYSWORD ulParam8, SYSWORD ulParam9, SYSWORD ulParam10);

void SetUINT32(UINT32 *pulAddr, UINT32 ulData);

void SetUINT16(UINT16 *pusAddr, UINT16 usData);

void SetUINT8(UINT16 *pucAddr, UINT8 ucData);

UINT32 GetUINT32(UINT32 *pulAddr);

UINT16 GetUINT16(UINT16 *pusAddr);

UINT8 GetUINT8(UINT8 *pucAddr);

void version(void);

void *malloc(SYSWORD nSize);
    
void free(void *pMemory);

void *Memcpy(void *dest, const void *src, unsigned int n);


#ifdef __cplusplus
}
#endif

#endif


