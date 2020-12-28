// Common.h: interface for the CCommon class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _COMMON_LIB_H_
#define _COMMON_LIB_H_


#include "typedef.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

extern int readHexString(const char *lpszBuffer, UINT8 *pucData, int nLen);
extern char * formatHexString(UINT8 * pucMsg, int usLen);
void mSleep(int nMs);

ULONG getTickCount(void);
ULONG tickPass(ULONG oldTick);

/* function: replace the char in the existChars with newChar for the defined string.  The size of the file won't changed.
For example:
	char word[] = "hello, my good friend. How are you?";
	replaceString(word, ",.?", '/');
	printf("word = '%s'\n", word);

run Result:
	word = 'hello/ my good friend/ How are you/'  */
void replaceString(char *string, const char *existChars, char newChar);

/*
Delete the black char ' ' at the head. 
For example:
	char word[] = "  hello, world. ";
	trimStringAtHead(word);
	printf("word='%s', len = %d\n", word, strlen(word));

run Result:
	word='hello, world. ', len = 14   */
void trimStringAtHead(char *string);

/* Delete the black char ' ' at the head and the tail of the string. 
For example:
	char word[] = "  hello, world. ";
	trimStringAtTail(word);
	printf("word='%s', len = %d\n", word, strlen(word));

run Result:
	word='  hello, world.', len = 15 */
void trimStringAtTail(char *string);

extern void d(void *pAddr, int nByteCount, int nWidth, int nDisplayOffset);
extern int memchk(UINT8 *pucByte, UINT8 ucData, int count);

bool isFdReadable(int fd, int timeout_in_us = 10);

bool isThreadAlive(pthread_t pid);

UINT32 getPhy32(UINT32 *pulAddr);
UINT32 setPhy32(UINT32 *pulAddr, UINT32 writeval);
UINT64 getPhy64(UINT64 *pulAddr);
UINT64 setPhy64(UINT64 *pulAddr, UINT64 writeval);


void explainUART();


#endif


