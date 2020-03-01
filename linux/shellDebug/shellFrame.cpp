
#include "shellFrame.h"
#include "commonLib.h"
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h> // exit()
#include <string.h> // memset()
#include <unistd.h> // close()
#include <sys/socket.h>
#include <netinet/in.h> // IPPROTO_TCP, htons(), htons()
#include <arpa/inet.h> // inet_addr()
#include <pthread.h>
#include <sys/select.h>
#include <unistd.h>

#define SHELL_DEBUG_VERSION		"0.0.1"

long d0, d1, d2, d3, d4, d5, d6, d7, d8, d9;
void *p0, *p1, *p2, *p3, *p4, *p5, *p6, *p7, *p8, *p9;

DATA_MAP g_theDataList[] = 
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
//const int g_theDataListCount = sizeof(g_theDataList) / sizeof(FUNC_MAP);


static FUNC_MAP s_theFuncList[] = 
{
#if 0
		{"Add",		  		Add, "SYSWORD Add(SYSWORD a, SYSWORD b)"},		
		{"Test",		  Test, "void Test(int d1, int d2, int d3, int d4, int d5, int d6, int d7, int d8, int d9, int d10)"},		
		{"version", 		version,		"void version();"	},
        {"version", 	    version,	    "void version();" 	},
        {"ExecuteScript", 	ExecuteScript,	    "VOS_STATUS ExecuteScript(char *lpszFileName);" 	},
        {"repeat", 	        repeat,		        "void repeat(int nTotalCount, int nBurstCount, int nBurstPeriodInMS, int pFunc, int nParam1, int nParam2, int nParam3, int nParam4, int nParam5, int nParam6);" 	},
        {"GetTickCount", 	GetTickCount,		"UINT32 GetTickCount()" 	},
        {"crc", 			crc,				"UINT16 crc(UINT8 *ptr,  int nLen)" 	},
#endif        
		{"malloc",			(void *)malloc,				"void *malloc(int nSize)"							},	
		{"free",			(void *)free,				"void free(void *ptr);"							},	
		{"help", 			(void *)help,				"void help(char *lpszFuncName)"								},
		{"lkup", 			(void *)lkup,		        "void lkup(char *lpszFuncName))"						},

		{"readHexString",	(void *)readHexString,		"int readHexString(const char *lpszBuffer, UINT8 *pucData, int nLen)"	},
#if 0		
		{"sp", 				sp,		            "HANDLE sp(pFuncPtr pFunc, int nData1, int nData2, int nData3, int nData4, int nData5, int nData6, int nData7, int nData8, int nData9, int nData10)"						},		
#endif		
		{"d", 				(void *)d,					"void d(void *pAddr, int nByteCount, int nWidth, int nDisplayOffset))"						},
#if 0		
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
#endif		 
		
		{"memset",			(void *)memset,				"void *memset( void *dest, UINT8 ucData, size_t count );"	},
		{"memcpy",			(void *)memcpy,				"void *memcpy(void *dest, void *src, int nCount)"	},
		{"memcmp",			(void *)memcmp,				"int memcmp (const void *s1,const void *s2,size_t n);"	},
		{"memchk",			(void *)memchk,				"void * memchk(UINT8 *pucByte, UINT8 ucData, UINT32 ulLength)"	},
		
		{"strcpy",			(void *)strcpy,				"char *strcpy(char *dest, char *src)"	},
		{"strcat",			(void *)strcat,				"char *strcat (char *dest,const char *src);"	},
		{"strcmp",			(void *)strcmp,				"int strcmp(const char *s1,const char *s2); "	},
		{"strlen",			(void *)strlen,				"size_t strlen (const char *s); "	},
		
		{"printf", 			(void *)printf,				"int printf(const char *, ...)" 		},
		{"sprintf", 		(void *)sprintf,			"int sprintf( char *str,const char * format,.........);" 		},
#if 0
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
#endif	

//const int g_theFuncListCount = sizeof(g_theFuncList) / sizeof(FUNC_MAP);
};

FUNC_MAP *g_pFuncMap = NULL;
int g_FuncMapCount = 0;

