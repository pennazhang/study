#include "pciConfigLib.h"
#include "sysdebugApi.h"

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
extern int g_nControllerNo;

VOS_STATUS pciConfigInByte(
    int	busNo,    /* bus number */
    int	deviceNo, /* device number */
    int	funcNo,	  /* function number */
    int	offset,	  /* offset into the configuration space */
    UINT8 * pData /* data read from the offset */
    )
{
	SYSWORD Result, p1InKernel;
	char lpszCmd1[] = "p1 = malloc 0x10";
	char lpszCmd2[100];
	char lpszCmd3[] = "free p1";

	if ((g_nControllerNo < 1) || (g_nControllerNo > 3))
	{
		Diag_Output("controller No:%d is invalid!\n", g_nControllerNo);
		return (VOS_ERROR);
	}

	if (sysCommandRun(lpszCmd1, &p1InKernel) == VOS_ERROR)
	{
		Diag_Output("sysCommandRun: %s failed\n", lpszCmd1);
		return (VOS_ERROR);
	}
		
	sprintf(lpszCmd2, "pciConfigInByte %d %d %d %d %d p1", g_nControllerNo, busNo, deviceNo, funcNo, offset);
	if ((sysCommandRun(lpszCmd2, &Result) == VOS_ERROR) || (Result == VOS_ERROR))
	{
		Diag_Output("sysCommandRun: %s failed\n", lpszCmd2);
		return (VOS_ERROR);
	}

	CopyMemFromKernelToUser((char *)pData, (char *)p1InKernel, 1);

	if (sysCommandRun(lpszCmd3, &Result) == VOS_ERROR)
	{
		Diag_Output("sysCommandRun: %s failed\n", lpszCmd3);
		return (VOS_ERROR);
	}
	return (VOS_OK);
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
    int	busNo,      /* bus number */
    int	deviceNo,   /* device number */
    int	funcNo,     /* function number */
    int	offset,     /* offset into the configuration space */
    UINT16 * pData  /* data read from the offset */
    )
{
	SYSWORD Result, p1InKernel;
	char lpszCmd1[] = "p1 = malloc 0x10";
	char lpszCmd2[100];
	char lpszCmd3[] = "free p1";

	if ((g_nControllerNo < 1) || (g_nControllerNo > 3))
	{
		Diag_Output("controller No:%d is invalid!\n", g_nControllerNo);
		return (VOS_ERROR);
	}

	if (sysCommandRun(lpszCmd1, &p1InKernel) == VOS_ERROR)
	{
		Diag_Output("sysCommandRun: %s failed\n", lpszCmd1);
		return (VOS_ERROR);
	}
		
	sprintf(lpszCmd2, "pciConfigInWord %d %d %d %d %d p1", g_nControllerNo, busNo, deviceNo, funcNo, offset);
	if ((sysCommandRun(lpszCmd2, &Result) == VOS_ERROR) || (Result == VOS_ERROR))
	{
		Diag_Output("sysCommandRun: %s failed\n", lpszCmd2);
		return (VOS_ERROR);
	}

	CopyMemFromKernelToUser((char *)pData, (char *)p1InKernel, 2);

	if (sysCommandRun(lpszCmd3, &Result) == VOS_ERROR)
	{
		Diag_Output("sysCommandRun: %s failed\n", lpszCmd3);
		return (VOS_ERROR);
	}
	return (VOS_OK);
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
    int	busNo,     /* bus number */
    int	deviceNo,  /* device number */
    int	funcNo,    /* function number */
    int	offset,    /* offset into the configuration space */
    UINT32 * pData /* data read from the offset */
    )
{
	SYSWORD Result, p1InKernel;
	char lpszCmd1[] = "p1 = malloc 0x10";
	char lpszCmd2[100];
	char lpszCmd3[] = "free p1";

	if ((g_nControllerNo < 1) || (g_nControllerNo > 3))
	{
		Diag_Output("controller No:%d is invalid!\n", g_nControllerNo);
		return (VOS_ERROR);
	}

	if (sysCommandRun(lpszCmd1, &p1InKernel) == VOS_ERROR)
	{
		Diag_Output("sysCommandRun: %s failed\n", lpszCmd1);
		return (VOS_ERROR);
	}
		
	sprintf(lpszCmd2, "pciConfigInLong %d %d %d %d %d p1", g_nControllerNo, busNo, deviceNo, funcNo, offset);
	if ((sysCommandRun(lpszCmd2, &Result) == VOS_ERROR) || (Result == VOS_ERROR))
	{
		Diag_Output("sysCommandRun: %s failed\n", lpszCmd2);
		return (VOS_ERROR);
	}

	CopyMemFromKernelToUser((char *)pData, (char *)p1InKernel, 4);

	if (sysCommandRun(lpszCmd3, &Result) == VOS_ERROR)
	{
		Diag_Output("sysCommandRun: %s failed\n", lpszCmd3);
		return (VOS_ERROR);
	}
	return (VOS_OK);
}

