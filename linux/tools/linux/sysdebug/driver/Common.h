// Common.h: interface for the CCommon class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _COMMON_H_
#define _COMMON_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "diag.h"


#define Diag_Assert(bFlag, strException) \
    if (!(bFlag))						\
	{									\
		Diag_Output("Assert At file:%s, Line:%d, Cause:%s\n", (INT8*)__FILE__, (UINT16)__LINE__ ,(INT8*)strException);   \
	}	

char * FormatString(UINT8 * pucMsg, UINT16 usLen);

int ReadFromString(char *lpszBuffer, UINT8 *pucData, int nLen);

void mSleep(int nMs);

UINT32 sysClkRateGet(void);


void* MapPhyToVirtual(unsigned long pPhyAddr, unsigned long ulLength, BOOL bCacheFlag);

void UnmapPhyToVirtual(void *pVirtualAddr, UINT32 ulLength);

VOS_STATUS ReadPhyMemory(void *pucDestVirtualMem, unsigned long pucSrcPhyMem, UINT32 ulSize);

VOS_STATUS WritePhyMemory(unsigned long pucDestPhyMem, void * pucSrcVirtualMem, UINT32 ulSize);


#ifdef __cplusplus
}
#endif

#endif

