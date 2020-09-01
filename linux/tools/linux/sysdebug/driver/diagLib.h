#ifndef _DIAG_LIB_H_
#define _DIAG_LIB_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "diag.h"

UINT16 crc(UINT8 *ptr,  int nLen);
void * memchk(UINT8 *pucByte, UINT8 ucData, UINT32 ulLength);
void d(void *pAddr, int nByteCount, int nWidth);
#if 0
int GetFileLength(char *lpszFileName);
VOS_STATUS CopyMemoryToFile(UINT8 *pucByte, int nCount, char *lpszFileName);
VOS_STATUS CopyFileToMemory(UINT8 *pucByte, int nCount, char *lpszFileName, int nFilePos);
VOS_STATUS cp(char *lpszOldFileName, char *lpszDstFileName);
#endif
UINT32 GetTickCount(void);
void repeat(int nTotalCount, int nBurstCount, int nBurstPeriodInMS, SYSWORD pFunc, SYSWORD nParam1, SYSWORD nParam2, SYSWORD nParam3, SYSWORD nParam4, SYSWORD nParam5, SYSWORD nParam6);
#if 0
VOS_STATUS sysRun(char *lpszCmd, int *pResult);
#endif

#ifdef __cplusplus
}
#endif

#endif


