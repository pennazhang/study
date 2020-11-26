#include "commonLib.h"
#include <sys/time.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/select.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include <ctype.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/mman.h>

char g_lpszFormatString[1024];
static int g_nByteCount = 16, g_nWidth = 1;

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
Function: Read Hex array from string.
For Example:  readHexString ("23 12 9a ba 08 00", aucMac, 6);
*****************************************************/
int readHexString(const char *lpszBuffer, UINT8 *pucData, int nLen)
{
    int i;
    char cData[3];
    UINT ulData;

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

char * formatHexString(UINT8 * pucMsg, int usLen)
{
    int ucCount = 0, i;
    INT8 lpszTemp[] =
        "00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F 10 11 12 13 14 15 16 17 18 19 1A 1B 1C 1D 1E 1F 20 21 22 23 24 25 26 27 28 29 2A 2B 2C 2D 2E 2F 30 31 32 33 34 35 36 37 38 39 3A 3B 3C 3D 3E 3F 40 41 42 43 44 45 46 47 48 49 4A 4B 4C 4D 4E 4F 50 51 52 53 54 55 56 57 58 59 5A 5B 5C 5D 5E 5F 60 61 62 63 64 65 66 67 68 69 6A 6B 6C 6D 6E 6F 70 71 72 73 74 75 76 77 78 79 7A 7B 7C 7D 7E 7F 80 81 82 83 84 85 86 87 88 89 8A 8B 8C 8D 8E 8F 90 91 92 93 94 95 96 97 98 99 9A 9B 9C 9D 9E 9F A0 A1 A2 A3 A4 A5 A6 A7 A8 A9 AA AB AC AD AE AF B0 B1 B2 B3 B4 B5 B6 B7 B8 B9 BA BB BC BD BE BF C0 C1 C2 C3 C4 C5 C6 C7 C8 C9 CA CB CC CD CE CF D0 D1 D2 D3 D4 D5 D6 D7 D8 D9 DA DB DC DD DE DF E0 E1 E2 E3 E4 E5 E6 E7 E8 E9 EA EB EC ED EE EF F0 F1 F2 F3 F4 F5 F6 F7 F8 F9 FA FB FC FD FE FF ";

    ucCount = usLen;
    if (ucCount > 500 / 3)
    {
        ucCount = 500 / 3;
    }

    for (i = 0; i < ucCount; i++)
    {
        memcpy(g_lpszFormatString + 3 * i, lpszTemp + 3 * pucMsg[i], 3);
//          sprintf(g_lpszFormatString + 3 * i, "%02x ", (int)(pucContent[i]));
    }

    g_lpszFormatString[ucCount * 3] = 0;
    return (g_lpszFormatString);
}

void mSleep(int nMs)
{
    usleep(nMs * 1000);
}

ULONG getTickCount(void)
{
    struct timeval tv;
    ULONG ulTick;

    gettimeofday(&tv, NULL);
    ulTick = (ULONG)(tv.tv_sec * 1000 + tv.tv_usec / 1000);
    return (ulTick);
}

ULONG tickPass(ULONG oldTick)
{
    ULONG currentTick = getTickCount();
    ULONG diff = currentTick - oldTick;

    return (diff);
}

void replaceString(char *string, const char *existChars, char newChar)
{
    int i;

    for (i = 0; string[i] != 0; i++)
    {
        if (strchr(existChars, string[i]) != NULL)
        {
            string[i] = newChar;
        }
    }
}

void trimStringAtHead(char *string)
{
	int firstNonBlank = -1, i;

	if (string[0] != ' ')
	{
		return;
	}
	
	for (i = 1; string[i] != 0; i++)
	{
		if (firstNonBlank == -1)
		{
			if (string[i] == ' ')
			{
				continue;
			}
			else
			{
				firstNonBlank = i;
				string[0] = string[i];
			}
		}
		else
		{
			string[i - firstNonBlank] = string[i];
		}	
	}
	string[i - firstNonBlank] = 0;
}

void trimStringAtTail(char *string)
{
	int len = strlen(string);
	for (int i = len - 1; i >= 0; i--)
	{
		if (string[i] == ' ')
		{
			string[i] = 0;
		}
		else
		{
			break;
		}
	}
}


int memchk(UINT8 *pucByte, UINT8 ucData, int count)
{
    int i;
    for (i = 0; i < count; i++)
    {
        if (pucByte[i] != ucData)
        {
        	break;
        }
    }

    return (i);
}

void d(void *pAddr, int nByteCount, int nWidth, int nDisplayOffset)
{
	long nStart, nEnd, i;
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

	nStart = (long)pAddr;
	nEnd = (long)pAddr + nByteCount;

//	printf("pAddr = %p\n", pAddr);
//	printf("(long)pAddr = %08llx\n", (long)pAddr);
//	printf("nStart = %08llx\n", nStart);
//	printf("nWidth = %d\n", nWidth);
//	printf("nCount = %d\n", nByteCount);
	

#if 1
	for (i = nStart; i < nEnd; i+=16)
	{
		printf("%08lx :", i + nDisplayOffset);

		if (nWidth == 1)
		{
			pucData = (UINT8 *)i;
			printf(" %02x  %02x  %02x  %02x  %02x  %02x  %02x  %02x -  %02x  %02x  %02x  %02x  %02x  %02x  %02x  %02x \n",
				pucData[0], pucData[1], pucData[2], pucData[3], pucData[4], pucData[5], pucData[6], pucData[7], 
				pucData[8],	pucData[9], pucData[10], pucData[11], pucData[12], pucData[13], pucData[14], pucData[15]);
		}
		else if (nWidth == 2)
		{
			pusData = (unsigned short *)i;
			printf(" %04x %04x %04x %04x - %04x %04x %04x %04x \n", 
				pusData[0], pusData[1], pusData[2], pusData[3], 
				pusData[4], pusData[5], pusData[6], pusData[7]); 
		}
		else if (nWidth == 4)
		{
			pulData = (UINT *)i;
			printf(" %08x %08x - %08x %08x \n", 
				pulData[0], pulData[1], pulData[2], pulData[3]);
		}
	}
#endif	
}

#define MAP_SIZE 4096UL
#define MAP_MASK (MAP_SIZE - 1)

UINT32 readPhyAddr(UINT32 *pulAddr)
{
	int fd = -1;
	off_t target = (off_t)pulAddr;
	void *map_base = (void *)-1, *virt_addr; 
	UINT32 ulValue = -1;

	if (pulAddr == NULL)
	{
		return (0);
	}

#if 0	
	if ((byteLength == 1) || (byteLength ==2) || (byteLength == 4))
	{
	}
	else
	{
		printf("Invalid byteLength: %d\n", byteLength);
		return false;
	}
	printf("pulAddr = 0x%x, byteLength = %d", pulAddr, byteLength);
#endif	

    if((fd = open("/dev/mem", O_RDWR | O_SYNC)) == -1)
    {
		printf("Failed to open /dev/mem!\n");
		ulValue = -1;
		goto END;
	}
//    printf("/dev/mem opened\n"); 

    /* Map one page */
    map_base = mmap(0, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, target & ~MAP_MASK);
    if(map_base == (void *) -1)
    {
		printf("memory map failed!\n");
		ulValue = -1;
		goto END;
    }
//    printf("Memory mapped at address %p.n", map_base); 

    virt_addr = (UINT8 *)map_base + (target & MAP_MASK);

    ulValue = *((UINT32*) virt_addr);
 //   printf("Value at address 0x%X (%p): 0x%Xn", target, virt_addr, ulValue); 
	
END:
	if (map_base != (void *)-1)
	{
    	munmap(map_base, MAP_SIZE);
	}

	if (fd != -1)
	{
	    close(fd);
	}

	return (ulValue);	
}

UINT32 writePhyAddr(UINT32 *pulAddr, UINT32 writeval)
{
	int fd = -1;
	off_t target = (off_t)pulAddr;
	void *map_base = (void *)-1, *virt_addr; 
	UINT32 ulValue = -1;

#if 0	
	if ((byteLength == 1) || (byteLength ==2) || (byteLength == 4))
	{
	}
	else
	{
		printf("Invalid byteLength: %d\n", byteLength);
		return false;
	}
	printf("pulAddr = 0x%x, byteLength = %d", pulAddr, byteLength);
#endif	

    if((fd = open("/dev/mem", O_RDWR | O_SYNC)) == -1)
    {
		printf("Failed to open /dev/mem!\n");
		ulValue = -1;
		goto END;
	}
//    printf("/dev/mem opened\n"); 

    /* Map one page */
    map_base = mmap(0, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, target & ~MAP_MASK);
    if(map_base == (void *) -1)
    {
		printf("memory map failed!\n");
		ulValue = -1;
		goto END;
    }
//    printf("Memory mapped at address %p.n", map_base); 

    virt_addr = (UINT8 *)map_base + (target & MAP_MASK);

    ulValue = *((UINT32*) virt_addr);
	*((unsigned long *) virt_addr) = writeval;
	ulValue = *((unsigned long *) virt_addr);
	
    printf("Value at address 0x%X (%p): write: 0x%X, read: 0x%Xn", (UINT32)target, virt_addr, writeval, ulValue); 
	
END:
	if (map_base != (void *)-1)
	{
    	munmap(map_base, MAP_SIZE);
	}

	if (fd != -1)
	{
	    close(fd);
	}

	return (ulValue);	
}
	

void set32(UINT32 *pulAddr, UINT32 ulData)
{
	*pulAddr = ulData;
}

void set16(UINT16 *pusAddr, UINT16 usData)
{
	*pusAddr = usData;
}

void set8(UINT8 *pucAddr, UINT8 ucData)
{
	*pucAddr = ucData;
}

UINT32 get32(UINT32 *pulAddr)
{
	return (*pulAddr);
}

UINT16 get16(UINT16 *pusAddr)
{
	return (*pusAddr);
}

UINT8 get8(UINT8 *pucAddr)
{
	return (*pucAddr);
}

bool isFdReadable(int fd, int timeout_in_us)
{
	//we can use select to check the fd is readable.
	int rc;
	fd_set fds;
	struct timeval tv = {0, timeout_in_us};
	
	FD_ZERO(&fds);
	FD_SET(fd, &fds);
	rc = select(fd + 1, &fds, NULL, NULL, &tv);
	if (rc < 0) //error
	{
		return false;
	}
	return FD_ISSET(fd, &fds) ? true : false;
}

bool isThreadAlive(pthread_t pid)
{
	int kill_rc = pthread_kill(pid, 0);
	if (kill_rc == ESRCH)
	{
		// printf("the specified thread did not exists or already quit\n");
		return (false);
	}
	else if(kill_rc == EINVAL)
	{
		//printf("signal is invalid\n");
		return (false);
	}
	else
	{
		//printf("the specified thread is alive\n");
		return (true);
	}
}


typedef struct
{
	int value;
	const char * lpszDescription; 
} DESCRIPTION_ITEM_TYPE;

void explainField(const char * description, int value, int size, DESCRIPTION_ITEM_TYPE *pDescriptionItem, const char * noneMatchString = NULL)
{
	int index;
	if (size <= 0)
	{
		return;
	}
	for (index = 0; index < size; index++)
	{
		if (value == pDescriptionItem[index].value)
		{
			printf("%s - Value:0x%x : %s\n", description, value, pDescriptionItem[index].lpszDescription);
			return;
		}
	}
	
	printf("%s - Value:0x%x (no match) : %s\n", description, value, noneMatchString);
}

DESCRIPTION_ITEM_TYPE g_UCRDescription[] =  
{
	{ 0, "Disable the UART" }, 
	{ 1, "Enable the UART" }, 
};

void explainUCR1(SYSWORD ulPhyAddr, const char *lpszRegDescription)
{
	const char *regDescription = "UART Control Register 1";
	UINT32 ulValue = readPhyAddr((UINT32 *)ulPhyAddr);
	printf("    0x%lx - %s (%s)= 0x%x\n", ulPhyAddr, lpszRegDescription, regDescription, ulValue);
	explainField("        bit 0 - UART Enable", (ulValue >> 0) & 1, sizeof(g_UCRDescription), g_UCRDescription); 
}

DESCRIPTION_ITEM_TYPE g_uartEnableDescription[] =  
{
	{ 0, "No slave address detected" }, 
	{ 1, "Slave address detected" }, 
};

void explainUSR1(SYSWORD ulPhyAddr, const char *lpszRegDescription)
{
	const char *regDescription = "UART Status Register 1";
	UINT32 ulValue = readPhyAddr((UINT32 *)ulPhyAddr);
	printf("    0x%lx - %s (%s)= 0x%x\n", ulPhyAddr, lpszRegDescription, regDescription, ulValue);
	printf("\t bit2-bit 0 - Reserved\n");
	explainField("\t bit3 ", (ulValue >> 3) & 1, sizeof(g_uartEnableDescription), g_uartEnableDescription); 
	explainField("\tbit 0 - UART Enable", (ulValue >> 0) & 1, sizeof(g_uartEnableDescription), g_uartEnableDescription); 
}
	
void explainUART()
{
	explainUSR1(0X30860094, "UART1_USR1");
}


