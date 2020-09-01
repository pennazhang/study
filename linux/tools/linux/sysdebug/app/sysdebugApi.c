/************************************************************************
* sysdevApi.c -- sysdev api interface for application
*
* Written By: Percy Zhang  @ HuaZhong University Of Science & Technoloty.
*
* Copyright (C) 2003 HuaZhong University Of Science & Technology.
*
*02/06/2003 - draft
*11/28/2009 - Modify for ATCA7367
*************************************************************************/
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h> 	/* ioctl */
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>       /* open */
#include <string.h>
#include "sysdebugApi.h"

BOOL g_bSysdebugOpenFlag = FALSE;
int g_nSysdebugDeviceFd = -1;

VOS_STATUS OpenSysDebugDev(void)
{
    if(g_bSysdebugOpenFlag)
    {
//        printf("[%s:%d] device has been opened\n", __FUNCTION__, __LINE__);
        return (VOS_OK);
    }

	g_nSysdebugDeviceFd = open("/dev/sysdev", O_RDWR);

    if (g_nSysdebugDeviceFd < 0)
    {
//        printf("[%s:%d] open device error\n", __FUNCTION__, __LINE__);
        return (VOS_ERROR);
    }
    else
    {
        g_bSysdebugOpenFlag = TRUE;
    }
    return (VOS_OK);
}


VOS_STATUS CloseSysDebugDev(void)
{
    if(!g_bSysdebugOpenFlag)
    {
//        printf("[%s:%d] device not opened\n", __FUNCTION__, __LINE__);
        return (VOS_OK);
    }
    else
    {

        close(g_nSysdebugDeviceFd);
        
        g_bSysdebugOpenFlag = FALSE;
        g_nSysdebugDeviceFd = -1;
    }
    return (VOS_OK);
}

BOOL IsSysDebugOpen()
{
    if ((g_bSysdebugOpenFlag == TRUE) && (g_nSysdebugDeviceFd >= 0))
    {
        return (TRUE);
    }
    else
    {
        return (FALSE);
    }
}

VOS_STATUS CopyMemFromUserToKernel(char *pKernelDest, char *pUserSrc, int nSize)
{
	COPY_MEM_STRUCT CopyStruct;
	 
	if(IsSysDebugOpen() != TRUE)
    {
    	if (OpenSysDebugDev() != VOS_OK)
    	{
    		printf("sysDebug Device can not be opened!\n");
	        return (VOS_ERROR);
    	}
    }

	/* Fill the struct */
	CopyStruct.m_pKernelAddr = (UINT32)pKernelDest;
	CopyStruct.m_nSize = nSize;
	CopyStruct.m_pUserMem = pUserSrc;

    if(ioctl(g_nSysdebugDeviceFd, SYSDEV_IOC_COPY_MEM_FROM_USER_TO_KERNEL, &CopyStruct) < 0)
	{
		printf("[%s:%d] sysCommandRun called failed for SYSDEV_IOC_COPY_MEM_FROM_USER_TO_KERNEL return failed\n", __FUNCTION__, __LINE__);
		CloseSysDebugDev();
		return (VOS_ERROR);
	}
	else
	{
		CloseSysDebugDev();	
		return (VOS_OK);
	}
}

VOS_STATUS CopyMemFromKernelToUser(char *pUserAddr, char *pKernelAddr, int nSize)
{
	COPY_MEM_STRUCT CopyStruct;
	 
	if(IsSysDebugOpen() != TRUE)
    {
    	if (OpenSysDebugDev() != VOS_OK)
    	{
    		printf("sysDebug Device can not be opened!\n");
	        return (VOS_ERROR);
    	}
    }

	/* Fill the struct */
	CopyStruct.m_pKernelAddr = (UINT32)pKernelAddr;
	CopyStruct.m_nSize = nSize;
	CopyStruct.m_pUserMem = pUserAddr;

    if(ioctl(g_nSysdebugDeviceFd, SYSDEV_IOC_COPY_MEM_FROM_KERNEL_TO_USER, &CopyStruct) < 0)
	{
		printf("[%s:%d] sysCommandRun called failed for SYSDEV_IOC_COPY_MEM_FROM_KERNEL_TO_USER return failed\n", __FUNCTION__, __LINE__);
		CloseSysDebugDev();
		return (VOS_ERROR);
	}
	else
	{
		CloseSysDebugDev();	
		return (VOS_OK);
	}
}

