#ifndef _DIAG_H_
#define _DIAG_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __KERNEL__
#define __KERNEL__
#endif
#ifndef MODULE
#define MODULE
#endif

#ifndef _DEFINE_H_
#include "define.h"
#define _DEFINE_H_
#endif

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/version.h>
#include <linux/init.h>
//#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/ioctl.h>
#include <linux/delay.h>
#include <asm/io.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/version.h>
#include <linux/init.h>
//#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/ioctl.h>
#include <linux/delay.h>
#include <asm/io.h>
#include <linux/jiffies.h>
#include <linux/cdev.h>
#include <linux/preempt.h>
#include <linux/types.h>

#ifndef VOS_TYPE
#define VOS_TYPE
typedef 	unsigned long	SYSWORD;

typedef  unsigned long long   UINT64;
typedef  long long     INT64;


typedef  unsigned int   UINT32;
typedef  int     INT32;

typedef  unsigned short UINT16;
typedef  short   INT16;

typedef  unsigned char  UINT8;
typedef  char    INT8;

#endif

#ifndef _DEF_STATUS_
#define _DEF_STATUS_
typedef	    int	STATUS;
#endif

#ifndef  FALSE
#define  FALSE 0
#endif

#ifndef  TRUE
#define  TRUE 1
#endif

#ifndef _DEF_BOOL_
#define _DEF_BOOL_
typedef	    unsigned short	BOOL;
#endif

typedef     STATUS      VOS_STATUS;
#define     VOS_ERROR   -1
#define     VOS_OK      0

#define EOS		'\0'

typedef int 		(*FUNCPTR) (void);	   /* ptr to function returning int */


#define Diag_Output printk

#define DIAG_VERSION "2009-12-7 Version: 1.2"


#ifdef __cplusplus
}
#endif

#endif

