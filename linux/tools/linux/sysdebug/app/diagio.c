/************************************************************************
* diagio.c -- Factory Diagnostic Module
*
* Written By: Percy Zhang UTStarcom (China) Co., Ltd.
*
* Copyright (C) 2006 UTStarcom (China) Co., Ltd.
*
*12/10/2007 - draft
*************************************************************************/
#ifndef __KERNEL__
#define __KERNEL__
#endif
#ifndef MODULE
#define MODULE
#endif

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/version.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/ioctl.h>

#include "diagio.h"

MODULE_DESCRIPTION( "Factory Diagnostic Module" );
MODULE_AUTHOR( "Percy.Zhang" );
MODULE_LICENSE( "GPL" );

static int DiagDevice_Open = 0;

static int diagdrv_ioctl (struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg);
static int diagdrv_open(struct inode *inode, struct file *file);
static int diagdrv_close(struct inode *inode, struct file *file);


static struct file_operations diagdrv_ops = 
{
    .owner	 = THIS_MODULE,
    .ioctl = diagdrv_ioctl,
    .open = diagdrv_open,
    .release = diagdrv_close,
};


/* device iotrl function */
static int diagdrv_ioctl(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg)
{
    int retval = 0;

    /* ============pars check================ */
    {
        int err = 0;
        unsigned int size;
 
        if (IOCTL_IDT72_DEV_MAGIC != _IOC_TYPE(cmd))
        {
            return -EINVAL;
        }

        if (IOCTL_IDT72_DEV_MAXNR < _IOC_NR(cmd)) 
        {
            return -EINVAL;
        }
        
        size = _IOC_SIZE(cmd);
        
        if (_IOC_DIR(cmd) & _IOC_READ)
        {
            err = !access_ok (VERIFY_WRITE, (void __user*)arg, size);
        }
        else if (_IOC_DIR(cmd) & _IOC_WRITE)
        {
            err = !access_ok (VERIFY_READ, (void __user*)arg, size);
        }

        if (err)
        {
            printk("access check error!\n");
            return -EFAULT;
        }
    }

    /* ============pars ok================ */

    switch(cmd)
    {
        /* timeslot connect */
        case IOCTL_IDT72_TS_CON:
        {
            IDT72_CTL_PARAM_T tmpParm;
            
            if (copy_from_user(&tmpParm, (IDT72_CTL_PARAM_T *) arg, sizeof(IDT72_CTL_PARAM_T)))
            {
                DPRINTK("[%s:%d] Get user data failed\n", __FUNCTION__, __LINE__);
                return -1;
            }
            
            retval = idt72TsCon(tmpParm.dSrcHw, tmpParm.dSrcTs, tmpParm.dDestHw, tmpParm.dDestTs);
        }
        break;

        /* timeslot dis-connect */
        case IOCTL_IDT72_TS_DISCON:
        {
            IDT72_CTL_PARAM_T tmpParm;
            
            if (copy_from_user(&tmpParm, (IDT72_CTL_PARAM_T *) arg, sizeof(IDT72_CTL_PARAM_T)))
            {
                DPRINTK("[%s:%d] Get user data failed\n", __FUNCTION__, __LINE__);
                return -1;
            }
            
            retval = idt72TsDisCon(tmpParm.dDestHw, tmpParm.dDestTs);
        }
        break;

        /* highway connect */
        case IOCTL_IDT72_HW_CON:
        {
            IDT72_CTL_PARAM_T tmpParm;
            
            if (copy_from_user(&tmpParm, (IDT72_CTL_PARAM_T *) arg, sizeof(IDT72_CTL_PARAM_T)))
            {
                DPRINTK("[%s:%d] Get user data failed\n", __FUNCTION__, __LINE__);
                return -1;
            }
            
            retval = idt72HwCon(tmpParm.dSrcHw, tmpParm.dDestHw);
        }
        break;

        /* highway dis-connect */
        case IOCTL_IDT72_HW_DISCON:
        {
            IDT72_CTL_PARAM_T tmpParm;
            
            if (copy_from_user(&tmpParm, (IDT72_CTL_PARAM_T *) arg, sizeof(IDT72_CTL_PARAM_T)))
            {
                DPRINTK("[%s:%d] Get user data failed\n", __FUNCTION__, __LINE__);
                return -1;
            }
            
            retval = idt72HwDisCon(tmpParm.dDestHw);
        }
        break;

        /* set timeslot send-pattern */
        case IOCTL_IDT72_SEND_PAT:
        {
            IDT72_CTL_PARAM_T tmpParm;

            if (copy_from_user(&tmpParm, (IDT72_CTL_PARAM_T *) arg, sizeof(IDT72_CTL_PARAM_T)))
            {
                DPRINTK("[%s:%d] Get user data failed\n", __FUNCTION__, __LINE__);
                return -1;
            }
            
            retval = idt72SendPattern(tmpParm.dDestHw, tmpParm.dDestTs, tmpParm.bPattern);
        }
        break;

        /* stop timeslot send-pattern */
        case IOCTL_IDT72_STOP_PAT:
        {
            IDT72_CTL_PARAM_T tmpParm;

            if (copy_from_user(&tmpParm, (IDT72_CTL_PARAM_T *) arg, sizeof(IDT72_CTL_PARAM_T)))
            {
                DPRINTK("[%s:%d] Get user data failed\n", __FUNCTION__, __LINE__);
                return -1;
            }
            
            retval = idt72StopSendPattern(tmpParm.dDestHw, tmpParm.dDestTs);
        }
        break;

        /* get timeslot's connection */
        case IOCTL_IDT72_GET_CON:
        {
            IDT72_CTL_PARAM_T tmpParm;
            unsigned short dSrcHw;
            unsigned short dSrcTs;

            if (copy_from_user(&tmpParm, (IDT72_CTL_PARAM_T *) arg, sizeof(IDT72_CTL_PARAM_T)))
            {
                DPRINTK("[%s:%d] Get user data failed\n", __FUNCTION__, __LINE__);
                return -1;
            }
            
            retval = idt72GetConnect(tmpParm.dDestHw, tmpParm.dDestTs, &dSrcHw, &dSrcTs);
            /*DPRINTK( "[%s:%d] Hw(%d) Ch(%d) connect to Hw(%d) Ch(%d)\n", __FUNCTION__, __LINE__,tmpParm.dDestHw,tmpParm.dDestTs, dSrcHw, dSrcTs);*/

            tmpParm.dSrcHw = dSrcHw;
            tmpParm.dSrcTs = dSrcTs;
            
            if( 0 == retval)
            {
                if (copy_to_user((IDT72_CTL_PARAM_T *)arg , &tmpParm, sizeof(IDT72_CTL_PARAM_T)))
                {
                    DPRINTK("[%s:%d] Send user data failed\n", __FUNCTION__, __LINE__);
                    return -1;
                }
            }
        }
        break;

        /* get timeslot's data */
        case IOCTL_IDT72_GET_DATA:
        {
            IDT72_CTL_PARAM_T tmpParm;
            unsigned short dData;

            if (copy_from_user(&tmpParm, (IDT72_CTL_PARAM_T *) arg, sizeof(IDT72_CTL_PARAM_T)))
            {
                DPRINTK("[%s:%d] Get user data failed\n", __FUNCTION__, __LINE__);
                return -1;
            }
            
            retval = idt72GetTsData(tmpParm.dDestHw, tmpParm.dDestTs, &dData);
            /*DPRINTK( "[%s:%d] Hw(%d) Ch(%d) data = 0x%x\n", __FUNCTION__, __LINE__,tmpParm.dDestHw,tmpParm.dDestTs, dData);*/

            tmpParm.bData = dData;
            
            if( 0 == retval)
            {
                if (copy_to_user((IDT72_CTL_PARAM_T *)arg , &tmpParm, sizeof(IDT72_CTL_PARAM_T)))
                {
                    DPRINTK("[%s:%d] Send user data failed\n", __FUNCTION__, __LINE__);
                    return -1;
                }
            }
        }
        break;

        /* adjust frame input offset register(FOR) bits */
        case IOCTL_IDT72_SET_FOR:
        {
            IDT72_CTL_PARAM_T tmpParm;
            
            if (copy_from_user(&tmpParm, (IDT72_CTL_PARAM_T *) arg, sizeof(IDT72_CTL_PARAM_T)))
            {
                DPRINTK("[%s:%d] Get user data failed\n", __FUNCTION__, __LINE__);
                return -1;
            }
            
            retval = idt72SetForReg(tmpParm.dSrcHw, tmpParm.bData);
        }
        break;

        /* read register data from tsi chip */
        case IOCTL_IDT72_REG_READ:
        {
            IDT72_RW_PARAM_T tmpParm;
            
            if (copy_from_user(&tmpParm, (IDT72_RW_PARAM_T *) arg, sizeof(IDT72_RW_PARAM_T)))
            {
                DPRINTK("[%s:%d] Get user data failed\n", __FUNCTION__, __LINE__);
                return -1;
            }
            
            tmpParm.regValue = idt72ReadShortReg(tmpParm.regAddr);

            if (copy_to_user((IDT72_RW_PARAM_T *)arg , &tmpParm, sizeof(IDT72_RW_PARAM_T)))
            {
                DPRINTK("[%s:%d] Send user data failed\n", __FUNCTION__, __LINE__);
                return -1;
            }
        }
        break;
        
        /* write register data to tsi chip */
        case IOCTL_IDT72_REG_WRITE:
        {
            IDT72_RW_PARAM_T tmpParm;
            
            if (copy_from_user(&tmpParm, (IDT72_RW_PARAM_T *) arg, sizeof(IDT72_RW_PARAM_T)))
            {
                DPRINTK("[%s:%d] Get user data failed\n", __FUNCTION__, __LINE__);
                return -1;
            }
            
            idt72WriteShortReg(tmpParm.regAddr, tmpParm.regValue);
        }
        break;

        /* re-config tsi chip */
        case IOCTL_IDT72_REINIT:
        {
            retval = idt72InitConfig();
        }
        break;
        
        default:
        {
            DPRINTK("[%s:%d] Unsupported iocmd 0x%x\n", __FUNCTION__, __LINE__, cmd);
            return -1;
            break;            
        }
    }
    
    return retval;
}

