//####
//# Legal Notice :
//#    Copyright, AMX LLC, 2009
//#    Private, proprietary information, the sole property of AMX LLC.  The
//#    contents, ideas, and concepts expressed herein are not to be disclosed
//#    except within the confines of a confidential relationship and only
//#    then on a need to know basis.
//#
//#    Any entity in possession of this AMX Software shall not, and shall not
//#    permit any other person to, disclose, display, loan, publish, transfer
//#    (whether by sale, assignment, exchange, gift, operation of law or
//#    otherwise), license, sublicense, copy, or otherwise disseminate this
//#    AMX Software.
//#
//#    This AMX Software is owned by AMX and is protected by United States
//#    copyright laws, patent laws, international treaty provisions, and/or
//#    state of Texas trade secret laws.
//#
//#    Portions of this AMX Software may, from time to time, include
//#    pre-release code and such code may not be at the level of performance,
//#    compatibility and functionality of the final code. The pre-release code
//#    may not operate correctly and may be substantially modified prior to
//#    final release or certain features may not be generally released. AMX is
//#    not obligated to make or support any pre-release code. All pre-release
//#    code is provided "as is" with no warranties.
//#
//#    This AMX Software is provided with restricted rights. Use, duplication,
//#    or disclosure by the Government is subject to restrictions as set forth
//#    in subparagraph (1)(ii) of The Rights in Technical Data and Computer
//#    Software clause at DFARS 252.227-7013 or subparagraphs (1) and (2) of
//#    the Commercial Computer Software Restricted Rights at 48 CFR 52.227-19,
//#    as applicable.
//####


#ifndef _COMMON_LIB_H_
#define _COMMON_LIB_H_


#include "typeDef.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

extern int readHexString(const char *lpszBuffer, UINT8 *pucData, int nLen);
extern char * formatHexString(UINT8 * pucMsg, int usLen);
void mSleep(int nMs);
void m(void *ptr, int nWidth);

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

UINT32 readPhyAddr(UINT32 *pulAddr);
UINT32 writePhyAddr(UINT32 *pulAddr, UINT32 writeval);

void help();

void explainUART();

void set32(UINT32 *pulAddr, UINT32 ulData);

void set16(UINT16 *pusAddr, UINT16 usData);

void set8(UINT8 *pucAddr, UINT8 ucData);

#endif


