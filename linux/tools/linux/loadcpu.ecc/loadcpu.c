/* $Id$ */

/*MH********************* MODULE HEADER *********************
**
**	MODULE NAME:	loadcpu.c
**
**	PROJECT:		Advanced High Availability (AHA) Platform
**					System Configuration/Event Manager (SCEM)
**					Software.
**
**	DESCRIPTION:	This file contains the loadcpu CPU loading program.
**
**	CONTENTS:		It contains the following functions:
**
**						main()
**
**  COPYRIGHT:		(C) COPYRIGHT 2003, MOTOROLA
**					ALL RIGHTS RESERVED
**
**					THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF MOTOROLA
**					The copyright notice above does not evidence any actual
**					or intended publication of such source code.
**
**	NOTES:			This file uses 4 spaces per tab.
**
************************************************************/

/*
**  Include Files
*/

#include <sys/time.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include "pthread.h"


/*FH********************* FUNCTION HEADER *********************
**
**	FUNCTION NAME:	main()
**
**	DESCRIPTION:	Main entry point for the loadcpu program.
**
**	PARAMETERS:
**
**		Input:		Argc		- Number of command line options,
**
**					Argv		- Pointer to command line options.
**
**						[1]		- The percentage cpu loading required.
**
**						[2]     - Sample time in which to achieve the loading.
**								optional parameter.
**
**		Output:		None.
**
**		Returns:	0 on success and 1 on error.
**
**	NOTES: This program will busy itself for the specified percent of the 
**  specified sample time.  The remainder of the sample time is spent sleeping.
**  This cycle is repeated until the program exits.
**
************************************************************/

unsigned long sampleTime=100, loadTarget=0;
int exitCode=0;

void mSleep(int nMs)
{
	usleep(nMs * 1000);
}

typedef unsigned int UINT;
#define Diag_Output printf

typedef struct
{
	UINT	m_uiUser;
	UINT 	m_uiNice;
	UINT	m_uiSystem;
	UINT 	m_uiIdle; 
}CPU_UTILITY_INFO;

void * LoopProc(void * pVoid)
{
	struct timeval curTime, markTime; /* this is in sec and micro sec */
	struct timespec sleepTime; /* sleep time is in sec and nano sec */

	/* loadTarget is in percent, sampleTime is in msec, delay is in usec */
	unsigned long   delay;

	sleepTime.tv_sec=0;
	sleepTime.tv_nsec = sampleTime * 10000 * (100 - loadTarget);

	gettimeofday(&markTime, NULL);
	while (!exitCode)
	{
		if (gettimeofday(&curTime, NULL))
		{
			printf("gettimeofday failed: error (%d)  %s\n", 
						errno, strerror(errno));
			exitCode = 0;
		}
		else
		{
			if (curTime.tv_sec > markTime.tv_sec)
			{
				delay = curTime.tv_usec + 1000000L - markTime.tv_usec;
			}
			else
			{
				delay = curTime.tv_usec - markTime.tv_usec;
			}

		
			if (delay >= (sampleTime * 1000 * loadTarget / 100))
			{
				if (nanosleep(&sleepTime, NULL) == -1)
				{
					printf("Nanosleep crapped out early!\n");
				}
				if (gettimeofday(&markTime, NULL))
				{
					printf("gettimeofday failed: error (%d)  %s\n", 
								errno, strerror(errno));
					exitCode = 1;
				}
			}
		}
	}	
	return (NULL);
}

void CreateThread(int nCount)
{
	pthread_t pid;

	if (nCount >= 1)
	{
		pthread_create(&pid, NULL, LoopProc, NULL);
		nCount--;
		CreateThread(nCount);
	}
	else
	{
		return;
	}

}

int GetCpuCount()
{
    char lpszCPUName[21] = "cpu";
    char text[201];

    FILE *fp;
	int nCpuId = 0;
	sprintf(lpszCPUName, "cpu%d", nCpuId);

    fp = fopen("/proc/stat", "r");
	if (fp == NULL)
	{
		return (-1);
	}
	
    while (fgets(text, 200, fp))
    {
        if (strstr(text, lpszCPUName))
        {
			  nCpuId++;
			  sprintf(lpszCPUName, "cpu%d", nCpuId);
        }
    }
    fclose(fp);

    return(nCpuId);
}

