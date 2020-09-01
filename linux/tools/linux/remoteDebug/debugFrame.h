// debugFrame.h: 
//
//////////////////////////////////////////////////////////////////////

#ifndef _DEBUG_FRAME_H_
#define _DEBUG_FRAME_H_

#include "common.h"

extern STATUS debugInit(int tcpPort);
extern void debugExit(void);
extern STATUS ExplainCmd(const char *lpszCmd, SYSWORD *result);

typedef SYSWORD (*pFuncPtr)(SYSWORD p1, SYSWORD p2, SYSWORD p3, SYSWORD p4, SYSWORD p5, SYSWORD p6, SYSWORD p7, SYSWORD p8, SYSWORD p9, SYSWORD p10);

typedef enum
{
	SYMBOL_FUNCTION = 0,	// void Key();
	SYMBOL_CONTENT = 1,		// *Key = 
	SYMBOL_ADDRESS = 2,		// &Key = 
	SYMBOL_VALUE = 3,		// Key = 
	SYMBOL_STRING = 4,		// "Key"
	SYMBOL_NUMBER = 5,		// 0x
	SYMBOL_OPERATION = 6,	// such as operator:  + - * /
} SYMBOL_TYPE;

typedef struct
{
	const char *m_lpszName;
	void * m_pAddr;
	const char *m_lpszHelp;
}FUNC_MAP;

typedef struct
{
	pFuncPtr m_pFunc;
	int m_nData1;
	int m_nData2;
	int m_nData3;
	int m_nData4;
	int m_nData5;
	int m_nData6;
	int m_nData7;
	int m_nData8;
	int m_nData9;
	int m_nData10;
}THREAD_DATA;

typedef struct
{
	const char * m_lpszName;
	void * m_pAddr;
}DATA_MAP;

#endif