/* device open */
static int diagdrv_open (struct inode *inode, struct file *file)
{
    int retval = 0;

    if (TsiDevice_Open)
    {
        DPRINTK("[%s:%d] Tsi Device has been opened\n", __FUNCTION__, __LINE__);
        return -EBUSY;
    }

    TsiDevice_Open++;

    if(0 == retval)
    {
        DPRINTK("[%s:%d] Tsi Device open success\n", __FUNCTION__, __LINE__);
    }
    return retval;
}

/* device close */
static int diagdrv_close(struct inode *inode, struct file *file)
{
    int retval = 0;

    TsiDevice_Open--;
    
    DPRINTK("[%s:%d] Tsi Device close success\n", __FUNCTION__, __LINE__);
    return retval;
}

#if 0 /* old way */

/* module init function */
static int idt72drv_mod_init(void)
{
    int retval = 0;    

    /* init driver main struncture */
    retval = idt72InitConfig();
        
    if( 0 != retval )
    {
        DPRINTK("[%s:%d] Tsi Device init config error\n", __FUNCTION__, __LINE__);
        return retval;
    }

    retval = register_chrdev(IDT72_DEV_MAJOR, IDT72_DEV_NAME, &idt72drv_ops);
    
    if( retval < 0)     
    {
        DPRINTK("[%s:%d] Tsi Device registered failed\n", __FUNCTION__, __LINE__);
        return retval;
    }
    else
    {
        DPRINTK("[%s:%d] Tsi Device registered success, Major=%d, Name=%s\n",  __FUNCTION__, __LINE__, IDT72_DEV_MAJOR, IDT72_DEV_NAME);
    }
    return 0;
}