int GetCurrentCPUInfo(int nCPUCount, CPU_UTILITY_INFO *pCPUUtilityInfo)
{
    UINT uiUser;
    UINT uiNice;
    UINT uiSystem;
    UINT uiIdle;

    char lpszCPUName[21] = "cpu";
    char text[201];

    FILE *fp;
	int nCpuId = 0;
	sprintf(lpszCPUName, "cpu%d", nCpuId);

    fp = fopen("/proc/stat", "r");
	if (fp == NULL)
	{
		return (-1);
	}
	
    while (fgets(text, 200, fp))
    {
    	if (nCpuId >= nCPUCount)
    	{
			break;
		}
        if (strstr(text, lpszCPUName))
        {
              sscanf(text, "%s %d %d %d %d", lpszCPUName, &uiUser, &uiNice, &uiSystem, &uiIdle);
			  pCPUUtilityInfo[nCpuId].m_uiUser = uiUser;
			  pCPUUtilityInfo[nCpuId].m_uiNice = uiNice;
			  pCPUUtilityInfo[nCpuId].m_uiSystem = uiSystem;
			  pCPUUtilityInfo[nCpuId].m_uiIdle = uiIdle;
			  nCpuId++;
			  sprintf(lpszCPUName, "cpu%d", nCpuId);
        }
    }
    fclose(fp);

    return(nCpuId);
}

void ShowCPUUtility(int nPeriodInSecond, int nCount)
{
	int i, j, nCPUCount;
	CPU_UTILITY_INFO CpuUtilityInfo1[32];
	CPU_UTILITY_INFO CpuUtilityInfo2[32];
	char lpszBuffer[512] = "   ";
	char lpszBuffer1[512] = "   ";
	char lpszTemp[40];

	if (nPeriodInSecond > 60)
	{
		nPeriodInSecond = 60;
	}
	if (nPeriodInSecond <= 0)
	{
		nPeriodInSecond = 1;
	}

	if (nCount <= 0)
	{
		nCount = 5;
	}

	Diag_Output("Period : %d second, Time: %d\n", nPeriodInSecond, nCount);
	nCPUCount = GetCurrentCPUInfo(32, CpuUtilityInfo1);
	for (i = 0; i < nCPUCount; i++)
	{	
		sprintf(lpszTemp, "        cpu%2d        ", i);
		strcat (lpszBuffer, lpszTemp);
		sprintf(lpszTemp, " user sys nice idle |");
		strcat (lpszBuffer1, lpszTemp);
	}
	Diag_Output("%s\n", lpszBuffer);
	Diag_Output("%s\n", lpszBuffer1);
	for (i = 0; i < nCount; i++)
	{
		mSleep(nPeriodInSecond * 1000);
		GetCurrentCPUInfo(32, CpuUtilityInfo2);
		sprintf(lpszBuffer, "%3d", i + 1);
		for (j = 0; j < nCPUCount; j++)
		{
			int uiUserDiff = CpuUtilityInfo2[j].m_uiUser - CpuUtilityInfo1[j].m_uiUser;
			int uiNiceDiff = CpuUtilityInfo2[j].m_uiNice - CpuUtilityInfo1[j].m_uiNice;
			int uiSystemDiff = CpuUtilityInfo2[j].m_uiSystem - CpuUtilityInfo1[j].m_uiSystem;
			int uiIdleDiff = CpuUtilityInfo2[j].m_uiIdle - CpuUtilityInfo1[j].m_uiIdle;
			int nCountDiff = uiUserDiff + uiNiceDiff + uiSystemDiff + uiIdleDiff;
			if (nCountDiff <= 0)
			{
				break;
			}
			
			sprintf(lpszTemp, "%4d  %3d  %3d  %3d |", uiUserDiff * 100 / nCountDiff, uiSystemDiff * 100 / nCountDiff, uiNiceDiff * 100 / nCountDiff, uiIdleDiff * 100 / nCountDiff);
			strcat(lpszBuffer, lpszTemp);

			memcpy((void *)(CpuUtilityInfo1 + j), (void *)(CpuUtilityInfo2 + j), sizeof(CPU_UTILITY_INFO));
		}
		Diag_Output("%s\n", lpszBuffer);
	}
}


int main(int argc, char *argv[])
{
	int nCPUCount;

	/*
	** Validat arguments.  Must have load target and sample time
	*/

	if (argc < 2)
	{
		printf("usage: %s <load target %%> [<sample time in ms>]\n",
				argv[0]);
		exit(1);
	}

	loadTarget = atoi(argv[1]);
	if (loadTarget <= 0 || loadTarget > 100)
	{
		printf("Load target must be a percent between 1 and 100, inclusive.\n");
		exit(1);
	}
	
	if (argc > 2)
	{
		sampleTime = atoi(argv[2]);
		if (sampleTime < 0 || sampleTime > 2000)
		{
			printf("Sample time must be between 1 and 2000 ms.\n");	
			exit(1);
		}
	}

	nCPUCount = GetCpuCount();
	CreateThread(nCPUCount);

	ShowCPUUtility(1, 10000000);
	return (exitCode);
}