void addFunctionMap(FUNC_MAP *pFuncMap, int nCount)
{
	FUNC_MAP *pNewMap = NULL;
	
	if (nCount <= 0)
	{
		return;
	}

	pNewMap = new FUNC_MAP[g_FuncMapCount + nCount];
	for (int i = 0; i < g_FuncMapCount; i++)
	{
		pNewMap[i] = g_pFuncMap[i];
	}
	for (int i = 0; i < nCount; i++)
	{
		pNewMap[g_FuncMapCount + i] = pFuncMap[i]; 
	}
	
	if (g_FuncMapCount != 0)
	{
		delete []g_pFuncMap;
	}
	g_pFuncMap = pNewMap;
	g_FuncMapCount += nCount;
}

void addBasicFuncMap()
{
	static int init = 0;
	if (init != 0)
	{
		return;
	}
	init = 1;
	addFunctionMap(s_theFuncList, sizeof(s_theFuncList) / sizeof(FUNC_MAP));
}


void help(char *lpszFunctionName)
{
    printf("Compiled time: %s %s   Version: %s\n", __DATE__, __TIME__, SHELL_DEBUG_VERSION);
	lkup(lpszFunctionName);
}

void lkup(char *lpszFuncName)
{
	int i;

	for (i = 0; i < g_FuncMapCount; i++)
	{
		if ((lpszFuncName == NULL) || (strlen(lpszFuncName) == 0))
		{
			printf("%20s    %s\n", g_pFuncMap[i].m_lpszName, g_pFuncMap[i].m_lpszHelp);
		}
		else if (strstr(g_pFuncMap[i].m_lpszName, lpszFuncName) != NULL)
		{
			printf("%20s    %s\n", g_pFuncMap[i].m_lpszName, g_pFuncMap[i].m_lpszHelp);
		}
	}
}


bool checkQuot(char *lpszCmd)
{
	int count = 0;
	for (int i = 0; lpszCmd[i] != 0; i++)
	{
		if (lpszCmd[i] == '\"')
		{
			count++;
		}
	}
	return ((count % 2) == 0);
}

