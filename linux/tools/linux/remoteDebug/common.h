// Common.h: interface for the CCommon class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _COMMON_H_
#define _COMMON_H_

typedef unsigned char 	UINT8;
typedef unsigned short 	UINT16;
typedef unsigned int	UINT32;
typedef unsigned int	UINT;
typedef bool			BOOL;
typedef unsigned long	SYSWORD;	// sizeof(SYSWORD) = sizeof(void *)

#define TRUE	true
#define FALSE	false

#define STATUS 			int
#define	STATUS_OK		0
#define STATUS_ERROR	-1

	
char * FormatString(UINT8 * pucMsg, int len);

int ReadFromString(char *lpszBuffer, UINT8 *pucData, int nLen);

void mSleep(int nMs);

void taskDelay(int nMs);

UINT sysClkRateGet(void);

bool isFdReadable(int fd);

#endif

