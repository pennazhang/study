
#include "tcpServer.h"
#include "common.h"
#include <netinet/in.h>    // for sockaddr_in
#include <sys/types.h>    // for socket
#include <sys/socket.h>    // for socket
#include <stdio.h>        // for printf
#include <stdlib.h>        // for exit
#include <string.h>        // for bzero
#include <pthread.h>
#include <sys/errno.h>    // for errno
#include <unistd.h>   //define close
#include <arpa/inet.h>
#include <signal.h>
#include "debugFrame.h"

#define SERVER_IP "127.0.0.1"
#define BUFFER_SIZE	 2048
#define LENGTH_OF_LISTEN_QUEUE	5		// accept at most 5 tcp client 


#define MAX_SYMBOL_COUNT 20
#define MAX_SYMBOL_LENGTH 80


/* 用于保存数字和指针，不能保存数组 */
SYSWORD d0, d1, d2, d3, d4, d5, d6, d7, d8, d9;
void *p0, *p1, *p2, *p3, *p4, *p5, *p6, *p7, *p8, *p9;

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

extern FUNC_MAP g_theFuncReg[];
extern unsigned int FUNC_REG_SIZE;

static void sig_int(int num)
{
	debugExit();
    return;
}

STATUS debugInit(int tcpPort)
{
	static bool s_initFlag = false;

	if (s_initFlag == true)
	{
		return STATUS_OK;
	}
	
	if (tcpServerInit(tcpPort) == STATUS_OK)
	{
		s_initFlag = true;
		signal(SIGINT, sig_int);//注册信号 捕获SIGINT(中断)信号,然后调用sig_int函数 （Ctrl-C会产生这个信号）

		return (STATUS_OK);
	}
	
	return (STATUS_ERROR);
}

void debugExit()
{
	tcpServerExit();
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

STATUS GetSymbolAsFunc(char *lpszSymbol, SYSWORD *pulAddr)
{
	unsigned int i;  
//	int nResult;
	for (i = 0; i < FUNC_REG_SIZE; i++)
	{
		/* 检查是否是函数 */
		if (strcmp(lpszSymbol, g_theFuncReg[i].m_lpszName) == 0)
		{
			*pulAddr = (SYSWORD)g_theFuncReg[i].m_pAddr;
			return (STATUS_OK);
		}
	}

    /* This code is just for kernel */
#if 0
    nResult = (UINT)kallsyms_lookup_name(lpszSymbol);
    if (nResult != 0)
	{
        *pulAddr = nResult;
        return (STATUS_OK);
	}
#endif
	return (STATUS_ERROR);
}

STATUS GetSymbolAsData(char *lpszSymbol, SYSWORD *pulData)
{
	UINT i;
	for (i = 0; i < sizeof(g_theDataReg) / sizeof(DATA_MAP); i++)
	{
		/* 检查是否是函数 */
		if (strcmp(lpszSymbol, g_theDataReg[i].m_lpszName) == 0)
		{
			*pulData = (SYSWORD)g_theDataReg[i].m_pAddr;
			return (STATUS_OK);
		}
	}

	return (STATUS_ERROR);
}

int ReadSymbol(char *string, char *lpszResult)
{
	int nResult, nIndex;

//	printf("Searching symbol for string:%s\n", string);
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
	/* 判断是否是有 ＋ － × / 四种符号的表达式，比如：5 ＋ 8，这种表达式目前只接收3个符号 */
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

		char *pSymbol = (char *)pSymbolData[nStartIndex + 1];
		switch (*pSymbol)
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
	else //剩下判断是否为函数
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

/* 解释单个symbol */
STATUS ExplainSymbol(char *lpszSymbol, char *lpszSymbolString, SYSWORD *pResult, SYMBOL_TYPE *pSymbolType)
{
//	int i;
	int nIndex = 0;
	char c;

	/* 检查是否是 + - * / 等符号 */
	if (((lpszSymbol[0] == '+') || (lpszSymbol[0] == '-') || (lpszSymbol[0] == '*') || (lpszSymbol[0] == '/')) && (lpszSymbol[1] == 0))
	{
		*pResult = lpszSymbol[0];
		*pSymbolType = SYMBOL_OPERATION;
		return (STATUS_OK);
	}
	
	/* 检查是否是注册变量 */
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
					return (STATUS_OK);
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
			return (STATUS_OK);
		}
	}

	/* 检查是否是十进制整数 */
	nIndex = sscanf(lpszSymbol, "%ld%c", pResult, &c);
	if (nIndex == 1)
	{
		*pSymbolType = SYMBOL_NUMBER;
		return (STATUS_OK);
	}

	/* 检查是否是注册函数 */
	if (GetSymbolAsFunc(lpszSymbol, pResult) == STATUS_OK)
	{
		*pSymbolType = SYMBOL_FUNCTION;
		return (STATUS_OK);	
	}

	return (STATUS_ERROR);
}

