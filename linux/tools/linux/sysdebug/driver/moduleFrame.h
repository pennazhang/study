#ifndef _MODULE_FRAME_H_
#define _MODULE_FRAME_H_

#include "diag.h"

#define MODULE_DRV_MAJOR           3722
#define MODULE_DRV_MINOR           0


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



extern int g_bDebugflag;

#define DRV_DEBUG(fmt, args...)    \
    do { \
         if (g_bDebugflag) \
            printk ("<%s,%d,%s> sysdev: "fmt"\n", __FILE__, __LINE__, __FUNCTION__, ##args); \
    } while(0)          
#endif

