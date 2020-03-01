#ifndef _TCP_SERVER_FRAME_H_
#define _TCP_SERVER_FRAME_H_

extern bool startShellThread(int tcpPort);

extern int endShellThread();

typedef struct
{
	const char *m_lpszName;
	void * m_pAddr;
	const char *m_lpszHelp;
}FUNC_MAP;

void addFunctionMap(FUNC_MAP *pFuncMap, int nCount);


#endif