/* module distory function*/  
static void idt72drv_mod_exit(void)
{
    DPRINTK("[%s:%d] Tsi Device unregistered\n", __FUNCTION__, __LINE__);
    unregister_chrdev (IDT72_DEV_MAJOR, IDT72_DEV_NAME);
}  

#else  /* udev */

struct cdev *diag_cdev;
struct class *diag_class;

static int __init diagdrv_mod_init(void)
{
    int err;
    int devno = MKDEV(DIAG_DEV_MAJOR, DIAG_DEV_MINOR);

    /* init driver main struncture */
    #if 1 /* percy */
        if( 0 != DiagInitConfig ())
        {
            printk ( "[%s:%d] 32e1t1 driver init error\n", __FUNCTION__, __LINE__);
            return -1;
        }
    #endif
    
    /* register char device */
    diag_cdev = cdev_alloc();
    cdev_init(diag_cdev, &idt72drv_ops);

    diag_cdev->owner = THIS_MODULE;

    err = cdev_add(diag_cdev, devno, 1);
    if (0 != err)
    {
        DPRINTK("[%s:%d] Tsi module add failed!\n", __FUNCTION__, __LINE__);
        return -1;
    }
    
    /* creating your own class */
    diag_class = class_create(THIS_MODULE, "tsi class");
    
    if(IS_ERR(diag_class)) 
    {
        DPRINTK("[%s:%d] Error: failed in creating class.\n", __FUNCTION__, __LINE__);
        return -1;
    }

    /* register your own device in sysfs, and this will cause udev to create corresponding device node */
    /* struct class_device *class_device_create(struct class *cls, dev_t devt, struct device *device, char *fmt, ...) */
    class_device_create(diag_class, MKDEV(IDT72_DEV_MAJOR, IDT72_DEV_MINOR), NULL, "tsi class device" );

    DPRINTK("[%s:%d] Tsi Device registered success, Major=%d, Name=%s\n",  __FUNCTION__, __LINE__, IDT72_DEV_MAJOR, IDT72_DEV_NAME);
    return 0;
}

static void __exit diagdrv_mod_exit(void)
{
    DPRINTK("[%s:%d] diag Device unregistered\n", __FUNCTION__, __LINE__);

    class_device_destroy(diag_class, MKDEV(IDT72_DEV_MAJOR, IDT72_DEV_MINOR));
    class_destroy(diag_class);

    cdev_del(diag_cdev);
}

#endif

module_init(diagdrv_mod_init);
module_exit(diagdrv_mod_exit);



