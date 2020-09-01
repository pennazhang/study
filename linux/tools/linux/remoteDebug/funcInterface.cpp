
#include "debugFrame.h"
#include "common.h"
#include "funcLib.h"
#include <stdio.h>
#include <stdlib.h>
#include <cstring>

extern void lkup(char *lpszFuncName);
extern void help();

void printData(unsigned char *data, int count)
{
	printf("%lx = %s\n", (SYSWORD)data, FormatString(data, count));
}

void printString(char * lpszBuffer)
{
	printf("%s", lpszBuffer);
}

FUNC_MAP g_theFuncReg[] = 
{
		{"help",			(void *)help, 			"void help()"											},	
		{"lkup",			(void *)lkup, 			"void lkup(char *lpszFuncName)"							},	
		{"malloc",			(void *)malloc,			"void * malloc(int size)"								},
		{"free",			(void *)free,			"void free(void *pointer)"								},
		{"strcpy",			(void *)strcpy,			"char * strcpy(char *dest, char *src)"					},
		{"printString", 	(void *)printString,	"void printString(char *lpszBuffer)"					},
		{"readHexString",	(void *)readHexString,  "int readHexString(UINT8 *pucData, char *lpszBuffer, int nLen)" },
		{"d",				(void *)d, 				"void d(void *lpszBuffer,  int nByteCount, int nWidth)" },
		{"crc",				(void *)crc, 			"UINT16 crc(UINT8 *ptr,  int nLen);" 					},		
		{"getTickCount",	(void *)getTickCount, 	"unsigned long getTickCount();" 						},		
		{"runScript",		(void *)runScript, 		"int runScript(char *fileName);" 						},		
};

unsigned int FUNC_REG_SIZE = sizeof(g_theFuncReg) / sizeof(FUNC_MAP);