VOS_STATUS CopyMemFromUserToPhy(char *pPhyDest, char *pUserSrc, int nSize)
{
	COPY_MEM_STRUCT CopyStruct;
	 
	if(IsSysDebugOpen() != TRUE)
    {
    	if (OpenSysDebugDev() != VOS_OK)
    	{
    		printf("sysDebug Device can not be opened!\n");
	        return (VOS_ERROR);
    	}
    }

	/* Fill the struct */
	CopyStruct.m_pKernelAddr = (UINT32)pPhyDest;
	CopyStruct.m_nSize = nSize;
	CopyStruct.m_pUserMem = pUserSrc;

    if(ioctl(g_nSysdebugDeviceFd, SYSDEV_IOC_COPY_MEM_FROM_USER_TO_PHY, &CopyStruct) < 0)
	{
		printf("[%s:%d] sysCommandRun called failed for SYSDEV_IOC_COPY_MEM_FROM_USER_TO_PHY return failed\n", __FUNCTION__, __LINE__);
		CloseSysDebugDev();
		return (VOS_ERROR);
	}
	else
	{
		CloseSysDebugDev();	
		return (VOS_OK);
	}
}

VOS_STATUS CopyMemFromPhyToUser(char *pUserAddr, char *pPhyAddr, int nSize)
{
	COPY_MEM_STRUCT CopyStruct;
	 
	if(IsSysDebugOpen() != TRUE)
    {
    	if (OpenSysDebugDev() != VOS_OK)
    	{
    		printf("sysDebug Device can not be opened!\n");
	        return (VOS_ERROR);
    	}
    }

	/* Fill the struct */
	CopyStruct.m_pKernelAddr = (UINT32)pPhyAddr;
	CopyStruct.m_nSize = nSize;
	CopyStruct.m_pUserMem = pUserAddr;

    if(ioctl(g_nSysdebugDeviceFd, SYSDEV_IOC_COPY_MEM_FROM_PHY_TO_USER, &CopyStruct) < 0)
	{
		printf("[%s:%d] sysCommandRun called failed for SYSDEV_IOC_COPY_MEM_FROM_USER_TO_PHY return failed\n", __FUNCTION__, __LINE__);
		CloseSysDebugDev();
		return (VOS_ERROR);
	}
	else
	{
		CloseSysDebugDev();	
		return (VOS_OK);
	}
}


int sysCommandRun(char *lpszCmd, SYSWORD *pResult)
{
	sysdev_args_t sysdevArgs;

	if(IsSysDebugOpen() != TRUE)
    {
    	if (OpenSysDebugDev() != VOS_OK)
    	{
    		printf("sysDebug Device can not be opened!\n");
	        return (VOS_ERROR);
    	}
    }

    if (strlen(lpszCmd) >= 256)
    {
        printf("[%s:%d] sysCommandRun called failed for lpszCmd is too long\n", __FUNCTION__, __LINE__);
        return -1;
    }
    
    sysdevArgs.m_lpszCmd = lpszCmd;
    sysdevArgs.m_nCmdLen = strlen(lpszCmd);
    
    if(ioctl(g_nSysdebugDeviceFd, SYSDEV_IOC_RUN, &sysdevArgs) < 0)
    {
        printf("[%s:%d] sysCommandRun called failed for SYSDEV_IOC_RUN return failed\n", __FUNCTION__, __LINE__);
		CloseSysDebugDev();
        return -1;
    }

//    printf("sysdevDevRun Run successfully, result = %d\n", sysdevArgs.m_nResult);
    if (pResult != NULL)
    {
        *pResult = sysdevArgs.m_nResult;
    }

	CloseSysDebugDev();	
	return 0;
}

#if 0


int main(void)
{
    int nFd;
    int iRet, nResult;

    nFd = sysdevDevOpen();
    if (nFd == -1)
    {
        printf("sysdev open failed!\n");
        return -1;
    }

    iRet = sysRun("Add 1 2", &nResult);
    if (iRet != 0)
    {
        printf("sysRun failed!\n");
        sysdevClose();
        return -1;
    }

    printf("sysRun OK, result  = %d!\n", nResult);
    sysdevClose();

    return (0);
}
#endif







