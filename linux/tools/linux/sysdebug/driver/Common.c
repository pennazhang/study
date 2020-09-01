// Common.cpp: implementation of the CCommon class.
//
//////////////////////////////////////////////////////////////////////


#ifdef __cplusplus
extern "C" {
#endif

#include "Common.h"
#include "FuncReg.h"
#include "Common.h"
/*
#include <stdarg.h>
*/

//定义FormatString数组
char g_lpszFormatString[1024];

BOOL IsValidNumber(char pucData)
{
    if ((pucData >= '0') && (pucData <= '9'))
    {
        return (TRUE);
    }
    if ((pucData >= 'a') && (pucData <= 'f'))
    {
        return (TRUE);
    }
    if ((pucData >= 'A') && (pucData <= 'F'))
    {
        return (TRUE);
    }

    return (FALSE);
}

/***************************************************
从字符串中读出数据传输到数组中，
For Example:  ReadFromString ("23 12 9a ba 08 00", aucMac, 6);
*****************************************************/
int ReadFromString(char *lpszBuffer, UINT8 *pucData, int nLen)
{
    int i;
    char cData[3];
    UINT32 ulData;

    cData[2] = 0;
    for (i = 0; i < nLen; i++)
    {
        cData[0] = lpszBuffer[0];
        cData[1] = lpszBuffer[1];
        
        if ((IsValidNumber(cData[0]) == TRUE) && (IsValidNumber(cData[1]) == TRUE))
        {
            sscanf(cData, "%x", &ulData);
            pucData[i] = (UINT8)ulData;
            if (lpszBuffer[2] != ' ')
            {
                i++;
                break;
            }
            lpszBuffer += 3;
        }
        else
        {
            break;
        }
    }

    return (i);
}

char * FormatString(UINT8 * pucMsg, UINT16 usLen)
{
    UINT16 ucCount = 0;
    UINT16 i;
    INT8 lpszTemp[] =  "00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F 10 11 12 13 14 15 16 17 18 19 1A 1B 1C 1D 1E 1F 20 21 22 23 24 25 26 27 28 29 2A 2B 2C 2D 2E 2F 30 31 32 33 34 35 36 37 38 39 3A 3B 3C 3D 3E 3F 40 41 42 43 44 45 46 47 48 49 4A 4B 4C 4D 4E 4F 50 51 52 53 54 55 56 57 58 59 5A 5B 5C 5D 5E 5F 60 61 62 63 64 65 66 67 68 69 6A 6B 6C 6D 6E 6F 70 71 72 73 74 75 76 77 78 79 7A 7B 7C 7D 7E 7F 80 81 82 83 84 85 86 87 88 89 8A 8B 8C 8D 8E 8F 90 91 92 93 94 95 96 97 98 99 9A 9B 9C 9D 9E 9F A0 A1 A2 A3 A4 A5 A6 A7 A8 A9 AA AB AC AD AE AF B0 B1 B2 B3 B4 B5 B6 B7 B8 B9 BA BB BC BD BE BF C0 C1 C2 C3 C4 C5 C6 C7 C8 C9 CA CB CC CD CE CF D0 D1 D2 D3 D4 D5 D6 D7 D8 D9 DA DB DC DD DE DF E0 E1 E2 E3 E4 E5 E6 E7 E8 E9 EA EB EC ED EE EF F0 F1 F2 F3 F4 F5 F6 F7 F8 F9 FA FB FC FD FE FF ";

	ucCount = usLen;
	if (ucCount > 500 / 3)
	{
		ucCount = 500 / 3;
	}

	for (i = 0; i < ucCount; i++)
	{
		memcpy(g_lpszFormatString + 3 * i, lpszTemp + 3 * pucMsg[i], 3);
//			sprintf(g_lpszFormatString + 3 * i, "%02x ", (int)(pucContent[i]));
	}

	g_lpszFormatString[ucCount * 3] = 0;
	return (g_lpszFormatString);
}

void mSleep(int nMs)
{
	msleep(nMs);
}

UINT32 sysClkRateGet()
{
	return (1000);
}

void* MapPhyToVirtual(unsigned long pPhyAddr, unsigned long ulLength, BOOL bCacheFlag)
{
	void *pReturn;

	mSleep(1000);	
	pReturn = ioremap(pPhyAddr, ulLength);
//	Diag_Output("MapPhyToVirtual 0x%x, 0x%x = 0x%x\n", (UINT32)pPhyAddr, (UINT32)ulLength, (UINT32)pReturn);
	return (pReturn);
	
}

void UnmapPhyToVirtual(void *pVirtualAddr, UINT32 ulLength)
{
	iounmap(pVirtualAddr);
}

VOS_STATUS ReadPhyMemory(void *pucDestVirtualMem, unsigned long pucSrcPhyMem, UINT32 ulSize)
{
	void *pSrcVirtualMem = MapPhyToVirtual(pucSrcPhyMem, ulSize, FALSE);
	if (pSrcVirtualMem == NULL)
	{
 		Diag_Output("MapPhyToVirtual failed!\n");
		return (VOS_ERROR);
	}
	
	Memcpy(pucDestVirtualMem, pSrcVirtualMem, ulSize);
	UnmapPhyToVirtual(pSrcVirtualMem, ulSize);
	return (VOS_OK);
}


VOS_STATUS WritePhyMemory(unsigned long pucDestPhyMem, void * pucSrcVirtualMem, UINT32 ulSize)
{
	void *pDestVitrualMem = MapPhyToVirtual(pucDestPhyMem, ulSize, FALSE);
	if (pDestVitrualMem == NULL)
	{
		Diag_Output("MmMapIoSpace failed!\n");
		return (VOS_ERROR);
	}
	
	Memcpy(pDestVitrualMem, pucSrcVirtualMem, ulSize);
	UnmapPhyToVirtual(pDestVitrualMem, ulSize);
	return (VOS_OK);
}

#if 0

void Diag_Output(char * pFormat, ...)
{
    char pszOutput[5000];
    char *pszTemp = pszOutput;
    va_list sArgList; 
    
    va_start(sArgList, pFormat);
    vsprintf(pszTemp, pFormat, sArgList);
    va_end(sArgList);

	printf(pszTemp);
}

#endif

#ifdef __cplusplus
}
#endif

