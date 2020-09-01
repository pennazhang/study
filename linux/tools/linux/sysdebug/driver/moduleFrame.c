#include "moduleFrame.h"
#include "FuncReg.h"
#include "Common.h"
#include<linux/slab.h>

#define MODULE_DRV_NAME "sysdev"
#define GLOBALMEM_SIZE	0x1000	/*全局内存最大4K字节*/

static int globalmem_major = MODULE_DRV_MAJOR;

/*globalmem设备结构体*/
struct globalmem_dev                                     
{                                                        
  	struct cdev cdev; /*cdev结构体*/                       
  	unsigned char mem[GLOBALMEM_SIZE]; /*全局内存*/        
};

struct globalmem_dev *globalmem_devp; /*设备结构体指针*/

#define DRV_GET_USERD_DATA(ioPars, command, args) \
    do{\
        memset (ioPars, 0, _IOC_SIZE(command)); \
        if (copy_from_user((unsigned char*)ioPars, (void *) args, _IOC_SIZE(command)))\
        {\
	        DRV_DEBUG ("get user data failed\n");\
            return -EFAULT;\
        }\
    }while(0)

#define DRV_SET_USERD_DATA(to, from, size) \
    do{\
        memset ((unsigned char*)to, 0, size); \
        if (copy_to_user((unsigned char*)to, (void *)from, size))\
        {\
	        DRV_DEBUG ("set user data failed\n");\
            return -EFAULT;\
        }\
    }while(0)

int g_bDebugflag = 1;
static int g_nOpenNum = 0;


static int drv_open (struct inode *inode, struct file *file)
{
	if (g_nOpenNum)
	{
	    DRV_DEBUG ("sysdev has opened! Please close fd.");
        return -EEXIST;
	}
    else
    {
	    DRV_DEBUG ("sysdev open!");
	    g_nOpenNum++;
    }
	
	return 0;
}


static char lpszSymbol[MAX_SYMBOL_COUNT][MAX_SYMBOL_LENGTH];
static char lpszCmd[256];

int drv_ioctl (struct inode *inode, struct file *file, unsigned int cmd, 
                  unsigned long arg)
{
    sysdev_args_t stArg, *pArg;
	COPY_MEM_STRUCT stMemArg;
	SYSWORD ulResult;
	int nSymbolCount;
	void *pVirtualAddr;

    DRV_DEBUG ("Input args<0x%x> verify....", (unsigned int)arg);
/* ============pars check================ */
    {
        if (SYSDEV_IOC_MAGIC != _IOC_TYPE(cmd)) return -EINVAL;
        if (SYSDEV_IOC_MAXNR < _IOC_NR(cmd)) return -EINVAL;
#if 0
        size = _IOC_SIZE(cmd);
        if (_IOC_DIR(cmd) & _IOC_READ)
        {
            err = !access_ok (VERIFY_WRITE, (void __user*)arg, size);
            if (err)
                DRV_DEBUG ("memory area write test failed!arg %#lX;size %ld", arg, size);
        }
        else if (_IOC_DIR(cmd) & _IOC_WRITE)
        {
            err = !access_ok (VERIFY_READ, (void __user*)arg, size);
            if (err)
                DRV_DEBUG ("memory area read test failed!arg %#lX;size %ld", arg, size);
        }
        if (err)
            return err;
#endif        
    }
/* ============pars ok================ */
       
    switch (cmd)
    {
    case SYSDEV_IOC_RUN:
        {
            if (copy_from_user((unsigned char*)&stArg, (void *)arg, sizeof(sysdev_args_t)))
            {
                return (-EFAULT);
            }
            if (stArg.m_nCmdLen >= 256)
            {
                return (-EFAULT);
            }
            if (copy_from_user((void *)lpszCmd, stArg.m_lpszCmd, stArg.m_nCmdLen))
            {
                return (-EFAULT);
            }
            lpszCmd[stArg.m_nCmdLen] = 0;
            {
        		/* 判断是否有内容 */
        		nSymbolCount = ReadSymbolTable(lpszCmd, lpszSymbol);
        		if (nSymbolCount == 0)
        		{
//        		    printk("No symbol found!\n");
        		    return (-1);
        		}
        		
        		/* 解释输入是否正确 */
        		if (ExplainCmd(lpszCmd, &ulResult) == VOS_OK)
        		{
//        			printk("\nExecute function successfully, Result = 0x%lX = %ld\n", ulResult, ulResult);
        		}
        		else
        		{
//        			printk("Execute function failed!\n");
        			return (-1);
        		}
        	}
            pArg = (sysdev_args_t *)arg;
            if (copy_to_user((unsigned char *)(&(pArg->m_nResult)), (void *)&ulResult, sizeof(SYSWORD)))
            {
                printk("copy_to_user failed!\n");
                return (-EFAULT);
            }

            return (0);
        }
        break;
	case SYSDEV_IOC_COPY_MEM_FROM_USER_TO_KERNEL:
	{
		if (copy_from_user((unsigned char*)&stMemArg, (void *)arg, sizeof(COPY_MEM_STRUCT)))
		{
			return (-EFAULT);
		}
		if (copy_from_user((void *)stMemArg.m_pKernelAddr, stMemArg.m_pUserMem, stMemArg.m_nSize))
		{
			return (-EFAULT);
		}
		return (0);
	}
	case SYSDEV_IOC_COPY_MEM_FROM_KERNEL_TO_USER:
	{
		
		DRV_DEBUG("copy_from_user: 0x%x, 0x%x, %d\n", (UINT32)&stMemArg, (UINT32)arg, sizeof(COPY_MEM_STRUCT));
		if (copy_from_user((unsigned char*)&stMemArg, (void *)arg, sizeof(COPY_MEM_STRUCT)))
		{
			return (-EFAULT);
		}

		DRV_DEBUG("copy_to_user: 0x%x, 0x%x, %d\n", (UINT32)stMemArg.m_pUserMem, (UINT32)stMemArg.m_pKernelAddr, sizeof(COPY_MEM_STRUCT));
		if (copy_to_user(stMemArg.m_pUserMem, (void *)stMemArg.m_pKernelAddr, stMemArg.m_nSize))
		{
			return (-EFAULT);
		}
		return (0);
	}
		
	case SYSDEV_IOC_COPY_MEM_FROM_USER_TO_PHY:
	{
		if (copy_from_user((unsigned char*)&stMemArg, (void *)arg, sizeof(COPY_MEM_STRUCT)))
		{
			return (-EFAULT);
		}
		pVirtualAddr = MapPhyToVirtual(stMemArg.m_pKernelAddr, stMemArg.m_nSize, FALSE);
		if (pVirtualAddr == NULL)
		{
			return (-EFAULT);			
		}
		copy_from_user(pVirtualAddr, stMemArg.m_pUserMem, stMemArg.m_nSize);
		UnmapPhyToVirtual(pVirtualAddr, stMemArg.m_nSize);
		return (0);
	}
		
    default:
        DRV_DEBUG ("[sysdev] unknow cmd<%#x>", cmd);
        break;
    }
	return -1;

}

