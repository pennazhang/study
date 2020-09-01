#ifndef __SYSDEV_API_H__
#define __SYSDEV_API_H__

#include "diag.h"
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>       /* open */
#include <sys/ioctl.h> /* ioctl */

typedef struct
{
    char *m_lpszCmd;
    int m_nCmdLen;
    SYSWORD m_nResult;
}sysdev_args_t;

typedef struct
{
	UINT32 m_pKernelAddr;
	int m_nSize;
	char *m_pUserMem; 
}COPY_MEM_STRUCT;

#define SYSDEV_IOC_MAGIC                   'c'

#define SYSDEV_IOC_BASE                 _IO(SYSDEV_IOC_MAGIC, 0)
#define SYSDEV_IOC_RUN                  _IOWR(SYSDEV_IOC_MAGIC, 1, sysdev_args_t)
#define SYSDEV_IOC_COPY_MEM_FROM_USER_TO_KERNEL  _IOWR(SYSDEV_IOC_MAGIC, 2, COPY_MEM_STRUCT)
#define SYSDEV_IOC_COPY_MEM_FROM_KERNEL_TO_USER  _IOWR(SYSDEV_IOC_MAGIC, 3, COPY_MEM_STRUCT)

#define SYSDEV_IOC_COPY_MEM_FROM_USER_TO_PHY  _IOWR(SYSDEV_IOC_MAGIC, 4, COPY_MEM_STRUCT)
#define SYSDEV_IOC_COPY_MEM_FROM_PHY_TO_USER  _IOWR(SYSDEV_IOC_MAGIC, 5, COPY_MEM_STRUCT)

#define SYSDEV_IOC_MAXNR               5


VOS_STATUS OpenSysDebugDev(void);
VOS_STATUS CloseSysDebugDev(void);
BOOL IsSysDebugOpen();
int sysCommandRun(char *lpszCmd, SYSWORD *pResult);


VOS_STATUS CopyMemFromUserToKernel(char *pKernelDest, char *pUserSrc, int nSize);
VOS_STATUS CopyMemFromKernelToUser(char *pUserDest, char *pKernelSrc, int nSize);
VOS_STATUS CopyMemFromUserToPhy(char *pPhyDest, char *pUserSrc, int nSize);
VOS_STATUS CopyMemFromPhyToUser(char *pUserAddr, char *pPhyAddr, int nSize);



#endif
