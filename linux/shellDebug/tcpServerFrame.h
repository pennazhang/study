#ifndef _TCP_SERVER_FRAME_H_
#define _TCP_SERVER_FRAME_H_

#include "typedef.h"

typedef struct
{
	const char *m_lpszName;
	void * m_pAddr;
	const char *m_lpszHelp;
}FuncMapType;

extern bool startShellThread(int tcpPort);

extern int endShellThread();

void addFunctionMap(FuncMapType *pFuncMap, int nCount);


#endif