int ReadString(char *string, const char * lpszAlph)
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

	
	// For example " = " "+" "-" "*" "/"
	if (strchr("+-*/=", string[0]) != NULL)
	{
	    lpszResult[0] = string[0];
	    lpszResult[1] = 0;
		return (1);
	}

	// For example  {abc} <==>  "abc"
	else if (string[0] == '{')
	{
		/* '{' must match with '}' */
		for (nIndex = 1; ; nIndex++)
		{
			if (string[nIndex] == 0)
			{
				/* Just '{abc' , it should be regards as error */
				return (0);
			}

			if (string[nIndex] == '}')
			{
				/* we found '}' at last */
				lpszResult[0] = '\"';
				memcpy(lpszResult + 1, string + 1, nIndex - 1);
				lpszResult[nIndex] = '\"';
				lpszResult[nIndex + 1] = 0;
				return (nIndex + 1);
			}
		}
	}
	    
	
	/* For example "abc" */
	else if (string[0] == '\"')
	{
		/* must be a pair of \"  */
		for (nIndex = 1; ; nIndex++)
		{
			if (string[nIndex] == 0)
			{
				/* we can't find another \" */
				return (0);
			}

			if (string[nIndex] == '\"')
			{
				/* we found another \" */
				memcpy(lpszResult, string, nIndex + 1);
				lpszResult[nIndex + 1] = 0;
				return (nIndex + 1);
			}
		}
	}
	else
	{

    	/* 读出合法的字符串*/
    	nResult = ReadString(string, "*&0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_.");
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
	char lpszData[1024];
	char *lpszBuffer = lpszData;
	char lpszResult[MAX_SYMBOL_LENGTH];

	if (strlen(lpszCommand) > 1024)
	{
        return (0);
	}
	strcpy(lpszBuffer, lpszCommand);

	for (nCount = 0; nCount < MAX_SYMBOL_COUNT; nCount++)
	{
		nIndex = ReadString(lpszBuffer, " ,()\t");
		lpszBuffer += nIndex;
		if (*lpszBuffer == '\0')
		{
			break;
		}
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


STATUS GetSymbolAsFunc(char *lpszSymbol, SYSWORD *pulAddr)
{
	int i;
	for (i = 0; i < g_FuncMapCount; i++)
	{
		if (strcmp(lpszSymbol, g_pFuncMap[i].m_lpszName) == 0)
		{
			*pulAddr = (SYSWORD)(g_pFuncMap[i].m_pAddr);
			return (STATUS_OK);
		}
	}

	return (STATUS_ERROR);
}

STATUS GetSymbolAsData(char *lpszSymbol, SYSWORD *pulData)
{
	int i;
	for (i = 0; i < sizeof(g_theDataList) / sizeof(DATA_MAP); i++)
	{
		if (strcmp(lpszSymbol, g_theDataList[i].m_lpszName) == 0)
		{
			*pulData = (SYSWORD)(g_theDataList[i].m_pAddr);
			return (STATUS_OK);
		}
	}

	return (STATUS_ERROR);
}

STATUS ExplainSymbol(char *lpszSymbol, char *lpszSymbolString, SYSWORD *pResult, SYMBOL_TYPE *pSymbolType)
{
//	int i;
	int nIndex = 0;
	char c;

	/* operation:  + - * /  */
	if (((lpszSymbol[0] == '+') || (lpszSymbol[0] == '-') || (lpszSymbol[0] == '*') || (lpszSymbol[0] == '/')) && (lpszSymbol[1] == 0))
	{
		*pResult = (SYSWORD)lpszSymbol[0];
		*pSymbolType = SYMBOL_OPERATION;
		return (STATUS_OK);
	}
	
	/* Address and value */
	if ((lpszSymbol[0] == '*') || (lpszSymbol[0] == '&'))
	{
		nIndex = 1;	
	}
	if (GetSymbolAsData(lpszSymbol + nIndex, pResult) == STATUS_OK)
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
		return (STATUS_OK);			
	}

	/* String with "" */
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
					return (STATUS_OK);
				}
			}
		}
	}

	/* Hex value */
	if ((lpszSymbol[0] == '0') && ((lpszSymbol[1] == 'x') || (lpszSymbol[1] == 'X')))
	{
		nIndex = sscanf(lpszSymbol, "%lx%c", pResult, &c);
		if (nIndex == 1)
		{
			*pSymbolType = SYMBOL_NUMBER;
			return (STATUS_OK);
		}
	}

	/* data */
	nIndex = sscanf(lpszSymbol, "%ld%c", pResult, &c);
	if (nIndex == 1)
	{
		*pSymbolType = SYMBOL_NUMBER;
		return (STATUS_OK);
	}

	/* function Name */
	if (GetSymbolAsFunc(lpszSymbol, pResult) == STATUS_OK)
	{
		*pSymbolType = SYMBOL_FUNCTION;
		return (STATUS_OK);	
	}

	return (STATUS_ERROR);
}


