/********************************************************************
* diagio.h -- Factory Diagnostic Program ioctl header file
*
* Written By: Percy Zhang UTStarcom(China) Co., Ltd.
*
* Copyright (C) 2006 UTStarcom (China) Co., Ltd.
*
*12/10/2007 - draft
**********************************************************************/
#ifndef __ATCA_DSP_DIAGIO_H__
#define __ATCA_DSP_DIAGIO_H__

#define DIAG_DEV_MAJOR     211             /* Device major number */
#define DIAG_DEV_MINOR     0               /* Device minor number */
#define DIAG_DEV_NAME      "diag"       /* Device name*/

typedef struct _DIAG_CTL_PARAM_T
{
    unsigned int dSrcHw;
    unsigned int dSrcTs;
    unsigned int dDestHw;
    unsigned int dDestTs;
    unsigned short bPattern;
    unsigned short bData;
}DIAG_CTL_PARAM_T, *pDIAG_CTL_PARAM_T;

typedef struct _DIAG_RW_PARAM_T
{
    unsigned int regAddr;
    unsigned short regValue;
}DIAG_RW_PARAM_T, *pDIAG_RW_PARAM_T;


/* ioctl command */
#define IOCTL_DIAG_DEV_MAGIC     DIAG_DEV_MAJOR
#define IOCTL_DIAG_FUNC1            _IOW(IDT72_DEV_MAJOR,0, DIAG_CTL_PARAM_T)
#define IOCTL_DIAG_FUNC2      _IOW(IDT72_DEV_MAJOR,1, DIAG_RW_PARAM_T)
#define IOCTL_DIAG_REINIT              _IO(IDT72_DEV_MAJOR,2)

#define IOCTL_DIAG_DEV_MAXNR      3

#endif


