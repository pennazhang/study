#ifndef _FUNC_LIB_H_
#define _FUNC_LIB_H_

#include "common.h"

//#define LITTLE_ENDIAN_MODE	0
//#define BIG_ENDIAN_MODE		1

//int GetEndianMode(void);

//UINT16 crc(UINT8 *ptr,  int nLen);
//void * memchk(UINT8 *pucByte, UINT8 ucData, UINT32 ulLength);
void d(void *pAddr, int nByteCount, int nWidth, int nDisplayOffset);
int readHexString(UINT8 *pucData, char *lpszBuffer, int nMaxLen);
//void dm(void *pAddr, int nByteCount, int nWidth);

//int GetFileLength(char *lpszFileName);
//VOS_STATUS CopyMemoryToFile(UINT8 *pucByte, int nCount, char *lpszFileName);
//VOS_STATUS AppendMemoryToFile(UINT8 *pucData, int nCount, char *lpszFileName);
//VOS_STATUS CopyFileToMemory(UINT8 *pucByte, int nCount, char *lpszFileName, int nFilePos);
//VOS_STATUS cp(char *lpszOldFileName, char *lpszDstFileName, int nCopyUnit, int nStart, int nEnd);
UINT16 crc(UINT8 *ptr,  int nLen);
unsigned long getTickCount();
//void repeat(int nTotalCount, int nBurstCount, int nBurstPeriodInMS, SYSWORD pFunc, SYSWORD nParam1, SYSWORD nParam2, SYSWORD nParam3, SYSWORD nParam4, SYSWORD nParam5, SYSWORD nParam6);
//void ShowThreadPriority(void);
STATUS runScript(char *fileName);

#endif


