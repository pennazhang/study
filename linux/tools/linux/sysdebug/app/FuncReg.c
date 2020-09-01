
#ifdef __cplusplus
extern "C" {
#endif

#include "FuncReg.h"

#include "diagLib.h"
#include "Diag_UDPTest.h"
#include "diag_function.h"
#include "Diag_ExternalFunction.h"
#include "SendArp.h"
#include "Common.h"
#include "cpuShow.h"
#include "pciLib.h"
#include "pciConfigLib.h"

#include "sysdebugApi.h"


int d0, d1, d2, d3, d4, d5, d6, d7, d8, d9;
void *p0, *p1, *p2, *p3, *p4, *p5, *p6, *p7, *p8, *p9;
 
SYSWORD Add(SYSWORD a, SYSWORD b)
{
	return (a + b);
}

SYSWORD Sub(SYSWORD a, SYSWORD b)
{
	return (a - b);
}


UINT8 auc100[100];
UINT16 aus100[100];
UINT32 aul100[100];
/* 
显示数组可以输入：
-> ShowArray "aul100"
 */
/* 用于保存数组 */
DATA_MAP g_theArray[] = 
{
		{"auc100", 		auc100},
		{"aus100", 		aus100},
		{"aul100", 		aul100},
};


void ArpTest(char *lpszIfName)
{
	p1 = malloc(0x10);
	ReadFromString("ff ff ff ff ff ff", p1, 6);
	p2 = malloc(0x10);
	ReadFromString ("00 e0 81 59 c3 8f", p2, 6);
	p3 = p2; 
	p4 = malloc(0x10);
	ReadFromString("0a 38 16 0f", p4, 6);
	p5 = p2;
	p6 = p4;

	p7 = malloc(0x80);
	FillArpPacket(p7, p1, p2, 1, p3, p4, p5, p6);
	SendARP(lpszIfName, p7, 60);
}

/* 用于保存数字和指针，不能保存数组 */
DATA_MAP g_theDataReg[] = 
{
		{"d1", 		&d1		 	},
		{"d2", 		&d2			},
		{"d3", 		&d3			},
		{"d4", 		&d4			},
		{"d5", 		&d5			},
		{"d6", 		&d6			},
		{"d7", 		&d7			},
		{"d8", 		&d8			},
		{"d9", 		&d9			},
		{"p1", 		&p1		 	},
		{"p2", 		&p2			},
		{"p3", 		&p3			},
		{"p4", 		&p4			},
		{"p5", 		&p5			},
		{"p6", 		&p6			},
		{"p7", 		&p7			},
		{"p8", 		&p8			},
		{"p9", 		&p9			},
};

void Test(int d1, int d2, int d3, int d4, int d5, int d6, int d7, int d8, int d9, int d10)
{
	Diag_Output("The function you call is: Test %d, %d, %d, %d, %d, %d, %d, %d, %d, %d\n", d1, d2, d3, d4, d5, d6, d7, d8, d9, d10);
}

#if defined(CONFIG_BLACKADDER)
void ShowEcc();
#endif


FUNC_MAP g_theFuncReg[] = 
{
		{"Add",		  		Add, "SYSWORD Add(SYSWORD a, SYSWORD b)"},		
		{"Test",		  Test, "void Test(int d1, int d2, int d3, int d4, int d5, int d6, int d7, int d8, int d9, int d10)"},		
		{"version", 		version,		"void version();"	},
        {"version", 	    version,	    "void version();" 	},
        {"ExecuteScript", 	ExecuteScript,	    "VOS_STATUS ExecuteScript(char *lpszFileName);" 	},
        {"repeat", 	        repeat,		        "void repeat(int nTotalCount, int nBurstCount, int nBurstPeriodInMS, int pFunc, int nParam1, int nParam2, int nParam3, int nParam4, int nParam5, int nParam6);" 	},
        {"GetTickCount", 	GetTickCount,		"UINT32 GetTickCount()" 	},
        {"crc", 			crc,				"UINT16 crc(UINT8 *ptr,  int nLen)" 	},
		{"help", 			help,				"void help(char *lpszFuncName)"								},
		{"lkup", 			lkup,		        "void lkup(char *lpszFuncName))"						},
		{"sp", 				sp,		            "HANDLE sp(pFuncPtr pFunc, int nData1, int nData2, int nData3, int nData4, int nData5, int nData6, int nData7, int nData8, int nData9, int nData10)"						},		
		{"d", 				d,					"void d(void *pAddr, int nByteCount, int nWidth, int nDisplayOffset))"						},
		{"ShowRegArray", 	ShowRegArray,		"SYSWORD ShowRegArray(char *lpszArray)"						},
		{"ShowRegData", 	ShowRegData,		"void ShowRegData()"						},
		{"ShowRegFunc", 	ShowRegFunc,		"void ShowRegFunc()"						},

		{"RunCommand", 		RunCommand,			"VOS_STATUS RunCommand(UINT32 ulCommandID, char *lpszCommandName, UINT32 ulParam1, UINT32 ulParam2, UINT32 ulParam3, UINT32 ulParam4, UINT32 ulParam5, UINT32 ulParam6, UINT32 ulParam7, UINT32 ulParam8, UINT32 ulParam9, UINT32 ulParam10)"},

		{"SetUINT32", 		SetUINT32,			"void SetUINT32(UINT32 *pulAddr, UINT32 ulData);"},
		{"SetUINT16", 		SetUINT16,			"void SetUINT16(UINT16 *pusAddr, UINT16 usData);"},
		{"SetUINT8", 		SetUINT8,			"void SetUINT8(UINT16 *pucAddr, UINT8 ucData);"},
		{"GetUINT32", 		GetUINT32,			"UINT32 GetUINT32(UINT32 *pulAddr);"},
		{"GetUINT16", 		GetUINT16,			"UINT16 GetUINT16(UINT16 *pusAddr);"},
		{"GetUINT8", 		GetUINT8,			"UINT8 GetUINT8(UINT8 *pucAddr);"},

		{"open",			open,				"int open( const char * pathname, int flags);"							},	
		{"lseek",			lseek,				"off_t lseek(int fildes,off_t offset ,int whence);"							},	
		{"close",			close,				"int close(int nFd)"							},	
		{"read",			read,				"ssize_t read(int fd,void * buf ,size_t count);"							},	
		{"sync",			sync,				"int sync(void)"							},	
		{"write",			write,				"ssize_t write (int fd,const void * buf,size_t count);"							},	
		 
		{"malloc",			malloc,				"void *malloc(int nSize)"							},	
		{"free",			free,				"void free(void *ptr);"							},	
		
		{"memset",			memset,				"void *memset( void *dest, UINT8 ucData, size_t count );"	},
		{"memcpy",			memcpy,				"void *memcpy(void *dest, void *src, int nCount)"	},
		{"memcmp",			memcmp,				"int memcmp (const void *s1,const void *s2,size_t n);"	},
		{"memchk",			memchk,				"void * memchk(UINT8 *pucByte, UINT8 ucData, UINT32 ulLength)"	},
		
		{"strcpy",			strcpy,				"char *strcpy(char *dest, char *src)"	},
		{"strcat",			strcat,				"char *strcat (char *dest,const char *src);"	},
		{"strcmp",			strcmp,				"int strcmp(const char *s1,const char *s2); "	},
		{"strlen",			strlen,				"size_t strlen (const char *s); "	},
		
		{"printf", 			Diag_Output,		"int printf(const char *, ...)" 		},
		{"sprintf", 		sprintf,			"int sprintf( char *str,const char * format,.........);" 		},
		{"cp", 				cp,					"VOS_STATUS cp(char *lpszOldFileName, char *lpszDstFileName, int nCopyUnit, int nStartAddr, int nEndAddr )" 		},
		{"mSleep", 			mSleep,			"void mSleep(int nMs)" 		},

		{"GetFileLength",	    GetFileLength,		"int GetFileLength(char *lpszFileName);"},
		{"CopyMemoryToFile",	CopyMemoryToFile,	"VOS_STATUS CopyMemoryToFile(UINT8 *pucByte, int nCount, char *lpszFileName);"},
		{"AppendMemoryToFile",	AppendMemoryToFile,	"VOS_STATUS AppendMemoryToFile(UINT8 *pucData, int nCount, char *lpszFileName);"},
		{"CopyFileToMemory",    CopyFileToMemory,	"VOS_STATUS CopyFileToMemory(UINT8 *pucByte, int nCount, char *lpszFileName, int nOffset);"},

		{"sysRun",			sysRun,				"int sysRun(char *lpszCmd);" },
		{"CopyMemFromUserToKernel", 		CopyMemFromUserToKernel,			"VOS_STATUS CopyMemFromUserToKernel(char *pKernelDest, char *pUserSrc, int nSize); "},
		{"CopyMemFromKernelToUser", 		CopyMemFromKernelToUser,			"VOS_STATUS CopyMemFromKernelToUser(char *pUserDest, char *pKernelSrc, int nSize);"},
		{"CopyMemFromUserToPhy", 		CopyMemFromUserToPhy,			"VOS_STATUS CopyMemFromUserToPhy(char *pKernelDest, char *pUserSrc, int nSize); "},
		{"CopyMemFromPhyToUser", 		CopyMemFromPhyToUser,			"VOS_STATUS CopyMemFromPhyToUser(char *pUserDest, char *pKernelSrc, int nSize);"},
		
		{"dm",					dm, "void dm(void *pAddr, int nByteCount, int nWidth); " },
		{"GetMAC",          GetMAC,	                "VOS_STATUS GetMAC(char *lpszIfName, UINT8 pucMAC[6]);"},
		{"GetIP",          GetIP,	                "VOS_STATUS GetIP(char *lpszIfName, UINT8 pucIP[4])"},
		{"GetNetMask",      GetNetMask,	            "VOS_STATUS GetNetMask(char *lpszIfName, UINT8 pucMASK[4])"},
		{"FillArpPacket",          FillArpPacket,	"VOS_STATUS FillArpPacket(UINT8 *pucData, UINT8 *pucDstMAC, UINT8 *pucSrcMac, UINT8 usOpCode, UINT8 *pucSrcMAC, UINT8 *pucSrcIP, UINT8 * pucDstMAC, UINT8 *pucDstIP);"},
		{"ReadFromString",          ReadFromString,	"int ReadFromString(char *lpszBuffer, UINT8 *pucData, int nLen)"},
		{"SendARP",          SendARP,	"VOS_STATUS SendARP(char *lpszIfName, UINT8 *pucData, int nDataLen)"},		


		{"BeginUdpTest", 	BeginUdpTest,		"UINT32 BeginUdpTest(char *lpszName, char *lpszLocalAddress, UINT32 ulLocalPort)"},
		{"EndUdpTest",		EndUdpTest,			"VOS_STATUS EndUdpTest(UINT32 ulTestID)"},
		{"SetAutoReply",	SetAutoReply,		"VOS_STATUS SetAutoReply(UINT32 ulTestID, BOOL bFlag)"},
		{"ShowUdpStat",		ShowUdpStat,		"void ShowUdpStat();"},
		{"ClearUdpStat",	ClearUdpStat,		"void ClearUdpStat();"},
		{"BeginUdpForward",	BeginUdpForward,		"UINT32 BeginUdpForward(char *lpszLocalAddress, UINT32 ulLocalPort, char *lpszRemoteAddress, UINT32 ulRemotePort);"},
		{"EndUdpForward",	EndUdpForward,		"void EndUdpForward(UINT32 ulTestID);"},
		{"ShowCPUUtility",	ShowCPUUtility, 		"void ShowCPUUtility(int nPeriodInSecond, int nCPUCount);"		},
		{"ShowThreadPriority",	ShowThreadPriority, 		"void ShowThreadPriority();"		},
#if defined(CONFIG_BLACKADDER)
		{"ShowEcc",		ShowEcc, 			"void ShowEcc();"		},
#endif

#if defined(CONFIG_P2020RDB)
		{"pciDeviceShow",	pciDeviceShow, 		"void pciDeviceShow(int nPCIController);" },
		{"pciHeaderShow",	pciHeaderShow,		"void pciHeaderShow(int nBusNo, int nDeviceNo, int nFuncNo);"},
		{"SetPCIController",	SetPCIController,		"void SetPCIController(int nPCIController);" },
		{"pciConfigInByte",	pciConfigInByte, 	"VOS_STATUS pciConfigInByte	(int busNo, int	deviceNo, int funcNo, int address, UINT8 * pData);"},
		{"pciConfigInWord", pciConfigInWord,	"VOS_STATUS pciConfigInWord (int busNo, int	deviceNo, int funcNo, int address, UINT16 * pData);"},
		{"pciConfigInLong", pciConfigInLong,	"VOS_STATUS pciConfigInLong (int busNo, int	deviceNo, int funcNo, int address, UINT32 * pData);"},
#endif		
};

SYSWORD ShowRegArray(char *lpszBuffer)
{
    int i;
    
	for (i = 0; i < sizeof(g_theArray) / sizeof(DATA_MAP); i++)
	{
	    if (lpszBuffer == NULL)
	    {
            Diag_Output("%30s : 0x%lx\n", g_theArray[i].m_lpszName, (SYSWORD)g_theArray[i].m_pAddr);
	    }
	    else if (strcmp(lpszBuffer, g_theArray[i].m_lpszName) == 0)
		{
            Diag_Output("%30s : 0x%lx\n", g_theArray[i].m_lpszName, (SYSWORD)g_theArray[i].m_pAddr);
			return ((SYSWORD)(g_theArray[i].m_pAddr));
		}
	}
	if (lpszBuffer != NULL)
	{
        Diag_Output("    Array %s not found\n", lpszBuffer);
	}

	return (0);
}



THREAD_DATA theThread;
void * StandardThread(void *pData)
{
	THREAD_DATA *pThreadData = (THREAD_DATA *)pData;
	int nData1, nData2, nData3, nData4, nData5, nData6, nData7, nData8, nData9, nData10;
	pFuncPtr pFunc = pThreadData->m_pFunc;
	SYSWORD nReturn;

	nData1 = pThreadData->m_nData1;
	nData2 = pThreadData->m_nData2;
	nData3 = pThreadData->m_nData3;
	nData4 = pThreadData->m_nData4;
	nData5 = pThreadData->m_nData5;
	nData6 = pThreadData->m_nData6;
	nData7 = pThreadData->m_nData7;
	nData8 = pThreadData->m_nData8;
	nData9 = pThreadData->m_nData9;
	nData10 = pThreadData->m_nData10;

	nReturn = pFunc(nData1, nData2, nData3, nData4, nData5, nData6, nData7, nData8, nData9, nData10);

	return ((void *)nReturn);
}

#if 1
pthread_t sp(pFuncPtr pFunc, SYSWORD nData1, SYSWORD nData2, SYSWORD nData3, SYSWORD nData4, SYSWORD nData5, SYSWORD nData6, SYSWORD nData7, SYSWORD nData8, SYSWORD nData9, SYSWORD nData10)
{
	pthread_t pid;

	theThread.m_pFunc = pFunc;
	theThread.m_nData1 = nData1;	
	theThread.m_nData2 = nData2;	
	theThread.m_nData3 = nData3;	
	theThread.m_nData4 = nData4;	
	theThread.m_nData5 = nData5;	
	theThread.m_nData6 = nData6;
	theThread.m_nData7 = nData7;
	theThread.m_nData8 = nData8;
	theThread.m_nData9 = nData9;
	theThread.m_nData10 = nData10;
/*Delete by percy for pthread_create can not pass compile */	
	pthread_create(&pid, NULL, StandardThread, &theThread);
	usleep(10);

	return pid;
}
#else
UINT32 sp(pFuncPtr pFunc, int nData1, int nData2, int nData3, int nData4, int nData5, int nData6, int nData7, int nData8, int nData9, int nData10)
{
		uintptr_t hThread = (uintptr_t)-1;
		int nStackSize = 0;
		
		theThread.m_pFunc = pFunc;
		theThread.m_nData1 = nData1;	
		theThread.m_nData2 = nData2;	
		theThread.m_nData3 = nData3;	
		theThread.m_nData4 = nData4;	
		theThread.m_nData5 = nData5;	
		theThread.m_nData6 = nData6;
		theThread.m_nData7 = nData7;
		theThread.m_nData8 = nData8;
		theThread.m_nData9 = nData9;
		theThread.m_nData10 = nData10;
		
		/*Delete by percy for pthread_create can not pass compile */	
		hThread = _beginthread(StandardThread, nStackSize, (void *)&theThread);
		mSleep(10);	
		return (UINT32)hThread;
}
#endif

void lkup(char *lpszFuncName)
{
	int i;

	for (i = 0; i < sizeof(g_theFuncReg) / sizeof (FUNC_MAP); i++)
	{
		if ((lpszFuncName == NULL) || (strlen(lpszFuncName) == 0))
		{
			Diag_Output("%20s    0x%08lX    %s\n", g_theFuncReg[i].m_lpszName, (SYSWORD)g_theFuncReg[i].m_pAddr, g_theFuncReg[i].m_lpszHelp);
		}
		else if (strstr(g_theFuncReg[i].m_lpszName, lpszFuncName) != NULL)
		{
			Diag_Output("%20s    0x%08lX    %s\n", g_theFuncReg[i].m_lpszName, (SYSWORD)g_theFuncReg[i].m_pAddr, g_theFuncReg[i].m_lpszHelp);
		}
	}
}

void ShowRegData()
{
	int i;

	for (i = 0; i < sizeof(g_theDataReg) / sizeof (DATA_MAP); i++)
	{
		Diag_Output("%20s", g_theDataReg[i].m_lpszName);
		if ((i & 0x3) == 3)
		{
			Diag_Output("\n");
		}
	}
}

void ShowRegFunc(void)
{
	int i;

	for (i = 0; i < sizeof(g_theFuncReg) / sizeof (FUNC_MAP); i++)
	{
		Diag_Output("%20s    %s\n", g_theFuncReg[i].m_lpszName, g_theFuncReg[i].m_lpszHelp);
	}
}

void help(char *lpszFunctionName)
{
    Diag_Output("Compiled time: %s %s   Version: %s\n", __DATE__, __TIME__, DIAG_VERSION);
	lkup(lpszFunctionName);
}

int ReadString(char *string, char * lpszAlph)
{
	int i;

	for (i = 0; ; i++)
	{
		if (string[i] == 0)
		{
			break;
		}
		
		if (strchr(lpszAlph, string[i]) == NULL)
		{
			break;
		}
	}

	return i;
}

int ReadSymbol(char *string, char *lpszResult)
{
	int nResult, nIndex;

//	Diag_Output("Searching symbol for string:%s\n", string);
	if (string[0] == '=')
	{
	    lpszResult[0] = string[0];
	    lpszResult[1] = 0;
		return (1);
	}

	else if (string[0] == '{')
	{
		/* 后面紧跟着的都是符号的内容 */
		for (nIndex = 1; ; nIndex++)
		{
			if (string[nIndex] == 0)
			{
				/* 只有前括号，没有后括号 */
				return (0);
			}

			if (string[nIndex] == '}')
			{
				/* 找到后括号了 */
				lpszResult[0] = '\"';
				memcpy(lpszResult + 1, string + 1, nIndex - 1);
				lpszResult[nIndex] = '\"';
				lpszResult[nIndex + 1] = 0;
				return (nIndex + 1);
			}
		}
	}
	    
	
	/* 查看是否是字符串类型的符号 */
	else if (string[0] == '\"')
	{
		/* 后面紧跟着的都是符号的内容 */
		for (nIndex = 1; ; nIndex++)
		{
			if (string[nIndex] == 0)
			{
				/* 只有前引号，没有后引号 */
				return (0);
			}

			if (string[nIndex] == '\"')
			{
				/* 找到后引号了 */
				memcpy(lpszResult, string, nIndex + 1);
				lpszResult[nIndex + 1] = 0;
				return (nIndex + 1);
			}
		}
	}
	else
	{

    	/* 读出合法的字符串*/
    	nResult = ReadString(string, "*&0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_+-.");
    	if (nResult != 0)
    	{
    		memcpy(lpszResult, string, nResult);
    	    lpszResult[nResult] = 0;
    	}
    	return (nResult);
	}
}

int ReadSymbolTable(char *lpszCommand, char lpszSymbol[MAX_SYMBOL_COUNT][MAX_SYMBOL_LENGTH])
{
	int nCount = 0;
	int nIndex = 0;
    UINT32 i;
	char lpszData[1024];
	char *lpszBuffer = lpszData;
	char lpszResult[MAX_SYMBOL_LENGTH];

	if (strlen(lpszCommand) > 1024)
	{
        return (0);
	}
	strcpy(lpszBuffer, lpszCommand);

	/* 注意//  # 和;是注释符，后面的所有内容都要省略 */
	for (i = 0; i < strlen(lpszBuffer); i++)
	{
        if ((lpszBuffer[i] == '#') || (lpszBuffer[i] == ';') || (lpszBuffer[i] == '\r') || (lpszBuffer[i] == '\n'))
        {
            lpszBuffer[i] = 0;
            break;
        }

        if ((lpszBuffer[i] == '\\') && (lpszBuffer[i + 1] == '\\')) 
        {
            lpszBuffer[i] = 0;
            break;
        }
	}


	for (nCount = 0; nCount < MAX_SYMBOL_COUNT; nCount++)
	{
		nIndex = ReadString(lpszBuffer, " ,()\t");
		lpszBuffer += nIndex;        
		nIndex = ReadSymbol(lpszBuffer, lpszResult);
		if ((nIndex != 0) && (nIndex < MAX_SYMBOL_LENGTH))
		{
			strcpy(lpszSymbol[nCount], lpszResult);
//			Diag_Output("symbol %d = %s\n", nCount, lpszSymbol[nCount]);
			lpszBuffer += nIndex;
		}
		else
		{
			break;
		}
	}

	return (nCount);
}

VOS_STATUS GetSymbolAsFunc(char *lpszSymbol, SYSWORD *pulAddr)
{
	UINT32 i;
	for (i = 0; i < sizeof(g_theFuncReg) / sizeof(FUNC_MAP); i++)
	{
		/* 检查是否是函数 */
		if (strcmp(lpszSymbol, g_theFuncReg[i].m_lpszName) == 0)
		{
			*pulAddr = (SYSWORD)(g_theFuncReg[i].m_pAddr);
			return (VOS_OK);
		}
	}

	return (VOS_ERROR);
}

VOS_STATUS GetSymbolAsData(char *lpszSymbol, SYSWORD *pulData)
{
	int i;
	for (i = 0; i < sizeof(g_theDataReg) / sizeof(DATA_MAP); i++)
	{
		/* 检查是否是函数 */
		if (strcmp(lpszSymbol, g_theDataReg[i].m_lpszName) == 0)
		{
			*pulData = (SYSWORD)(g_theDataReg[i].m_pAddr);
			return (VOS_OK);
		}
	}

	return (VOS_ERROR);
}

/* 解释单个symbol */
VOS_STATUS ExplainSymbol(char *lpszSymbol, char *lpszSymbolString, SYSWORD *pResult, SYMBOL_TYPE *pSymbolType)
{
//	int i;
	int nIndex = 0;
	char c;

	/* 检查是否是 + - * / 等符号 */
	if (((lpszSymbol[0] == '+') || (lpszSymbol[0] == '-') || (lpszSymbol[0] == '*') || (lpszSymbol[0] == '/')) && (lpszSymbol[1] == 0))
	{
		*pResult = (SYSWORD)lpszSymbol[0];
		*pSymbolType = SYMBOL_OPERATION;
		return (VOS_OK);
	}
	
	/* 检查是否是注册变量 */
	if ((lpszSymbol[0] == '*') || (lpszSymbol[0] == '&'))
	{
		nIndex = 1;	
	}
	if (GetSymbolAsData(lpszSymbol + nIndex, pResult) == VOS_OK)
	{
		if (lpszSymbol[0] == '*')
		{
			*pSymbolType = SYMBOL_CONTENT;
		}
		else if (lpszSymbol[0] == '&')
		{
			*pSymbolType = SYMBOL_ADDRESS;
		}
		else
		{
			*pSymbolType = SYMBOL_VALUE;
		}
		return (VOS_OK);			
	}

	/* 检查是否是字符串 */
	if (lpszSymbol[0] == '\"')
	{
		for (nIndex = 1; ; nIndex++)
		{
			if (lpszSymbol[nIndex] == 0)
			{
				break;
			}
			if (lpszSymbol[nIndex] == '\"')
			{
				if (lpszSymbol[nIndex + 1] == 0)
				{
					*pSymbolType = SYMBOL_STRING;
					strcpy(lpszSymbolString, lpszSymbol + 1);
					lpszSymbolString[nIndex - 1] = 0;

					*pResult = (SYSWORD)lpszSymbolString;
					return (VOS_OK);
				}
			}
		}
	}

	/* 检查是否是16进制整数 */
	if ((lpszSymbol[0] == '0') && ((lpszSymbol[1] == 'x') || (lpszSymbol[1] == 'X')))
	{
		nIndex = sscanf(lpszSymbol, "%lx%c", pResult, &c);
		if (nIndex == 1)
		{
			*pSymbolType = SYMBOL_NUMBER;
			return (VOS_OK);
		}
	}

	/* 检查是否是十进制整数 */
	nIndex = sscanf(lpszSymbol, "%ld%c", pResult, &c);
	if (nIndex == 1)
	{
		*pSymbolType = SYMBOL_NUMBER;
		return (VOS_OK);
	}

	/* 检查是否是注册函数 */
	if (GetSymbolAsFunc(lpszSymbol, pResult) == VOS_OK)
	{
		*pSymbolType = SYMBOL_FUNCTION;
		return (VOS_OK);	
	}

	return (VOS_ERROR);
}

VOS_STATUS GetSymbolValue(SYSWORD pSymbolData[MAX_SYMBOL_COUNT], SYMBOL_TYPE pSymbolType[MAX_SYMBOL_COUNT], int nSymbolCount, SYSWORD *pulResult, int nStartIndex)
{
	pFuncPtr theFunc;
	SYSWORD ulData, ulData1, ulData2;
	VOS_STATUS iRet;
	int nIndex;

	/* Just one symbol */
	if (((nSymbolCount - nStartIndex) == 1) && (pSymbolType[nStartIndex] != SYMBOL_FUNCTION))
	{
		switch (pSymbolType[nStartIndex])
		{
		case SYMBOL_ADDRESS: /*  &d1 取d1的地址	*/
			*pulResult = pSymbolData[nStartIndex];
			break;
		case SYMBOL_VALUE: /*  = d1 */
			*pulResult = *(SYSWORD *)(pSymbolData[nStartIndex]);
			break;
		case SYMBOL_CONTENT: /* = *p1 */
			ulData = *(SYSWORD *)(pSymbolData[nStartIndex]);
			*pulResult = *(SYSWORD *)ulData;
			break;
		case SYMBOL_NUMBER:
			*pulResult = pSymbolData[nStartIndex];
			break;
		default:
			return (VOS_ERROR);
		}
		return (VOS_OK);
	}
	/* 判断是否是有 ＋ － × / 四种符号的表达式，比如：5 ＋ 8，这种表达式目前只接收3个符号 */
	else if (((nSymbolCount - nStartIndex) == 3) && (pSymbolType[nStartIndex + 1] == SYMBOL_OPERATION))
	{
		iRet = GetSymbolValue(pSymbolData, pSymbolType, nStartIndex + 1, (SYSWORD *)&ulData1, nStartIndex);
		if (iRet == VOS_ERROR)
		{
			return (VOS_ERROR);
		}
		iRet = GetSymbolValue(pSymbolData, pSymbolType, nStartIndex + 3, (SYSWORD *)&ulData2, nStartIndex + 2);
		if (iRet == VOS_ERROR)
		{
			return (VOS_ERROR);
		}

		switch (pSymbolData[nStartIndex + 1])
		{
		case '+':
			*pulResult = ulData1 + ulData2;
			break;
		case '-':
			*pulResult = ulData1 - ulData2;
			break;
		case '*':
			*pulResult = ulData1 * ulData2;
			break;
		case '/':
			*pulResult = ulData1 / ulData2;
			break;
		}
		return (VOS_OK);
	}
	else //剩下判断是否为函数
	{
		/* Now we can execute the function */	
		if (pSymbolType[nStartIndex] != SYMBOL_FUNCTION)
		{
			return (VOS_ERROR);
		}
		theFunc = (pFuncPtr)pSymbolData[nStartIndex];
		for (nIndex = nStartIndex + 1; nIndex < nSymbolCount; nIndex++)
		{
			switch (pSymbolType[nIndex])
			{
			case SYMBOL_VALUE: /*  = p1 */
				pSymbolData[nIndex] = *(UINT32 *)(pSymbolData[nIndex]);
				break;
			case SYMBOL_CONTENT: /* = *p1 */
				ulData = *(UINT32 *)(pSymbolData[nIndex]);
				pSymbolData[nIndex] = *(UINT32 *)ulData;
				break;
			default:
				break;
			}
		}

		for (nIndex = nSymbolCount; nIndex <= 10 + nStartIndex; nIndex++)
		{
			pSymbolData[nIndex] = 0;
		}

		*pulResult = theFunc(pSymbolData[1 + nStartIndex], pSymbolData[2 + nStartIndex], 
								pSymbolData[3 + nStartIndex], pSymbolData[4 + nStartIndex], 
								pSymbolData[5 + nStartIndex], pSymbolData[6 + nStartIndex],
								pSymbolData[7 + nStartIndex], pSymbolData[8 + nStartIndex],
								pSymbolData[9 + nStartIndex], pSymbolData[10 + nStartIndex]
							);
		return (VOS_OK);	
	}
	
}

VOS_STATUS RunExpression(char lpszSymbol[MAX_SYMBOL_COUNT][MAX_SYMBOL_LENGTH], char lpszSymbolString[MAX_SYMBOL_COUNT][MAX_SYMBOL_LENGTH], int nSymbolCount, SYSWORD *pulResult, int nStartIndex)
{
	/* Get Function Address */
	SYSWORD pSymbolData[MAX_SYMBOL_COUNT];
	int nIndex;
	SYMBOL_TYPE	 pSymbolType[MAX_SYMBOL_COUNT];
	VOS_STATUS iRet;

	/* Check the parameter */
	if (nSymbolCount - nStartIndex > 11)
	{
		Diag_Output("too many parameters!\n");
		return (VOS_ERROR);
	}

	for (nIndex = nStartIndex; nIndex < nSymbolCount; nIndex++)
	{
		if (ExplainSymbol(lpszSymbol[nIndex], lpszSymbolString[nIndex] , (SYSWORD *)(pSymbolData + nIndex), pSymbolType + nIndex) == VOS_ERROR)
		{
			Diag_Output("ExplainSymblo failed for %s!\n", lpszSymbol[nIndex]);
			break;
		}
	}

	/* Can not explain the expression */
	if (nIndex != nSymbolCount)
	{
		return (VOS_ERROR);
	}

	iRet = GetSymbolValue(pSymbolData, pSymbolType, nSymbolCount, pulResult, nStartIndex);

	return (iRet);
}

VOS_STATUS ExplainCmd(char *lpszBuffer, SYSWORD * pulResult)
{
	int iStartIndex = 0, iIndex;
	int nLength = strlen(lpszBuffer);
	int nTotalCommand = 0;

	for (iIndex = 0; iIndex <= nLength; iIndex++)
	{
		if ((lpszBuffer[iIndex] == ';') || (lpszBuffer[iIndex] == 0))
		{
			nTotalCommand++;
		}
	}
	
	for (iIndex = 0; iIndex <= nLength; iIndex++)
	{
		if ((lpszBuffer[iIndex] == ';') || (lpszBuffer[iIndex] == 0))
		{
			lpszBuffer[iIndex] = 0;
			if (nTotalCommand != 1)
			{
				Diag_Output("\n%s\n", lpszBuffer + iStartIndex);
			}
			if (ExplainSignleCmd(lpszBuffer + iStartIndex, pulResult) == VOS_ERROR)
			{
				if (nTotalCommand != 1)
				{
					Diag_Output("Execute function failed!\n");
				}
				return (VOS_ERROR);
			}
			else
			{
				iStartIndex = iIndex + 1;
				if (nTotalCommand != 1)
				{
					Diag_Output("Execute function successfully, Result = 0x%lX = %ld\n", *pulResult, *pulResult);
				}
			}				
		}
	}
	return (VOS_OK);
}

VOS_STATUS ExplainSignleCmd(char *lpszBuffer, SYSWORD * pulResult)
{
	int nSymbolCount;
	char lpszSymbol[MAX_SYMBOL_COUNT][MAX_SYMBOL_LENGTH];
	char lpszSymbolString[MAX_SYMBOL_COUNT][MAX_SYMBOL_LENGTH];
	SYSWORD nResult = 0;
	SYSWORD SymbolData;
	SYMBOL_TYPE SymbolType;


	/* 判断是否是: ,  表示执行sysRun函数 */
	if (lpszBuffer[0] == ':')
	{
		*pulResult = sysRun(lpszBuffer + 1);
		return (VOS_OK);
	}
	
	nSymbolCount = ReadSymbolTable(lpszBuffer, lpszSymbol);
	if (nSymbolCount == 0)
	{
		return (VOS_ERROR);
	}

	/* if the second symbol is '=', it means a = func(b) */
	if ((nSymbolCount > 2) && (strcmp(lpszSymbol[1], "=") == 0))
	{
		/* Explain the first symbol */
		if (ExplainSymbol(lpszSymbol[0], lpszSymbolString[0], (SYSWORD *)&SymbolData, &SymbolType) == VOS_ERROR)
		{
			return (VOS_ERROR);
		}
		if ((SymbolType != SYMBOL_CONTENT) && (SymbolType != SYMBOL_VALUE))
		{
			return (VOS_ERROR);
		}

		if (RunExpression(lpszSymbol, lpszSymbolString, nSymbolCount, &nResult, 2) == VOS_ERROR)
		{
			return VOS_ERROR;
		}

		if (SymbolType == SYMBOL_CONTENT)
		{
			SymbolData = *(SYSWORD *)SymbolData;
			*(SYSWORD *)SymbolData = nResult;
		}
		else if (SymbolType == SYMBOL_VALUE)
		{
			*(SYSWORD *)SymbolData = nResult;
		}
		else
		{
			return (VOS_ERROR);
		}

		*pulResult = nResult;
		return (VOS_OK);
	}
	else
	{
		if (RunExpression(lpszSymbol, lpszSymbolString, nSymbolCount, &nResult, 0) == VOS_OK)
		{
			*pulResult = nResult;
			return (VOS_OK);
		}
		else
		{
			return (VOS_ERROR);
		}
	}
}

VOS_STATUS RunCommand(UINT32 ulCommandID, char *lpszCommandName, SYSWORD ulParam1, SYSWORD ulParam2,
            SYSWORD ulParam3, SYSWORD ulParam4, SYSWORD ulParam5, SYSWORD ulParam6, SYSWORD ulParam7, 
            SYSWORD ulParam8, SYSWORD ulParam9, SYSWORD ulParam10)
{
    pFuncPtr pFunc;
    SYSWORD ulResult;

    SYSWORD ulAddr;

    if (GetSymbolAsFunc(lpszCommandName, &ulAddr) != VOS_OK)
    {
        Diag_Output("Func:%s Not Find!\n", lpszCommandName);
        return (VOS_ERROR);
    }

    pFunc = (pFuncPtr)ulAddr;
    ulResult = pFunc(ulParam1, ulParam2, ulParam3, ulParam4, ulParam5, ulParam6, ulParam7, ulParam8, ulParam9, ulParam10);

    Diag_Output("Function:%s Execute Successfully! Command ID = %d, Result = %ld.\n", lpszCommandName, ulCommandID, ulResult);
    return (VOS_OK);
}

void SetUINT32(UINT32 *pulAddr, UINT32 ulData)
{
	*pulAddr = ulData;
}

void SetUINT16(UINT16 *pusAddr, UINT16 usData)
{
	*pusAddr = usData;
}

void SetUINT8(UINT16 *pucAddr, UINT8 ucData)
{
	*pucAddr = ucData;
}

UINT32 GetUINT32(UINT32 *pulAddr)
{
	return (*pulAddr);
}

UINT16 GetUINT16(UINT16 *pusAddr)
{
	return (*pusAddr);
}

UINT8 GetUINT8(UINT8 *pucAddr)
{
	return (*pucAddr);
}

#if 0
BOOL IsBufferInSTDIN()
{
    struct timeval tv; 
    fd_set readfds; 
    tv.tv_sec = 0; 
    tv.tv_usec = 0; 
    FD_ZERO(&readfds);
    FD_SET(STDIN_FILENO, &readfds);
    select(1, &readfds, NULL, NULL, &tv); 
    if (FD_ISSET(STDIN_FILENO, &readfds))
    {
        return (TRUE);
    }
    else
    {
        return (FALSE);
    }
}
#endif

void CmdLoop()
{
	char lpszCommand[200];
	SYSWORD ulResult;
	char lpszSymbol[MAX_SYMBOL_COUNT][MAX_SYMBOL_LENGTH];
	int nSymbolCount;
//	BOOL bFlag;

	for (;;)
	{
	
//	    bFlag = IsBufferInSTDIN();
		Diag_Output("\n->");
#if 1
		fgets(lpszCommand, 200, stdin);
#else
		gets(lpszCommand);
#endif
//		if (bFlag == TRUE)
//		{
 //           Diag_Output("\n-> %s\n", lpszCommand);
//		}
//		GetLine(lpszCommand, 100);

		if ((memcmp(lpszCommand, "bye", 3) == 0) || (memcmp(lpszCommand, "exit", 4) == 0))
		{
			break;
		}

		/* 判断是否有内容 */
		if (lpszCommand[0] != ':')
		{
			nSymbolCount = ReadSymbolTable(lpszCommand, lpszSymbol);
			if (nSymbolCount == 0)
			{
				continue;
			}
		}
		
		/* 解释输入是否正确 */
		if (ExplainCmd(lpszCommand, &ulResult) == VOS_OK)
		{
			Diag_Output("\nExecute function successfully, Result = 0x%lX = %ld\n", ulResult, ulResult);
		}
		else
		{
			Diag_Output("Execute function failed!\n");
		}
	}
}

VOS_STATUS ExecuteScript(char *lpszFileName)
{
    FILE *pFile;
    int nSymbolCount;
    char lpszCommand[1024];
	char lpszSymbol[MAX_SYMBOL_COUNT][MAX_SYMBOL_LENGTH];
	SYSWORD ulResult;
	char *pResult;
    
    pFile = fopen(lpszFileName, "r");
    if (pFile == NULL)
    {
        Diag_Output("Can not open file: %s\n", lpszFileName);
        return (VOS_ERROR);
    }
    
    while(!feof(pFile))
    {
        /* 从文件中读出一行 */
        pResult = fgets(lpszCommand, 1024, pFile);
        if (pResult == lpszCommand)
        {
    		/* 判断是否有内容 */
			if (lpszCommand[0] != ':')
			{
	    		nSymbolCount = ReadSymbolTable(lpszCommand, lpszSymbol);
	    		if (nSymbolCount == 0)
	    		{
	    			continue;
	    		}
			}
            Diag_Output("->%s ", lpszCommand);

    		/* 解释输入是否正确 */
    		if (ExplainCmd(lpszCommand, &ulResult) == VOS_OK)
    		{
    			Diag_Output("\nExecute function successfully, Result = 0x%lX = %ld\n", ulResult, ulResult);
    		}
    		else
    		{
    			Diag_Output("Execute function failed!\n");
    		}
        }
        else
        {
            continue;
        }
    }

    fclose(pFile);

    return (VOS_OK);
}

void version()
{
    Diag_Output("Compiled time: %s %s   Version: %s\n", __DATE__, __TIME__, DIAG_VERSION);
}

int sysRun(char *lpszCommand)
{
	SYSWORD nResult;
	if (sysCommandRun(lpszCommand, &nResult) != 0)
	{
		return (-1);
	}
	else
	{
		return ((int)nResult);
	}
}
#if 0 
/*  测试多线程的函数，Test1 (5, 10, 2) */

/* The Function Test1 can be used to test function sp */
void PrintThread(int nTotalCount, int nInterval, char chParam)
{
	int i;
	for (i = 0; i < nTotalCount; i++)
	{
		mSleep(1000 * nInterval);
		printf("%c\n", chParam);
	}
}

int Test1(int nThreadNum, int nTotalCount, int nPrintIntervalInSecond)
{
	int i;
	for (i = 0; i < nThreadNum; i++)
	{
		sp(PrintThread, nTotalCount, nPrintIntervalInSecond, i + '0', 4, 5, 6, 7, 8, 9, 10);
	}
	return (0);
}
#endif

#if 0

/*  测试多线程之间同步的函数，Test2 (10, 1, 8, 1)  Test2 (40, 2, 10, 5)  */

HANDLE g_hTestEvent;
int g_nCount = 0;

void Thread1(int nTotalCount1, int nInterval)
{
	int i;
	for (i = 0; i < nTotalCount1; i++)
	{
		mSleep(nInterval * 1000);
		SetEvent(g_hTestEvent);
	}
	printf("Thread1 Quit!\n");
}

void Thread2(int nTotalCount2, int nThread2SleepSecond)
{

	for (;;)
	{
		WaitForSingleObject(g_hTestEvent, INFINITE);
//		ResetEvent(g_hTestEvent);
		g_nCount++;
		mSleep(nThread2SleepSecond * 1000);
		printf("%d\n", g_nCount);
		if (nTotalCount2 <= g_nCount)
		{
			break;
		}
	}
	printf("Thread2 Quit!\n");
}

void Test(int nTotalCount1, int nInterval, int nTotalCount2, int nThread2SleepSecond)
{
	BOOL bManualResetEvent = FALSE; 	/* TRUE :必须每次调用WaitForSingleObject后手动调用ResetEvent来清除Event，
										FALSE:只要调用WaitForSingleObject后，Event就会被自动清除 */
	BOOL bInitFlag = FALSE;  /* 初始没有信号量 */
	g_hTestEvent = CreateEvent(NULL, bManualResetEvent, bInitFlag, NULL); 
	if (g_hTestEvent == NULL)
	{
		printf("CreateEvent Failed!\n");
		return;
	}
	else
	{
		printf("hEvent = 0x%x\n", g_hTestEvent); 
	}

	g_nCount = 0;
	
	sp(Thread1, nTotalCount1, nInterval, 3, 4, 5, 6, 7, 8, 9, 10);
	sp(Thread2, nTotalCount2, nThread2SleepSecond, 3, 4, 5, 6, 7, 8, 9, 10);
}
	
#endif
/*
void Test(int nTotalCount1, int nInterval, int nTotalCount2, int nThread2SleepSecond)
{
	return;
}
*/

#ifdef __cplusplus
}
#endif

