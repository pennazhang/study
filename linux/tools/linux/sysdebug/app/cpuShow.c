#ifdef __cplusplus
extern "C" {
#endif

#include "cpuShow.h"

extern void mSleep(int nMs);
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
    SYSWORD uiUser;
    SYSWORD uiNice;
    SYSWORD uiSystem;
    SYSWORD uiIdle;

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
              sscanf(text, "%s %ld %ld %ld %ld", lpszCPUName, &uiUser, &uiNice, &uiSystem, &uiIdle);
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

	if (nCount > 100)
	{
		nCount = 100;
	}
	if (nCount <= 0)
	{
		nCount = 5;
	}

	nCPUCount = GetCurrentCPUInfo(32, CpuUtilityInfo1);
	Diag_Output("Period : %d second, Time: %d, cpu number: %d\n", nPeriodInSecond, nCount, nCPUCount);
	for (i = 0; i < nCPUCount; i++)
	{	
		sprintf(lpszTemp, "        cpu%d        ", i);
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
			int uiUserDiff = (int)(CpuUtilityInfo2[j].m_uiUser - CpuUtilityInfo1[j].m_uiUser);
			int uiNiceDiff = (int)(CpuUtilityInfo2[j].m_uiNice - CpuUtilityInfo1[j].m_uiNice);
			int uiSystemDiff = (int)(CpuUtilityInfo2[j].m_uiSystem - CpuUtilityInfo1[j].m_uiSystem);
			int uiIdleDiff = (int)(CpuUtilityInfo2[j].m_uiIdle - CpuUtilityInfo1[j].m_uiIdle);
			int nCountDiff = uiUserDiff + uiNiceDiff + uiSystemDiff + uiIdleDiff;
			if (nCountDiff <= 0)
			{
				break;
			}
			
			sprintf(lpszTemp, " %3d  %3d  %3d  %3d |", uiUserDiff * 100 / nCountDiff, uiSystemDiff * 100 / nCountDiff, uiNiceDiff * 100 / nCountDiff, uiIdleDiff * 100 / nCountDiff);
			strcat(lpszBuffer, lpszTemp);

			memcpy((void *)(CpuUtilityInfo1 + j), (void *)(CpuUtilityInfo2 + j), sizeof(CPU_UTILITY_INFO));
		}
		Diag_Output("%s\n", lpszBuffer);
	}
}

#ifdef __cplusplus
}
#endif