STATUS GetSymbolValue(SYSWORD pSymbolData[MAX_SYMBOL_COUNT], SYMBOL_TYPE pSymbolType[MAX_SYMBOL_COUNT], int nSymbolCount, SYSWORD *pulResult, int nStartIndex)
{
	pFuncPtr theFunc;
	SYSWORD ulData, ulData1, ulData2;
	STATUS iRet;
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
			return (STATUS_ERROR);
		}
		return (STATUS_OK);
	}

	/* For example:  "d1 + 5" */
	else if (((nSymbolCount - nStartIndex) == 3) && (pSymbolType[nStartIndex + 1] == SYMBOL_OPERATION))
	{
		iRet = GetSymbolValue(pSymbolData, pSymbolType, nStartIndex + 1, (SYSWORD *)&ulData1, nStartIndex);
		if (iRet == STATUS_ERROR)
		{
			return (STATUS_ERROR);
		}
		iRet = GetSymbolValue(pSymbolData, pSymbolType, nStartIndex + 3, (SYSWORD *)&ulData2, nStartIndex + 2);
		if (iRet == STATUS_ERROR)
		{
			return (STATUS_ERROR);
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
		return (STATUS_OK);
	}

	else /* For example:  "add(1, 2) " */
	{
		/* Now we can execute the function */	
		if (pSymbolType[nStartIndex] != SYMBOL_FUNCTION)
		{
			return (STATUS_ERROR);
		}
		theFunc = (pFuncPtr)pSymbolData[nStartIndex];
		for (nIndex = nStartIndex + 1; nIndex < nSymbolCount; nIndex++)
		{
			switch (pSymbolType[nIndex])
			{
			case SYMBOL_VALUE: /*  = p1 */
				pSymbolData[nIndex] = *(SYSWORD *)(pSymbolData[nIndex]);
				break;
			case SYMBOL_CONTENT: /* = *p1 */
				ulData = *(SYSWORD *)(pSymbolData[nIndex]);
				pSymbolData[nIndex] = *(SYSWORD *)ulData;
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
		return (STATUS_OK);	
	}
	
}

STATUS RunExpression(char lpszSymbol[MAX_SYMBOL_COUNT][MAX_SYMBOL_LENGTH], char lpszSymbolString[MAX_SYMBOL_COUNT][MAX_SYMBOL_LENGTH], int nSymbolCount, SYSWORD *pulResult, int nStartIndex)
{
	/* Get Function Address */
	SYSWORD pSymbolData[MAX_SYMBOL_COUNT];
	int nIndex;
	SYMBOL_TYPE	 pSymbolType[MAX_SYMBOL_COUNT];
	STATUS iRet;

	/* Check the parameter */
	if (nSymbolCount - nStartIndex > 11)
	{
		printf("too many parameters!\n");
		return (STATUS_ERROR);
	}

	for (nIndex = nStartIndex; nIndex < nSymbolCount; nIndex++)
	{
		if (ExplainSymbol(lpszSymbol[nIndex], lpszSymbolString[nIndex] , (SYSWORD *)(pSymbolData + nIndex), pSymbolType + nIndex) == STATUS_ERROR)
		{
#ifndef RUN_SYSTEM_WHEN_POSSIBLE 	
			printf("ExplainSymblo failed for %s!\n", lpszSymbol[nIndex]);
#endif
			return (STATUS_ERROR);
		}
	}

	/* Can not explain the expression */
	if (nIndex != nSymbolCount)
	{
		return (STATUS_ERROR);
	}

	iRet = GetSymbolValue(pSymbolData, pSymbolType, nSymbolCount, pulResult, nStartIndex);

	return (iRet);
}


STATUS explainCmd(char *lpszBuffer, SYSWORD * pulResult)
{
	int nSymbolCount;
	char lpszSymbol[MAX_SYMBOL_COUNT][MAX_SYMBOL_LENGTH];
	char lpszSymbolString[MAX_SYMBOL_COUNT][MAX_SYMBOL_LENGTH];
	SYSWORD nResult = 0;
	SYSWORD SymbolData;
	SYMBOL_TYPE SymbolType;

	if (checkQuot(lpszBuffer) == false)
	{
		return (STATUS_ERROR);
	}

	nSymbolCount = ReadSymbolTable(lpszBuffer, lpszSymbol);
	if (nSymbolCount == 0)
	{
		return (STATUS_ERROR);
	}

	/* if the second symbol is '=', it means a = func(b) */
	if ((nSymbolCount > 2) && (strcmp(lpszSymbol[1], "=") == 0))
	{
		/* Explain the first symbol */
		if (ExplainSymbol(lpszSymbol[0], lpszSymbolString[0], (SYSWORD *)&SymbolData, &SymbolType) == STATUS_ERROR)
		{
			return (STATUS_ERROR);
		}
		if ((SymbolType != SYMBOL_CONTENT) && (SymbolType != SYMBOL_VALUE))
		{
			return (STATUS_ERROR);
		}

		if (RunExpression(lpszSymbol, lpszSymbolString, nSymbolCount, &nResult, 2) == STATUS_ERROR)
		{
			return STATUS_ERROR;
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
			return (STATUS_ERROR);
		}

		*pulResult = nResult;
		return (STATUS_OK);
	}
	else
	{
		if (RunExpression(lpszSymbol, lpszSymbolString, nSymbolCount, &nResult, 0) == STATUS_OK)
		{
			*pulResult = nResult;
			return (STATUS_OK);
		}
		else
		{
			return (STATUS_ERROR);
		}
	}
}



