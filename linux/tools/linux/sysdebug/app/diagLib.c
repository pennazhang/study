
#ifdef __cplusplus
extern "C" {
#endif

#include "diagLib.h"
#include "time.h"
#include "FuncReg.h"
#include "Common.h"
#include "sysdebugApi.h"

int g_nByteCount = 0x10;
int g_nWidth = 1;

void * memchk(UINT8 *pucByte, UINT8 ucData, UINT32 ulLength)
{
    UINT32 i;
    for (i = 0; i < ulLength; i++)
    {
        if (pucByte[i] != ucData)
        {
            return ((void *)((UINT32)pucByte + i));
        }
    }

    return (NULL);
}

void d(void *pAddr, int nByteCount, int nWidth, int nDisplayOffset)
{
	UINT32 nStart, nEnd, i;
	UINT8 *pucData;
	UINT16 *pusData;
	UINT32 *pulData;

	if (nByteCount <= 0)
	{
		nByteCount = g_nByteCount;
	}
	else
	{
		g_nByteCount = nByteCount;
	}

	if ((nWidth != 1) && (nWidth != 2) && (nWidth != 4))
	{
		nWidth = g_nWidth;
	}
	else
	{
		g_nWidth = nWidth;
	}

	nStart = (UINT32)pAddr;
	nEnd = (UINT32)pAddr + nByteCount;
	
	for (i = nStart; i < nEnd; i+=16)
	{
		Diag_Output("%08x :", i + nDisplayOffset);

		if (nWidth == 1)
		{
			pucData = (UINT8 *)i;
			Diag_Output(" %02x  %02x  %02x  %02x  %02x  %02x  %02x  %02x -  %02x  %02x  %02x  %02x  %02x  %02x  %02x  %02x \n",
				pucData[0], pucData[1], pucData[2], pucData[3], pucData[4], pucData[5], pucData[6], pucData[7], 
				pucData[8],	pucData[9], pucData[10], pucData[11], pucData[12], pucData[13], pucData[14], pucData[15]);
		}
		else if (nWidth == 2)
		{
			pusData = (UINT16 *)i;
			Diag_Output(" %04x %04x %04x %04x - %04x %04x %04x %04x \n", 
				pusData[0], pusData[1], pusData[2], pusData[3], 
				pusData[4], pusData[5], pusData[6], pusData[7]); 
		}
		else if (nWidth == 4)
		{
			pulData = (UINT32 *)i;
			Diag_Output(" %08x %08x - %08x %08x \n", 
				pulData[0], pulData[1], pulData[2], pulData[3]);
		}
	}
}

VOS_STATUS ReadPhyMem(void *pDestAddr, void * pPhyMem, int nByteCount)
{
	char lpszCmd[0x100];
	UINT32 ulKernelAddr;
	
	if ((nByteCount < 0) || (nByteCount > 0x100000))
	{
		Diag_Output("Invalid nByteCount: %d\n", nByteCount);
		return (VOS_ERROR);
	}

	sprintf(lpszCmd, "malloc %d", nByteCount);
	ulKernelAddr = sysRun(lpszCmd);
	if (ulKernelAddr == 0)
	{
		Diag_Output("Memory Block is too big!\n");
		return (VOS_ERROR);
	}

	sprintf(lpszCmd, "ReadPhyMemory 0x%x, 0x%x, %d", ulKernelAddr, (UINT32)pPhyMem, nByteCount);
	sysRun(lpszCmd);

	CopyMemFromKernelToUser(pDestAddr, (char *)ulKernelAddr, nByteCount);

	sprintf(lpszCmd, "free 0x%x", ulKernelAddr);
	sysRun(lpszCmd);
	return (VOS_OK);
}

UINT32 GetPhy_UINT32(int nPhyAddress)
{
	UINT32 ulResult = 0;

	if (ReadPhyMem((void *)&ulResult, (void *)nPhyAddress, 4) != VOS_OK)
	{
		printf("Read Phy: 0x%x error!\n", nPhyAddress);
		return (VOS_ERROR);
	}
	else
	{
		return (ulResult);
	}
}
void dm(void *pAddr, int nByteCount, int nWidth)
{
	char lpszCmd[0x100];
	UINT32 ulData;
	void *pAppAddr;
	
	if (nByteCount <= 0)
	{
		nByteCount = g_nByteCount;
	}
	else
	{
		g_nByteCount = nByteCount;
	}

	if ((nWidth != 1) && (nWidth != 2) && (nWidth != 4))
	{
		nWidth = g_nWidth;
	}
	else
	{
		g_nWidth = nWidth;
	}

	sprintf(lpszCmd, "malloc %d", nByteCount);
	ulData = sysRun(lpszCmd);
	if (ulData == 0)
	{
		Diag_Output("Memory Block is too big!\n");
		return;
	}

	sprintf(lpszCmd, "ReadPhyMemory 0x%x, 0x%x, %d", ulData, (UINT32)pAddr, nByteCount);
	sysRun(lpszCmd);

	pAppAddr = malloc(nByteCount);
	CopyMemFromKernelToUser(pAppAddr, (char *)ulData, nByteCount);
	d(pAppAddr, nByteCount, nWidth, (int)pAddr - (int)pAppAddr);
	free(pAppAddr);

	sprintf(lpszCmd, "free 0x%x", ulData);
	sysRun(lpszCmd);
}

#if defined(CONFIG_BLACKADDER)
void ShowEcc()
{
	unsigned ulData;

	ulData = GetPhy_UINT32((void *)0xFFE02110);
	printf("\nDDR SDRAM Control Configuration(Addr = 0xFFE02110): 0x%x\n", ulData);
	if (IsBitSet(ulData, 2, BIG_ENDIAN_MODE))
	{
		printf("ECC is enabled.\n");
	}
	else
	{
		printf("No ECC errors are reported. No ECC interrupts are generated.\n");
	}


	ulData = GetPhy_UINT32((void *)0xFFE02E08);
	printf("\nMemory Data Path Error Injection Mask ECC Register(Addr = 0xFFE02E08): 0x%x\n", ulData);

	if (IsBitSet(ulData, 15, BIG_ENDIAN_MODE))
	{
		printf("Address parity error injection: Enable\n");
	}
	else
	{
		printf("Address parity error injection: Disable\n");
	}
	
	if (IsBitSet(ulData, 22, BIG_ENDIAN_MODE))
	{
		printf("Mirror byte functionality: Enable\n");
	}
	else
	{
		printf("Mirror byte functionality: Disable\n");
	}

	if (IsBitSet(ulData, 23, BIG_ENDIAN_MODE))
	{
		printf("Error injection: Enable\n");
		ulData = GetPhy_UINT32((void *)0xFFE02E00);
		printf("Memory Data Path Error Injection Mask High(Addr = 0xFFE02E00): 0x%x\n", ulData);
		ulData = GetPhy_UINT32((void *)0xFFE02E04);
		printf("Memory Data Path Error Injection Mask High(Addr = 0xFFE02E04): 0x%x\n", ulData);
	}
	else
	{
		printf("Error injection: Disable\n");
	}


	ulData = GetPhy_UINT32((void *)0xFFE02E40);
	printf("\nMemory Error Detect Register(Addr = 0xFFE02E40): 0x%x\n", ulData);
	if (IsBitSet(ulData, 0, BIG_ENDIAN_MODE))
	{
		printf("Multiple memory errors of the same type were detected.\n");
	}
	else
	{
		printf("Multiple memory errors of the same type were not detected.\n");
	}
	
	if (IsBitSet(ulData, 23, BIG_ENDIAN_MODE))
	{
		printf("An address parity error has been detected.\n");
	}
	else
	{
		printf("An address parity error has not been detected.\n");
	}
	
	if (IsBitSet(ulData, 24, BIG_ENDIAN_MODE))
	{
		printf("An automatic calibration error has been detected.\n");
	}
	else
	{
		printf("An automatic calibration error has not been detected.\n");
	}

	if (IsBitSet(ulData, 28, BIG_ENDIAN_MODE))
	{
		printf("A multiple-bit error has been detected.\n");
	}
	else
	{
		printf("A multiple-bit error has not been detected.\n");
	}

	if (IsBitSet(ulData, 29, BIG_ENDIAN_MODE))
	{
		printf("The number of single-bit ECC errors detected crossed the threshold.\n");
	}
	else
	{
		printf("The number of single-bit ECC errors detected has not crossed the threshold.\n");
	}

	if (IsBitSet(ulData, 31, BIG_ENDIAN_MODE))
	{
		printf("A memory select error has been detected.\n");
	}
	else
	{
		printf("A memory select error has not been detected.\n");
	}
	
	ulData = GetPhy_UINT32((void *)0xFFE02E44);
	printf("\nMemory Error Disable Register (Addr = 0xFFE02E44): 0x%x\n", ulData);
	if (IsBitSet(ulData, 23, BIG_ENDIAN_MODE))
	{
		printf("Address parity errors are not detected or reported.\n");
	}
	else
	{
		printf("Address parity errors are detected and reported\n");
	}
	
	if (IsBitSet(ulData, 24, BIG_ENDIAN_MODE))
	{
		printf("Automatic calibration errors are disabled.\n");
	}
	else
	{
		printf("Automatic calibration errors are enabled.\n");
	}

	if (IsBitSet(ulData, 28, BIG_ENDIAN_MODE))
	{
		printf("Multiple-bit ECC errors are not detected or reported.\n");
	}
	else
	{
		printf("Multiple-bit ECC errors are detected and reported.\n");
	}

	if (IsBitSet(ulData, 29, BIG_ENDIAN_MODE))
	{
		printf("Single-bit ECC errors are disabled.\n");
	}
	else
	{
		printf("Single-bit ECC errors are enabled.\n");
	}

	if (IsBitSet(ulData, 31, BIG_ENDIAN_MODE))
	{
		printf("Memory select errors are disabled.\n");
	}
	else
	{
		printf("Memory select errors are enabled.\n");
	}

	ulData = GetPhy_UINT32((void *)0xFFE02E58);
	printf("\nSingle-Bit ECC Memory Error Management Register (Addr = 0xFFE02E58): 0x%x\n", ulData);
	printf("Single-bit error counter: 0x%x\n", ulData & 0xff);	
	printf("Single-bit error threshold: 0x%x\n", (ulData >> 16) & 0xff);

}

#endif

int GetFileLength(char *lpszFileName)
{
    int nTotal;
	FILE *pFile;

	pFile = fopen(lpszFileName, "rb");
    if (pFile == NULL)
    {
        return (VOS_ERROR);
    }

    if (fseek(pFile, 0, SEEK_END) != 0)
    {
    	fclose(pFile);
		return (VOS_ERROR);
	}
	
	nTotal = ftell(pFile);
    fclose(pFile);
    return (nTotal);
}

VOS_STATUS CopyMemoryToFile(UINT8 *pucByte, int nCount, char *lpszFileName)
{
    int nTotal;
	FILE *pFile;

	pFile = fopen(lpszFileName, "wb");
    if (pFile == NULL)
    {
        return (VOS_ERROR);
    }

    nTotal = fwrite(pucByte, 1, nCount, pFile);
    fclose(pFile);
    
    if (nTotal != nCount)
    {
        Diag_Output("Write file failed!\n");
        return (VOS_ERROR);
    }
    else
    {
        return (VOS_OK);
    }
}

	
VOS_STATUS AppendMemoryToFile(UINT8 *pucData, int nCount, char *lpszFileName)
{
    int nTotal;
	FILE *pFile;

	pFile = fopen(lpszFileName, "a+b");
    if (pFile == NULL)
    {
        return (VOS_ERROR);
    }

	if (fseek(pFile, 0, SEEK_END) != 0)
    {
    	fclose(pFile);
		return (VOS_ERROR);
	}
		
    nTotal = fwrite(pucData, 1, nCount, pFile);
    fclose(pFile);
    
    if (nTotal != nCount)
    {
        Diag_Output("Write file failed!\n");
        return (VOS_ERROR);
    }
    else
    {
        return (VOS_OK);
    }
}

VOS_STATUS CopyFileToMemory(UINT8 *pucByte, int nCount, char *lpszFileName, int nOffset)
{
    int nTotal;
	FILE *pFile;

    pFile = fopen(lpszFileName, "rb");
    if (pFile == NULL)
    {
        Diag_Output("Open file:%s failed!\n", lpszFileName);
        return (VOS_ERROR);
    }

    fseek(pFile, nOffset, SEEK_SET);
    nTotal = fread(pucByte, 1, nCount, pFile);
    fclose(pFile);
    
    if (nTotal != nCount)
    {
        Diag_Output("Read file failed! Total Read = %d\n", nTotal);
        return (VOS_ERROR);
    }
    else
    {
        return (VOS_OK);
    }
}

VOS_STATUS cp(char *lpszOldFileName, char *lpszDstFileName, int nCopyUnit, int nStart, int nEnd)
{
	UINT8 *pucByte = NULL;
	int nFileLength = GetFileLength(lpszOldFileName);
	FILE *pSrcFile = NULL, *pDstFile = NULL;
	VOS_STATUS iRet = VOS_ERROR;
	int nCopySize = 0;

	if (nCopyUnit == 0)
	{
		nCopyUnit = 16 * 1024 * 1024;  /* The default unit is 4M */
	}
	Diag_Output("Copy Unit = %d Bytes\n", nCopyUnit);
	
	if (nFileLength <= 0)
	{
		Diag_Output("Invalid source file: %s\n", lpszOldFileName);
		goto cp_end;
	}
	if (nEnd <= 0)
	{
		nEnd = nFileLength;
	}
	Diag_Output("Total Length = %d Bytes\n", nEnd - nStart);

	pucByte = (UINT8 *)malloc(nCopyUnit);
	if (pucByte == NULL)
	{
		Diag_Output("CopyUnit is too big to malloc memory!\n");
		goto cp_end;
	}

    pSrcFile = fopen(lpszOldFileName, "rb");
    if (pSrcFile == NULL)
    {
        Diag_Output("Open file:%s failed!\n", lpszOldFileName);
		goto cp_end;
    }

	pDstFile = fopen(lpszDstFileName, "wb");
    if (pDstFile == NULL)
    {
        Diag_Output("Open file:%s failed!\n", lpszDstFileName);
        return (VOS_ERROR);
    }

	fseek(pSrcFile, nStart, SEEK_SET);
	iRet = VOS_OK;
	for (;;)
	{
		if (nStart >= nEnd)
		{
			break;
		}
		if (nStart + nCopyUnit >=  nEnd)
		{
			nCopySize = nEnd - nStart;
		}
		else
		{
			nCopySize = nCopyUnit;
		}
		Diag_Output("Reading 0x%x - 0x%x...\n", nStart, nStart + nCopySize);
	    if (1 != fread(pucByte, nCopySize, 1, pSrcFile))
	    {
			Diag_Output("Reading failed!\n");
			iRet = VOS_ERROR;
		}

		Diag_Output("Writing 0x%x - 0x%x...\n", nStart, nStart + nCopySize);
		if (1 != fwrite(pucByte, nCopySize, 1, pDstFile))
		{
			Diag_Output("Writing failed!\n");
			iRet = VOS_ERROR;
		}
		nStart += nCopySize;		
	}
	Diag_Output("Copy finished.\n");

#if 0
	if (CopyFileToMemory(pucByte, nFileLength, lpszOldFileName, 0) != VOS_OK)
	{
		Diag_Output("Fail to read file: %s\n", lpszOldFileName);
		free(pucByte);
		return (VOS_ERROR);
	}

	if (CopyMemoryToFile(pucByte, nFileLength, lpszDstFileName) != VOS_OK)
	{
		Diag_Output("Fail to write file: %s\n", lpszDstFileName);
		free(pucByte);
		return (VOS_ERROR);
	}
#endif
cp_end:

	if (pDstFile != NULL)
	{
		Diag_Output("Closing file: %s...\n", lpszDstFileName);
		fclose(pDstFile);
	}
	if (pSrcFile != NULL)
	{
		Diag_Output("Closing file: %s...\n", lpszOldFileName);
		fclose(pSrcFile);
	}
	if (pucByte != NULL)
	{
		free(pucByte);
	}
	return (iRet);
}

UINT16 crc(UINT8 *ptr,  int nLen) 
{
	UINT16 crc;
	unsigned char ucData1, ucData2, ucData3;
	UINT16 crc_ta[256]={               /* CRC余式表 */
			0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7,
			0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef,
			0x1231, 0x0210, 0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6,
			0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c, 0xf3ff, 0xe3de,
			0x2462, 0x3443, 0x0420, 0x1401, 0x64e6, 0x74c7, 0x44a4, 0x5485,
			0xa56a, 0xb54b, 0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d,
			0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6, 0x5695, 0x46b4,
			0xb75b, 0xa77a, 0x9719, 0x8738, 0xf7df, 0xe7fe, 0xd79d, 0xc7bc,
			0x48c4, 0x58e5, 0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823,
			0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969, 0xa90a, 0xb92b,
			0x5af5, 0x4ad4, 0x7ab7, 0x6a96, 0x1a71, 0x0a50, 0x3a33, 0x2a12,
			0xdbfd, 0xcbdc, 0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a,
			0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03, 0x0c60, 0x1c41,
			0xedae, 0xfd8f, 0xcdec, 0xddcd, 0xad2a, 0xbd0b, 0x8d68, 0x9d49,
			0x7e97, 0x6eb6, 0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70,
			0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a, 0x9f59, 0x8f78,
			0x9188, 0x81a9, 0xb1ca, 0xa1eb, 0xd10c, 0xc12d, 0xf14e, 0xe16f,
			0x1080, 0x00a1, 0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067,
			0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c, 0xe37f, 0xf35e,
			0x02b1, 0x1290, 0x22f3, 0x32d2, 0x4235, 0x5214, 0x6277, 0x7256,
			0xb5ea, 0xa5cb, 0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d,
			0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
			0xa7db, 0xb7fa, 0x8799, 0x97b8, 0xe75f, 0xf77e, 0xc71d, 0xd73c,
			0x26d3, 0x36f2, 0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634,
			0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9, 0xb98a, 0xa9ab,
			0x5844, 0x4865, 0x7806, 0x6827, 0x18c0, 0x08e1, 0x3882, 0x28a3,
			0xcb7d, 0xdb5c, 0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a,
			0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0, 0x2ab3, 0x3a92,
			0xfd2e, 0xed0f, 0xdd6c, 0xcd4d, 0xbdaa, 0xad8b, 0x9de8, 0x8dc9,
			0x7c26, 0x6c07, 0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1,
			0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8,
			0x6e17, 0x7e36, 0x4e55, 0x5e74, 0x2e93, 0x3eb2, 0x0ed1, 0x1ef0
	};

	crc=0;
	while(nLen--!=0) 
	{
		ucData1 = (UINT8)(crc/256);    /* 以8位二进制数的形式暂存CRC的高8位 */
		crc = crc << 8;              /* 左移8位，相当于CRC的低8位乘以  */
		ucData2 = *ptr;
		ucData3 = ucData2 ^ ucData1;
		crc = crc ^ crc_ta[ucData3];   /* 高8位和当前字节相加后再查表求CRC ，再加上以前的CRC  */
		ptr++;
	}
	return(crc);
}

UINT32 GetTickCount()
{
    struct timeval tv;
    UINT32 ulTick;
    
    gettimeofday(&tv,NULL);
    ulTick = (UINT32)(tv.tv_sec * 1000 + tv.tv_usec / 1000);
    return (ulTick);
}

void repeat(int nTotalCount, int nBurstCount, int nBurstPeriodInMS, SYSWORD pFunc, SYSWORD nParam1, SYSWORD nParam2, SYSWORD nParam3, SYSWORD nParam4, SYSWORD nParam5, SYSWORD nParam6)
{
    int nSendCount, i;
    UINT32 ulNextBeginTick = GetTickCount(); 
    UINT32 ulCurrentTick;

//    pFuncPtr theFunc = (pFuncPtr)pFunc;
    pFuncPtr theFunc = (pFuncPtr)pFunc;

    for (nSendCount = 0; nSendCount < nTotalCount; )
    {
        ulCurrentTick = GetTickCount();
        if (ulCurrentTick < ulNextBeginTick)
        {
            mSleep(ulNextBeginTick - ulCurrentTick);
        }
        else
        {
            for (i = 0; i < nBurstCount; i++)
            {
                theFunc(nParam1, nParam2, nParam3, nParam4, nParam5, nParam6, 0, 0, 0, 0);
            }
//            Diag_Output("Last execute func burst for %d time at tick: %d\n", nBurstCount, ulCurrentTick);
            nSendCount += nBurstCount;
            
            ulCurrentTick = GetTickCount();
            ulNextBeginTick += nBurstPeriodInMS;

            if (ulNextBeginTick < ulCurrentTick)
            {
                mSleep(0xFFFFFFFF - ulCurrentTick + 1 + ulNextBeginTick);
            }
            else
            {
                mSleep(ulNextBeginTick - ulCurrentTick);
            }
        }
    }
    return;    
}

void * MemFind(UINT8 *pucAddr, UINT8 *pucKey, int nKeyLength, int nMemoryLength)
{
	UINT8 *pStartAddr = pucAddr;
	int nFirstChar = (int)pucKey[0];

	if ((nKeyLength > nMemoryLength) || (nKeyLength < 0) || (nMemoryLength < 0))
	{
		return (NULL);
	}
	for (;;)
	{
		if ((SYSWORD)pStartAddr >= (SYSWORD)pucAddr + nMemoryLength)
		{
			return (NULL);
		}
		pStartAddr = memchr(pStartAddr, nFirstChar, (SYSWORD)pucAddr + nMemoryLength  - (SYSWORD)pStartAddr);
		if (pStartAddr == NULL)
		{
			return (NULL);
		}
		if (memcmp(pStartAddr, pucKey, nKeyLength) == 0)
		{
			return (pStartAddr);
		}
		pStartAddr++;
	}
}

void ShowThreadPriority()
{
	int status;
	struct sched_param thread_param;
	int thread_policy;

	status = pthread_getschedparam(pthread_self(), &thread_policy, &thread_param);
	if (status) 
	{
		printf("report_threadinfo: failed to get scheduler param: 0x%x\n", status);
		return;
	}
	printf("running as %s thread at priority %d\n",
	     thread_policy == SCHED_FIFO ? "SCHED_FIFO" :
	     thread_policy == SCHED_RR ? "SCHED_RR" : "SCHED_OTHER",
	     thread_param.sched_priority);
	return;
}

#ifdef __cplusplus
}
#endif


