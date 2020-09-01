#include "pciConfigLib.h"
#include "diagLib.h"
#include "FuncReg.h"
#include "Common.h"

#if defined(CONFIG_P2020RDB)

static inline void sync(void){ __asm__ __volatile__ ("sync" : : : "memory");}


BOOL g_bPCIDrvInit = FALSE;

UINT32 g_nCSSRPhy = 0xFFE00000;
#define CSSR_PCI_OFFSET 0x8000

typedef struct 
{
	UINT32 m_pAddressReg;
	UINT32 m_pDataReg;
} STRU_PCI_CONTROLLER;

STRU_PCI_CONTROLLER g_PCIDev[3];

void pciDrvInit(void)
{
	UINT32 pVirtualAddr;
	int i;
	if (g_bPCIDrvInit == TRUE)
	{
		return;
	}	

	pVirtualAddr = (UINT32)MapPhyToVirtual(g_nCSSRPhy + CSSR_PCI_OFFSET, 0x3000, 0);

	printk("pVirtualAddr = 0x%x\n", pVirtualAddr);
	
	g_PCIDev[0].m_pAddressReg = pVirtualAddr + 0x2000;
	g_PCIDev[1].m_pAddressReg = pVirtualAddr + 0x1000;
	g_PCIDev[2].m_pAddressReg = pVirtualAddr;

	g_PCIDev[0].m_pDataReg = pVirtualAddr + 0x2004;
	g_PCIDev[1].m_pDataReg = pVirtualAddr + 0x1004;
	g_PCIDev[2].m_pDataReg = pVirtualAddr + 4;

	for (i = 0; i < 3; i++)
	{
		printk("PCI Controller%d: Address Reg = 0x%x, Data Reg = 0x%x\n", i, g_PCIDev[i].m_pAddressReg, g_PCIDev[i].m_pDataReg);
	}
	
	
	g_bPCIDrvInit = TRUE;
} 
#endif
/*******************************************************************************
*
* pciConfigInByte - read one byte from the PCI configuration space
*
* This routine reads one byte from the PCI configuration space
*
* RETURNS:
* OK, or ERROR if this library is not initialized.
*
*/

VOS_STATUS pciConfigInByte(
	int nControllerNo, /* PCI Controller number */
    int	busNo,    /* bus number */
    int	deviceNo, /* device number */
    int	funcNo,	  /* function number */
    int	offset,	  /* offset into the configuration space */
    UINT8 * pData /* data read from the offset */
    )
{
#if defined(CONFIG_P2020RDB)
	UINT32 ulAddress;
	UINT32 *pAddressReg;
	UINT8 *pDataReg;
	
	pciDrvInit();
	if ((nControllerNo < 1) || (nControllerNo > 3))
	{
		return (VOS_ERROR);
	}
	ulAddress = ((busNo) << 16 | (deviceNo) << 11 | (funcNo) << 8); 
	pAddressReg = (UINT32 *)(g_PCIDev[nControllerNo - 1].m_pAddressReg);
	*pAddressReg = ulAddress | (offset & 0xfc) | ((offset & 0xf00) << 16) | 0x80000000; 
	sync();
	pDataReg = (UINT8 *)(g_PCIDev[nControllerNo - 1].m_pDataReg + (offset & 3));
	*pData = *pDataReg; 
	
	return (VOS_OK);
#else
	return (VOS_ERROR);	
#endif	
}

/*******************************************************************************
*
* pciConfigInWord - read one word from the PCI configuration space
*
* This routine reads one word from the PCI configuration space
*
* RETURNS: OK, or ERROR if this library is not initialized
*/

VOS_STATUS pciConfigInWord
    (
	int nControllerNo, /* PCI Controller number */
    int	busNo,      /* bus number */
    int	deviceNo,   /* device number */
    int	funcNo,     /* function number */
    int	offset,     /* offset into the configuration space */
    UINT16 * pData  /* data read from the offset */
    )
{
#if defined(CONFIG_P2020RDB)
	UINT32 ulAddress;
	UINT32 *pAddressReg;
	UINT16 *pDataReg;
	
	pciDrvInit();
	if ((nControllerNo < 1) || (nControllerNo > 3))
	{
		return (VOS_ERROR);
	}
	ulAddress = ((busNo) << 16 | (deviceNo) << 11 | (funcNo) << 8); 
	ulAddress = ulAddress | (offset & 0xfc) | ((offset & 0xf00) << 16) | 0x80000000;
	pAddressReg = (UINT32 *)(g_PCIDev[nControllerNo - 1].m_pAddressReg);
//	printk("Write Address Reg: *(0x%x) = 0x%x\n", (UINT32)pAddressReg, (UINT32)ulAddress);
	*pAddressReg = ulAddress; 
	sync();
	pDataReg = (UINT16 *)(g_PCIDev[nControllerNo - 1].m_pDataReg + (offset & 2));
	*pData = *pDataReg; 
//	printk("Read Data Reg: *(0x%x) = 0x%x\n", (UINT32)pDataReg, (UINT32)*pData);
	
	return (VOS_OK);
#else
	return (VOS_ERROR); 
#endif	
}

/*******************************************************************************
*
* pciConfigInLong - read one longword from the PCI configuration space
*
* This routine reads one longword from the PCI configuration space
*
* RETURNS: OK, or ERROR if this library is not initialized
*/

VOS_STATUS pciConfigInLong
    (
	int nControllerNo, /* PCI Controller number */
    int	busNo,     /* bus number */
    int	deviceNo,  /* device number */
    int	funcNo,    /* function number */
    int	offset,    /* offset into the configuration space */
    UINT32 * pData /* data read from the offset */
    )
{
#if defined(CONFIG_P2020RDB)
		UINT32 ulAddress;
		UINT32 *pAddressReg;
		UINT32 *pDataReg;
		
		pciDrvInit();
		if ((nControllerNo < 1) || (nControllerNo > 3))
		{
			return (VOS_ERROR);
		}
		ulAddress = ((busNo) << 16 | (deviceNo) << 11 | (funcNo) << 8); 
		pAddressReg = (UINT32 *)(g_PCIDev[nControllerNo - 1].m_pAddressReg);
		*pAddressReg = ulAddress | (offset & 0xfc) | ((offset & 0xf00) << 16) | 0x80000000; 
		sync();
		pDataReg = (UINT32 *)(g_PCIDev[nControllerNo - 1].m_pDataReg + (offset & 0));
		*pData = *pDataReg; 
		
		return (VOS_OK);
#else
		return (VOS_ERROR); 
#endif	
}