int ReadSymbolTable(const char *lpszCommand, char lpszSymbol[MAX_SYMBOL_COUNT][MAX_SYMBOL_LENGTH])
{
	int nCount = 0;
	int nIndex = 0;
    size_t i;
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
//			printf("symbol %d = %s\n", nCount, lpszSymbol[nCount]);
			lpszBuffer += nIndex;
		}
		else
		{
			break;
		}
	}

	return (nCount);
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
			printf("ExplainSymbol failed for %s!\n", lpszSymbol[nIndex]);
			break;
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


static char s_lpszSymbol[MAX_SYMBOL_COUNT][MAX_SYMBOL_LENGTH];
static char s_lpszSymbolString[MAX_SYMBOL_COUNT][MAX_SYMBOL_LENGTH];

STATUS ExplainCmd(const char *lpszCmd, SYSWORD *result)
{
	int nSymbolCount;
	SYSWORD tempResult = 0;
	SYSWORD SymbolData;
	SYMBOL_TYPE SymbolType;

	nSymbolCount = ReadSymbolTable(lpszCmd, s_lpszSymbol);
	if (nSymbolCount == 0)
	{
		return (STATUS_ERROR);
	}


	/* if the second symbol is '=', it means a = func(b) */
	if ((nSymbolCount > 2) && (strcmp(s_lpszSymbol[1], "=") == 0))
	{
		/* Explain the first symbol */
		if (ExplainSymbol(s_lpszSymbol[0], s_lpszSymbolString[0], (SYSWORD *)&SymbolData, &SymbolType) == STATUS_ERROR)
		{
			return (STATUS_ERROR);
		}
		if ((SymbolType != SYMBOL_CONTENT) && (SymbolType != SYMBOL_VALUE))
		{
			return (STATUS_ERROR);
		}

		if (RunExpression(s_lpszSymbol, s_lpszSymbolString, nSymbolCount, &tempResult, 2) == STATUS_ERROR)
		{
			return STATUS_ERROR;
		}

		if (SymbolType == SYMBOL_CONTENT)
		{
			SymbolData = *(SYSWORD *)SymbolData;
			*(SYSWORD *)SymbolData = tempResult;
		}
		else if (SymbolType == SYMBOL_VALUE)
		{
			*(SYSWORD *)SymbolData = tempResult;
		}
		else
		{
			return (STATUS_ERROR);
		}

		*result = tempResult;
		return (STATUS_OK);
	}
	else
	{
		if (RunExpression(s_lpszSymbol, s_lpszSymbolString, nSymbolCount, &tempResult, 0) == STATUS_OK)
		{
			*result = tempResult;
			return (STATUS_OK);
		}
		else
		{
			return (STATUS_ERROR);
		}
	}

	return (STATUS_OK);
}

void lkup(char *lpszFuncName)
{
	unsigned int i;

	for (i = 0; i < FUNC_REG_SIZE; i++)
	{
		if ((lpszFuncName == NULL) || (strlen(lpszFuncName) == 0))
		{
			printf("%20s    0x%08lX    %s\n", g_theFuncReg[i].m_lpszName, (SYSWORD)g_theFuncReg[i].m_pAddr, g_theFuncReg[i].m_lpszHelp);
		}
		else if (strstr(g_theFuncReg[i].m_lpszName, lpszFuncName) != NULL)
		{
			printf("%20s    0x%08lX    %s\n", g_theFuncReg[i].m_lpszName, (SYSWORD)g_theFuncReg[i].m_pAddr, g_theFuncReg[i].m_lpszHelp);
		}
	}
}

void help(void)
{
	lkup(NULL);
}