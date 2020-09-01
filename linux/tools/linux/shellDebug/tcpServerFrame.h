#ifndef _TCP_SERVER_FRAME_H_
#define _TCP_SERVER_FRAME_H_
#include <cstddef>

extern bool startShellThread(int tcpPort, const char *prompt = NULL, const char *userName = NULL, const char *password = NULL);

extern int endShellThread();

typedef struct
{
	const char *m_lpszName;
	void * m_pAddr;
	const char *m_lpszHelp;
}FUNC_MAP;

void addFunctionMap(FUNC_MAP *pFuncMap, int nCount);


#endif