static int drv_release (struct inode *inode, struct file *file)
{
    if (g_nOpenNum)
        g_nOpenNum--;

    if (g_nOpenNum == 0)
		DRV_DEBUG ("Driver Release OK!\n");
    return 1;
}


static struct file_operations drv_fops = {
	.owner		= THIS_MODULE,
	.read		= NULL,
	.write		= NULL,
    .unlocked_ioctl		       = drv_ioctl,
	.open		= drv_open,
	.release	       = drv_release,
};

struct cdev *g_pDevice;
struct class *g_pClass;


/*初始化并注册cdev*/
static void globalmem_setup_cdev(struct globalmem_dev *dev, int index)
{
  int err, devno = MKDEV(globalmem_major, index);

  cdev_init(&dev->cdev, &drv_fops);
  dev->cdev.owner = THIS_MODULE;
  dev->cdev.ops = &drv_fops;
  err = cdev_add(&dev->cdev, devno, 1);
  if (err)
    printk(KERN_NOTICE "Error %d adding LED%d", err, index);
}

/*设备驱动模块加载函数*/
static int drv_init(void)
{
	int result;
	dev_t devno = MKDEV(MODULE_DRV_MAJOR, MODULE_DRV_MINOR);

	/* 申请设备号*/
	if (globalmem_major)
    result = register_chrdev_region(devno, 1, "sysdev");
	else  /* 动态申请设备号 */
	{
		result = alloc_chrdev_region(&devno, 0, 1, "sysdev");
		globalmem_major = MAJOR(devno);
	}  
	if (result < 0)
		return result;
    
	/* 动态申请设备结构体的内存*/
	globalmem_devp = kmalloc(sizeof(struct globalmem_dev), GFP_KERNEL);
	if (!globalmem_devp)    /*申请失败*/
	{
		result =  - ENOMEM;
		goto fail_malloc;
	}
	memset(globalmem_devp, 0, sizeof(struct globalmem_dev));
  
  	globalmem_setup_cdev(globalmem_devp, 0);
    DRV_DEBUG("[%s:%d] dev_init OK\n", __FUNCTION__, __LINE__);
	g_bDebugflag = FALSE;
  	return 0;

fail_malloc: 
    DRV_DEBUG("[%s:%d] dev_init failed\n", __FUNCTION__, __LINE__);
	unregister_chrdev_region(devno, 1);
  	return result;
}

/*模块卸载函数*/
void drv_exit(void)
{
    DRV_DEBUG("[%s:%d] Device unregistered\n", __FUNCTION__, __LINE__);
	cdev_del(&globalmem_devp->cdev);   /*注销cdev*/
	kfree(globalmem_devp);     /*释放设备结构体内存*/
	unregister_chrdev_region(MKDEV(globalmem_major, 0), 1); /*释放设备号*/
}



/* module init paramter */
module_param(g_bDebugflag, int, S_IRUGO);
MODULE_PARM_DESC(g_bDebugflag, "Driver debug flag.");


module_init(drv_init);
module_exit(drv_exit);


MODULE_AUTHOR("Copyright (C), 2006-2007. UTStarCom, Ltd. rights");
MODULE_DESCRIPTION("VPHONE sysdev Driver");
MODULE_LICENSE("GPL");

