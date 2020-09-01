
#ifdef __cplusplus
extern "C" {
#endif

#include "diagLib.h"
#include "time.h"
#include "FuncReg.h"
#include "pciLib.h"
#include "diagLib.h"
#include "pciConfigLib.h"

int g_nControllerNo = 2;  /* For P2020RDB, the Valid Value is 1 - 3 */

int GetEndianMode(void)
{	
	UINT16 us = 0x1122;	
	UINT8 uc = *(UINT8 *)&us;
	if (uc == 0x11)		
	{
		return BIG_ENDIAN_MODE;	
	}
	else
	{
		return LITTLE_ENDIAN_MODE;
	}
}
		

BOOL IsBitSet(UINT32 ulData, int nBit, int nEndianMode)
{
	if (BIG_ENDIAN_MODE == nEndianMode)
	{
		nBit = 31 - nBit;
	}
	
	if (ulData & (1 << nBit))
	{
		return (TRUE);
	}
	else
	{
		return (FALSE);
	}
}

void SetPCIController(int nController)
{
	printf("Change PCI Controller from %d to %d\n", g_nControllerNo, nController);
	g_nControllerNo = nController;
}

VOS_STATUS pciDeviceShow(void)
{
	int busNo, deviceNo, funcNo;
	UINT16 vendorId, deviceId;
	UINT32 ulClassCode;
	UINT8 ucCode, ucSubCode, ucInterface;
	
	if ((g_nControllerNo < 1) || (g_nControllerNo > 3))
	{
		Diag_Output("controller No:%d is invalid!\n", g_nControllerNo);
		return (VOS_ERROR);
	}

	Diag_Output("PCI Controller: %d\n", g_nControllerNo);
	Diag_Output ("bus		device	  function	vendorID  deviceID	class	 type\n");
	for (busNo = 0; busNo < PCI_MAX_BUS; busNo++)
	{
	    for (deviceNo=0; deviceNo < PCI_MAX_DEV; deviceNo++)
		{
			for (funcNo = 0; funcNo < 8; funcNo++)
			{
				vendorId = 0xFFFF;
				deviceId = 0xFFFF;
				
				pciConfigInWord (busNo, deviceNo, funcNo, PCI_CFG_VENDOR_ID, &vendorId);
				pciConfigInWord (busNo, deviceNo, funcNo, PCI_CFG_DEVICE_ID, &deviceId);
				pciConfigInLong (busNo, deviceNo, funcNo, PCI_CFG_REVISION, &ulClassCode);
				
				if ((vendorId != 0xFFFF) && (deviceId != 0xFFFF))
				{
					ucCode = (ulClassCode >> 24) & 0xFF;
					ucSubCode = (ulClassCode >> 16) & 0xFF;
					ucInterface= (ulClassCode >> 8) & 0xFF;

					Diag_Output ("%.8x	%.8x  %.8x	%.8x  %.8x	%02x-%02x-%02x	 %s\n", busNo, deviceNo, funcNo, 
								vendorId, deviceId, ucCode, ucSubCode, ucInterface,
								PCIExplainSubClass(ucCode, ucSubCode, ucInterface));
				}
			}
	    }
	}
	return (VOS_OK);
}



void pciHeaderShow(int nBusNo, int nDeviceNo, int nFuncNo)
{
	UINT8 pucData[256];

	int i;
	for (i = 0; i < 255; i += 4)
	{
		pciConfigInLong(nBusNo, nDeviceNo, nFuncNo, i, (UINT32 *)(pucData + i));
	}
	d(pucData, 256, 2, 0 - (UINT32)pucData);
//	if (PCIDeviceExplain(pucData) != VOS_OK)
	{
		PCIDeviceExplainByDefault(pucData);
	}
}

#if 0

void PciDeviceGetConfig(int nBusNo, int nDeviceNo, int nFunctionNo, void *pucPciConfig)
{
	char lpszCmd[100];
	UINT32 ulAddr;

	ulAddr = sysRun("malloc 0x100");
	sprintf(lpszCmd, "pciConfigInArray %d %d %d 0 0x100 0x%x", nBusNo, nDeviceNo, nFunctionNo, ulAddr);
	sysRun(lpszCmd);

	CopyMemFromKernelToUser(pucPciConfig, (char *)ulAddr, 0x100);

	sprintf(lpszCmd, "free 0x%x", ulAddr);
	sysRun(lpszCmd);
}

void PciDeviceSaveConfig(int nBusNo, int nDeviceNo, int nFunctionNo, char *lpszFileName)
{
	UINT8 pucPciConfig[256];
	PciDeviceGetConfig(nBusNo, nDeviceNo, nFunctionNo, (void *)pucPciConfig);
	CopyMemoryToFile(pucPciConfig, 256, lpszFileName);
}

typedef struct
{
	UINT8 m_ucBusNo;
	UINT8 m_ucDeviceNo;
	UINT8 m_ucFuncNo;
	UINT16 m_usVendorID;
	UINT16 m_usDeviceID;	
	void * m_pExplainFuncAddr;
}PCI_DEVICE_EXPLAIN_MAP;


PCI_DEVICE_EXPLAIN_MAP g_aPCIATCA7360[] = 
{
	{0, 30, 0, 0x8086, 0x244e, PCIDeviceExplain},
//	{0, 30, 0, 0x8086, 0x244e, ICH10ExplainP2P},
};
#endif

void PCIFormatTitle(UINT8 *pucPCIConfig, char *lpszTitle, int nStart, int nEnd)
{
	int nCount = nEnd - nStart + 1;
	if (nCount > 4)
	{
		sprintf(lpszTitle, "%02x %02x .. %02x (%2x-%2x)", pucPCIConfig[nStart], pucPCIConfig[nStart + 1], pucPCIConfig[nEnd], nStart, nEnd);
	}
	else if (nCount == 4)
	{
		sprintf(lpszTitle, "%02x %02x %02x %02x (%2x-%2x)", pucPCIConfig[nStart], pucPCIConfig[nStart + 1], pucPCIConfig[nStart + 2], pucPCIConfig[nEnd], nStart, nEnd);
	}
	else if (nCount == 3)
	{
		sprintf(lpszTitle, "%02x %02x %02x (%2x-%2x)", pucPCIConfig[nStart], pucPCIConfig[nStart + 1], pucPCIConfig[nStart + 2], nStart, nEnd);
	}
	else if (nCount == 2)
	{
		sprintf(lpszTitle, "%02x %02x (%2x-%2x)", pucPCIConfig[nStart], pucPCIConfig[nStart + 1], nStart, nEnd);
	}
	else if (nCount == 1)
	{
		sprintf(lpszTitle, "%02x (%2x-%2x)", pucPCIConfig[nStart], nStart, nEnd);
	}
	else
	{
//		ASSERT(FALSE);
	}
}




char g_lpszDefaultSearchName[0x100];
SEARCH_STRING g_aPCIClassType[] = 
{
	{0x00, "Device was built before Class Code definitions were finalized"},
	{0x01, "Mass storage controller"},
	{0x02, "Network controller"},
	{0x03, "Display controller"},
	{0x04, "Multimedia device"},
	{0x05, "Memory controller"},
	{0x06, "Bridge device"},
	{0x07, "Simple communication controllers"},
	{0x08, "Base system peripherals"},
	{0x09, "Input devices"},
	{0x0A, "Docking stations"},
	{0x0B, "Processors"},
	{0x0C, "Serial bus controllers"},
	{0x0D, "Wireless controller"},
	{0x0E, "Intelligent I/O controllers"},
	{0x0F, "Satellite communication controllers"},
	{0x10, "Encryption/Decryption controllers"},
	{0x11, "Data acquisition and signal processing controllers"},
	{0xFF, "Device does not fit in any defined classes"},
};

char * SearchString(SEARCH_STRING *aSearchString, UINT16 usID, int ItemCount)
{
	int i;
	for (i = 0; i < ItemCount; i++)
	{
		if (usID == aSearchString[i].m_usID)
		{
			return (aSearchString[i].m_lpszType);
		}
	}
	return (NULL);
}

char *PCIExplainClass(UINT8 ucClass)
{
	char *lpszBuffer;
	lpszBuffer = SearchString(g_aPCIClassType, (UINT16)ucClass, sizeof(g_aPCIClassType) / sizeof(SEARCH_STRING));
	if (lpszBuffer != NULL)
	{
		return (lpszBuffer);
	}
	else
	{
		strcpy(g_lpszDefaultSearchName, "Unknown Class Code");
		return (g_lpszDefaultSearchName);
	}
}

/* Device was built before Class Code definitions were finalized */
SEARCH_STRING g_aPCISubClassType_0[] = 
{
	/*  SubClass + Interface */
	{0x0000,  "All currently implemented devices except VGA-compatible devices"},
	{0x0100,  "VGA-compatible device"}
};

/* Mass storage controller */
SEARCH_STRING g_aPCISubClassType_1[] = 
{
	{0x0000, "SCSI bus controller"},
//	{0x01, "IDE controller"},  /* For IDE Device, the subcalss = 0x01, the Interface = 0x##  */
	{0x0200, "Floppy disk controller"},
	{0x0300, "IPI bus controller"},
	{0x0400, "RAID controller"},
	{0x8000, "Other mass storage controller"},
};

/* Network controller */
SEARCH_STRING g_aPCISubClassType_2[] = 
{
	{0x0000, "Ethernet controller"},
	{0x0100, "Token Ring controller"},
	{0x0200, "FDDI controller"},
	{0x0300, "ATM controller"},
	{0x0400, "ISDN controller"},
	{0x8000, "Other network controller"},
};

/* Display controller */
SEARCH_STRING g_aPCISubClassType_3[] = 
{
	{0x0000, "VGA-compatible controller."},
	{0x0001, "8514-compatible controller"},
	{0x0100, "XGA controller"},
	{0x0200, "3D controller"},
	{0x8000, "Other display controller"},
};

/* Multimedia device */
SEARCH_STRING g_aPCISubClassType_4[] = 
{
	{0x0000, "Video device"},
	{0x0100, "Audio device"},
	{0x0200, "Computer telephony device"},
	{0x8000, "Other multimedia device"},
};

/* Memory controller */
SEARCH_STRING g_aPCISubClassType_5[] = 
{
	{0x0000, "RAM"},
	{0x0100, "Flash"},
	{0x8000, "Other memory controller"},
};

/* Bridge device */
SEARCH_STRING g_aPCISubClassType_6[] = 
{
	{0x0000, "Host bridge"},
	{0x0100, "ISA bridge"},
	{0x0200, "EISA bridge"},
	{0x0300, "MCA bridge"},
	{0x0400, "PCI-to-PCI bridge"},
	{0x0401, "Subtractive Decode PCI-to-PCI bridge"},
	{0x0500, "PCMCIA bridge" },
	{0x0600, "NuBus bridge" },
	{0x0700, "CardBus bridge"},
	{0x8000, "Other bridge device"},
};

/* Simple communication controllers */
SEARCH_STRING g_aPCISubClassType_7[] = 
{
	{0x0000, "Generic XT-compatible serial controller"},
	{0x0001, "16450-compatible serial controller"},
	{0x0002, "16550-compatible serial controller"},
	{0x0003, "16650-compatible serial controller"},
	{0x0004, "16750-compatible serial controller"},
	{0x0005, "16850-compatible serial controller"},
	{0x0006, "16950-compatible serial controller"},
	{0x0100, "Parallel port"},
	{0x0101, "Bi-directional parallel port"},
	{0x0102, "ECP 1.X compliant parallel port"},
	{0x0103, "IEEE1284 controller"},
	{0x01FE, "IEEE1284 target device (not a controller)" },
	{0x0200, "Multiport serial controller" },
	{0x0300, "Generic modem" },
	{0x0301, "Hayes compatible modem, 16450-compatible interface" },
	{0x0302, "Hayes compatible modem, 16550-compatible interface" },
	{0x0303, "Hayes compatible modem, 16650-compatible interface" },
	{0x0304, "Hayes compatible modem, 16750-compatible interface" },
	{0x8000, "Other communications device" },
};

/* Base system peripherals */
SEARCH_STRING g_aPCISubClassType_8[] = 
{
	{0x0000, "Generic 8259 PIC"},
	{0x0001, "ISA PIC"},
	{0x0002, "EISA PIC"},
	{0x0010, "I/O APIC interrupt controller"},
	{0x0020, "I/O(x) APIC interrupt controller"},
	{0x0100, "Generic 8237 DMA controller"},
	{0x0101, "ISA DMA controller"},
	{0x0102, "EISA DMA controller"},
	{0x0200, "Generic 8254 system timer"},
	{0x0201, "ISA system timer"},
	{0x0202, "EISA system timers (two timers)"},
	{0x0300, "Generic RTC controller"},
	{0x0301, "ISA RTC controller"},
	{0x0400, "Generic PCI Hot-Plug controller"},
	{0x8000, "Other system peripheral" },
};

/* Input devices */
SEARCH_STRING g_aPCISubClassType_9[] = 
{
	{0x0000, "Keyboard controller"},
	{0x0100, "Digitizer (pen)"},
	{0x0200, "Mouse controller"},
	{0x0300, "Scanner controller"},
	{0x0400, "Gameport controller (generic)"},
	{0x0410, "Gameport controller"},
	{0x8000, "Other input controller"},
};

/* Docking stations */
SEARCH_STRING g_aPCISubClassType_10[] = 
{
	{0x0000, "Generic docking station" },
	{0x8000, "Other type of docking station" },
};

/* Process */
SEARCH_STRING g_aPCISubClassType_11[] = 
{
	{0x0000, "386"},
	{0x0100, "486"},
	{0x0200, "Pentium"},
	{0x1000, "Alpha"},
	{0x2000, "PowerPC"},
	{0x3000, "MIPS"},
	{0x4000, "Co-processor"},
};

/* Serial bus controllers */
SEARCH_STRING g_aPCISubClassType_12[] = 
{
	{0x0000, "IEEE 1394 (FireWire)" },
	{0x0010, "IEEE 1394 following the 1394 OpenHCI specification"},
	{0x0100, "ACCESS.bus"},
	{0x0200, "SSA"},
	{0x0300, "Universal Serial Bus (USB)"},
	{0x0310, "Universal Serial Bus (USB)"},
	{0x0320, "Universal Serial Bus (USB)"},
	{0x0380, "Universal Serial Bus with no specific programming interface"},
	{0x03FE, "USB device (not host controller)"},
	{0x0400, "Fibre Channel"},
	{0x0500, "SMBus (System Management Bus)"},
};

/* Wireless controller */
SEARCH_STRING g_aPCISubClassType_13[] = 
{
	{0x0000, "iRDA compatible controller"},
	{0x0100, "Consumer IR controller"},
	{0x1000, "RF controller"},
	{0x8000, "Other type of wireless controller"},
};

/* Intelligent I/O controllers */
#if 0
SEARCH_STRING g_aPCISubClassType_14[] = 
{
}
#endif

/* Satellite communication controllers */
SEARCH_STRING g_aPCISubClassType_15[] = 
{
	{0x0100, "TV"},
	{0x0200, "Audio"},
	{0x0300, "Voice"},
	{0x0400, "Data"},
};

/* Encryption/Decryption controllers */
SEARCH_STRING g_aPCISubClassType_16[] = 
{
	{0x0000, "Network and Computing en/decryption"},
	{0x0100, "Entertainment en/decryption"},
	{0x8000, "Other en/decryption"},
};

/* Data acquisition and signal processing controllers */
SEARCH_STRING g_aPCISubClassType_17[] = 
{
	{0x0000, "DPIO Modules"},
	{0x8000, "Other Data acquisition/signal processing controllers"},
};

char *PCIExplainSubClass(UINT8 ucClass, UINT8 ucSubClass, UINT8 ucInterface)
{
	char *lpszBuffer = NULL;
	switch (ucClass)
	{
		case 0: /* Device was built before Class Code definitions were finalized */
		{
			lpszBuffer = SearchString(g_aPCISubClassType_0, (UINT16)((ucSubClass << 8) | ucInterface), sizeof(g_aPCISubClassType_0) / sizeof(SEARCH_STRING));
			break;
		}
		case 1: /* Mass storage controller */
		{
			if (ucSubClass == 0x1) /* That's IDE Controller */
			{
				lpszBuffer = g_lpszDefaultSearchName;
				strcpy(g_lpszDefaultSearchName, "IDE Controller");
			}
			else
			{
				lpszBuffer = SearchString(g_aPCISubClassType_1, (UINT16)((ucSubClass << 8) | ucInterface), sizeof(g_aPCISubClassType_1) / sizeof(SEARCH_STRING));
			}
			break;
		}
		case 2: /* Network controller */
		{
			lpszBuffer = SearchString(g_aPCISubClassType_2,  (UINT16)((ucSubClass << 8) | ucInterface), sizeof(g_aPCISubClassType_2) / sizeof(SEARCH_STRING));
			break;
		}
		case 3: /* Display controller */
		{
			lpszBuffer = SearchString(g_aPCISubClassType_3,  (UINT16)((ucSubClass << 8) | ucInterface), sizeof(g_aPCISubClassType_3) / sizeof(SEARCH_STRING));
			break;
		}
		case 4: /* Multimedia device */
		{
			lpszBuffer = SearchString(g_aPCISubClassType_4,  (UINT16)((ucSubClass << 8) | ucInterface), sizeof(g_aPCISubClassType_4) / sizeof(SEARCH_STRING));
			break;
		}
		case 5: /* Memory Controller */
		{
			lpszBuffer = SearchString(g_aPCISubClassType_5,  (UINT16)((ucSubClass << 8) | ucInterface), sizeof(g_aPCISubClassType_5) / sizeof(SEARCH_STRING));
			break;
		}
		case 6: /* PCI Bridge */
		{
			if (ucSubClass == 0x8) /* RACEway bridge */
			{
				lpszBuffer = g_lpszDefaultSearchName;
				strcpy(g_lpszDefaultSearchName, "RACEway bridge");
			}
			else
			{
				lpszBuffer = SearchString(g_aPCISubClassType_6,  (UINT16)((ucSubClass << 8) | ucInterface), sizeof(g_aPCISubClassType_6) / sizeof(SEARCH_STRING));
			}
			break;
		}
		case 7: /* Simple communication controllers */
		{
			lpszBuffer = SearchString(g_aPCISubClassType_7,  (UINT16)((ucSubClass << 8) | ucInterface), sizeof(g_aPCISubClassType_7) / sizeof(SEARCH_STRING));
			break;
		}
		case 8:	/* Base system peripherals */		
		{
			lpszBuffer = SearchString(g_aPCISubClassType_8,  (UINT16)((ucSubClass << 8) | ucInterface), sizeof(g_aPCISubClassType_8) / sizeof(SEARCH_STRING));
			break;
		}
		case 9:	/* Input Device */		
		{
			lpszBuffer = SearchString(g_aPCISubClassType_9,  (UINT16)((ucSubClass << 8) | ucInterface), sizeof(g_aPCISubClassType_9) / sizeof(SEARCH_STRING));
			break;
		}
		case 10:	/* Docking stations */	
		{
			lpszBuffer = SearchString(g_aPCISubClassType_10,  (UINT16)((ucSubClass << 8) | ucInterface), sizeof(g_aPCISubClassType_10) / sizeof(SEARCH_STRING));
			break;
		}		
		case 11:	/* Processors */	
		{
			lpszBuffer = SearchString(g_aPCISubClassType_11,  (UINT16)((ucSubClass << 8) | ucInterface), sizeof(g_aPCISubClassType_11) / sizeof(SEARCH_STRING));
			break;
		}
		case 12:	/* Serial bus controllers */	
		{
			lpszBuffer = SearchString(g_aPCISubClassType_12,  (UINT16)((ucSubClass << 8) | ucInterface), sizeof(g_aPCISubClassType_12) / sizeof(SEARCH_STRING));
			break;
		}
		case 13:	/* Wireless controller */	
		{
			lpszBuffer = SearchString(g_aPCISubClassType_13,  (UINT16)((ucSubClass << 8) | ucInterface), sizeof(g_aPCISubClassType_13) / sizeof(SEARCH_STRING));
			break;
		}
		case 14:	/* Intelligent I/O controllers */	
		{
			if (ucSubClass == 0x00)
			{				
				lpszBuffer = g_lpszDefaultSearchName;
				strcpy(g_lpszDefaultSearchName, "Intelligent I/O (I2O) Architecture Specification 1.0");
			}
			break;
		}
		case 15:	/* Satellite communication controllers  */	
		{
			lpszBuffer = SearchString(g_aPCISubClassType_15,  (UINT16)((ucSubClass << 8) | ucInterface), sizeof(g_aPCISubClassType_15) / sizeof(SEARCH_STRING));
			break;
		}
		case 16:	/* Encryption/Decryption controllers */	
		{
			lpszBuffer = SearchString(g_aPCISubClassType_16,  (UINT16)((ucSubClass << 8) | ucInterface), sizeof(g_aPCISubClassType_16) / sizeof(SEARCH_STRING));
			break;
		}
		case 17:	/* Data acquisition and signal processing controllers */	
		{
			lpszBuffer = SearchString(g_aPCISubClassType_17,  (UINT16)((ucSubClass << 8) | ucInterface), sizeof(g_aPCISubClassType_17) / sizeof(SEARCH_STRING));
			break;
		}
		default:
		{
			lpszBuffer = g_lpszDefaultSearchName;
			strcpy(g_lpszDefaultSearchName, "Unknown Device");
		}
	}
	return (lpszBuffer);
}
#if 0
typedef struct
{
	UINT16 m_usVendorID;
	UINT16 m_usDeviceID;
	void * m_pucExplainFunc;
	char m_lpszDeviceName[100];
}PCI_DEVICE_EXPLAIN;

PCI_DEVICE_EXPLAIN g_PciDeviceExplain[] = 
{
//	{0x8086, 0x244e, ICH10ExplainP2P, "Intel ICH10 PCI to PCI Bridge"},
	{0x8086, 0x3A10, ICH10ExplainPCI_LPC, "LPC on Corporate ICH10 Engineering sample"},
	{0x8086, 0x3A14, ICH10ExplainPCI_LPC, "LPC on Corporate ICH10DO"},
	{0x8086, 0x3A1A, ICH10ExplainPCI_LPC, "LPC on Corporate ICH10D"},
	{0x8086, 0x3A16, ICH10ExplainPCI_LPC, "LPC on Consumer ICH10R"},
	{0x8086, 0x3A18, ICH10ExplainPCI_LPC, "LPC on Consumer ICH10"},
	{0x8086, 0x3A1e, ICH10ExplainPCI_LPC, "LPC on Consumer ICH10 Engineering sample"},
};

typedef VOS_STATUS (*PCI_DEVICE_EXPLAIN_FUNC)(UINT8 *pucPCIConfig);

/* Explain the common PCI Device  */
VOS_STATUS PCIDeviceExplain(UINT8 *pucPCIConfig)
{
	int i;
	VOS_STATUS iRet;
	
	for (i = 0; i < sizeof(g_PciDeviceExplain) / sizeof(PCI_DEVICE_EXPLAIN); i++)
	{
		if ((g_PciDeviceExplain[i].m_usVendorID == *((UINT16 *)pucPCIConfig)) && (g_PciDeviceExplain[i].m_usDeviceID == *((UINT16 *)(pucPCIConfig + 2))))
		{
			PCI_DEVICE_EXPLAIN_FUNC pFunc = (PCI_DEVICE_EXPLAIN_FUNC)(g_PciDeviceExplain[i].m_pucExplainFunc);
			iRet = pFunc(pucPCIConfig);
			return (iRet);
		}
	}

	return (VOS_ERROR);
}
#endif

/* Explain the common PCI Device  */
VOS_STATUS PCIDeviceExplainByDefault(UINT8 *pucPCIConfig)
{
	char lpszTitle[0x100];
	UINT16 usData;
	UINT8 ucData;

	if (*(UINT32 *)pucPCIConfig == 0xFFFFFFFF)
	{
		return (VOS_ERROR);
	}

	PCIFormatTitle(pucPCIConfig, lpszTitle, 0, 1);
	Diag_Output("%30s: Vendor ID 0x%04x\n", lpszTitle, *((UINT16 *)pucPCIConfig));

	PCIFormatTitle(pucPCIConfig, lpszTitle, 2, 3);
	Diag_Output("%30s: Device ID 0x%04x\n", lpszTitle, *((UINT16 *)(pucPCIConfig + 2)));

	PCIFormatTitle(pucPCIConfig, lpszTitle, 4, 5);
	Diag_Output("%30s: Command Register 0x%04x\n", lpszTitle, *((UINT16 *)(pucPCIConfig + 4)));
	usData = *((UINT16 *)(pucPCIConfig + 4));
	if (IsBitSet(usData, 0, LITTLE_ENDIAN_MODE))
	{
		Diag_Output("%36s IOSpace - Enable\n","");
	}
	else
	{
		Diag_Output("%36s IOSpace - Disable\n","");
	}
	
	if (IsBitSet(usData, 1, LITTLE_ENDIAN_MODE))
	{
		Diag_Output("%36s Memory Space - Enable\n","");
	}
	else
	{
		Diag_Output("%36s Memory Space - Disable\n","");
	}
	
	if (IsBitSet(usData, 2, LITTLE_ENDIAN_MODE))
	{
		Diag_Output("%36s Bus Master - Enable\n","");
	}
	else
	{
		Diag_Output("%36s Bus Master - Disable\n","");
	}
	if (IsBitSet(usData, 3, LITTLE_ENDIAN_MODE))
	{
		Diag_Output("%36s Special Cycle - Enable\n","");
	}
	else
	{
		Diag_Output("%36s Special Cycle - Disable\n","");
	}
	if (IsBitSet(usData, 4, LITTLE_ENDIAN_MODE))
	{
		Diag_Output("%36s Memory write and Invalidate - Enable\n","");
	}
	else
	{
		Diag_Output("%36s Memory write and Invalidate - Disable\n","");
	}
	if (IsBitSet(usData, 5, LITTLE_ENDIAN_MODE))
	{
		Diag_Output("%36s VGA Palette Snoop - Enable\n","");
	}
	else
	{
		Diag_Output("%36s VGA Palette Snoop - Disable\n","");
	}
	if (IsBitSet(usData, 6, LITTLE_ENDIAN_MODE))
	{
		Diag_Output("%36s Parity Error Response - Enable\n","");
	}
	else
	{
		Diag_Output("%36s Parity Error Response - Disable\n","");
	}
	if (IsBitSet(usData, 7, LITTLE_ENDIAN_MODE))
	{
		Diag_Output("%36s Wait Cycle Control - Enable\n","");
	}
	else
	{
		Diag_Output("%36s Wait Cycle Control - Disable\n","");
	}
	if (IsBitSet(usData, 8, LITTLE_ENDIAN_MODE))
	{
		Diag_Output("%36s SERR# - Enable\n","");
	}
	else
	{
		Diag_Output("%36s SERR# - Disable\n","");
	}
	if (IsBitSet(usData, 9, LITTLE_ENDIAN_MODE))
	{
		Diag_Output("%36s Fast Back to Back - Enable\n","");
	}
	else
	{
		Diag_Output("%36s Fast Back to Back - Disable\n","");
	}
	if (IsBitSet(usData, 10, LITTLE_ENDIAN_MODE))
	{
		Diag_Output("%36s PCI Interrupt is disable\n","");
	}
	else
	{
		Diag_Output("%36s PCI Interrupt is enable\n","");
	}

	PCIFormatTitle(pucPCIConfig, lpszTitle, 6, 7);
	Diag_Output("%30s: Status Register 0x%04x\n", lpszTitle, *((UINT16 *)(pucPCIConfig + 6)));
	usData = *((UINT16 *)(pucPCIConfig + 6));
	if (IsBitSet(usData, 3, LITTLE_ENDIAN_MODE))
	{
		Diag_Output("%36s Interrupt is asserted.\n","");
	}
	else
	{
		Diag_Output("%36s Interrupt is deasserted..\n","");
	}

	if (IsBitSet(usData, 4, LITTLE_ENDIAN_MODE))
	{
		Diag_Output("%36s Capabilities List - Enable\n","");
	}
	else
	{
		Diag_Output("%36s Capabilities List - Disable\n","");
	}
	if (IsBitSet(usData, 5, LITTLE_ENDIAN_MODE))
	{
		Diag_Output("%36s 66 MHz - Enable\n","");
	}
	else
	{
		Diag_Output("%36s 66 MHz - Disable\n","");
	}
	if (IsBitSet(usData, 7, LITTLE_ENDIAN_MODE))
	{
		Diag_Output("%36s Fast Back-to-Back Capable - Enable\n","");
	}
	else
	{
		Diag_Output("%36s Fast Back-to-Back Capable - Disable\n","");
	}
	if (IsBitSet(usData, 8, LITTLE_ENDIAN_MODE))
	{
		Diag_Output("%36s Data parity error received.\n","");
	}
	else
	{
		Diag_Output("%36s No data parity error was detected.\n","");
	}

	switch ((usData >> 9) & 0x3)
	{
		case 0:			
			Diag_Output("%36s DEVSEL timing - 0 (Fast)\n","");
			break;
		case 1: 		
			Diag_Output("%36s DEVSEL timing - 1 (Medium)\n","");
			break;
		case 2:
			Diag_Output("%36s DEVSEL timing - 2 (Slow)\n","");
			break;
		default:
			Diag_Output("%36s DEVSEL timing - 3 (Invalid Parameter)\n","");
			break;
	}
	if (IsBitSet(usData, 11, LITTLE_ENDIAN_MODE))
	{
		Diag_Output("%36s Device generates a completion packet with target abort status\n","");
	}
	else
	{
		Diag_Output("%36s No signaled target abort packet generates\n","");
	}
	if (IsBitSet(usData, 12, LITTLE_ENDIAN_MODE))
	{
		Diag_Output("%36s Device Received a Target Abort\n","");
	}
	else
	{
		Diag_Output("%36s No target abort received.\n","");
	}
	if (IsBitSet(usData, 13, LITTLE_ENDIAN_MODE))
	{
		Diag_Output("%36s Received a Master Abort\n","");
	}
	else
	{
		Diag_Output("%36s No master abort received.\n","");
	}
	
	if (IsBitSet(usData, 14, LITTLE_ENDIAN_MODE))
	{
		Diag_Output("%36s System Error (SERR#) Received\n","");
	}
	else
	{
		Diag_Output("%36s System Error (SERR#) not Received\n","");
	}
	if (IsBitSet(usData, 15, LITTLE_ENDIAN_MODE))
	{
		Diag_Output("%36s Detected Parity Error\n","");
	}
	else
	{
		Diag_Output("%36s Not detected Parity Error\n","");
	}
	
	
	PCIFormatTitle(pucPCIConfig, lpszTitle, 8, 8);
	Diag_Output("%30s: Revision 0x%02x\n", lpszTitle, *((UINT8 *)(pucPCIConfig + 8)));

	PCIFormatTitle(pucPCIConfig, lpszTitle, 9, 0xb);
	Diag_Output("%30s: Class Code\n", lpszTitle);
	Diag_Output("%36s Class 0x%x (%s)\n", "", pucPCIConfig[0xb], PCIExplainClass(pucPCIConfig[0xb]));
	Diag_Output("%36s SubClass 0x%x;  Interface 0x%x;  (%s)\n","", pucPCIConfig[0xa], pucPCIConfig[0x9], PCIExplainSubClass(pucPCIConfig[0xb], pucPCIConfig[0xa], pucPCIConfig[0x9]));

	PCIFormatTitle(pucPCIConfig, lpszTitle, 0xc, 0xc);
	Diag_Output("%30s: Cache Line Size - %d\n", lpszTitle, pucPCIConfig[0xc]);
	PCIFormatTitle(pucPCIConfig, lpszTitle, 0xd, 0xd);
	Diag_Output("%30s: Latency Timer - %d\n", lpszTitle, pucPCIConfig[0xd]);
	PCIFormatTitle(pucPCIConfig, lpszTitle, 0xe, 0xe);
	Diag_Output("%30s: Header Type - %d\n", lpszTitle, pucPCIConfig[0xe]);
	ucData = pucPCIConfig[0xE];
	if (ucData & (1 << 7))
	{
		Diag_Output("%36s MultiFunction Device\n","");
	}
	else
	{
		Diag_Output("%36s Single Function Device\n","");
	}

	switch (ucData & 0x7F)
	{
		case 0:
			Diag_Output("%36s Type 00h Configuration Space Header\n","");
			break;
		case 1:
			Diag_Output("%36s Type 01h PCI to PCI Bridge Specification\n","");
			break;
		case 2:
			Diag_Output("%36s Type 02h CardBus bridge Specification\n","");
			break;
		default:
			Diag_Output("%36s Invalid Header Type\n","");
	}

	PCIFormatTitle(pucPCIConfig, lpszTitle, 0xF, 0xF);
	Diag_Output("%30s: Build In Self test - %d\n", lpszTitle, pucPCIConfig[0xF]);
	ucData = pucPCIConfig[0xF];
	if (ucData & (1 << 7))
	{
		Diag_Output("%36s BIST - support\n","");
		if (ucData & 0xF)
		{
			Diag_Output("%36s BIST - Failed\n","");
		}
		else
		{
			Diag_Output("%36s BIST - Passed\n","");
		}
	}
	else
	{
		Diag_Output("%36s BIST - not support\n","");
	}

	PCIFormatTitle(pucPCIConfig, lpszTitle, 0x3c, 0x3c);
	Diag_Output("%30s: Interrupt Line - %d\n", lpszTitle, pucPCIConfig[0x3C]);
	PCIFormatTitle(pucPCIConfig, lpszTitle, 0x3d, 0x3d);
	ucData = pucPCIConfig[0x3D];
	switch (ucData)
	{
		case 0:
			Diag_Output("%30s: Interrupt Pin - %d(None)\n", lpszTitle, pucPCIConfig[0x3D]);
			break;
		case 1:
			Diag_Output("%30s: Interrupt Pin - %d(INTA#)\n", lpszTitle, pucPCIConfig[0x3D]);
			break;
		case 2:
			Diag_Output("%30s: Interrupt Pin - %d(INTB#)\n", lpszTitle, pucPCIConfig[0x3D]);
			break;
		case 3:
			Diag_Output("%30s: Interrupt Pin - %d(INTC#)\n", lpszTitle, pucPCIConfig[0x3D]);
			break;
		case 4:
			Diag_Output("%30s: Interrupt Pin - %d(INTD#)\n", lpszTitle, pucPCIConfig[0x3D]);
			break;
		default:
			Diag_Output("%30s: Interrupt Pin - %d(Invalid Parameter)\n", lpszTitle, pucPCIConfig[0x3D]);
			break;
			
	}

	return (VOS_OK);
}

#if 0
VOS_STATUS ICH10ExplainP2P(UINT8 *pucPCIConfig)
{
	
}
void PciDeviceShowFromFile(char *lpszPCIFileName)
{
	int iIndex, nPCIDeviceCount;
	char lpszFileName[100] = "PCIDevice_Config.dat";
	UINT8 pucConfig[0x100];
	
	if (lpszPCIFileName != NULL)
	{
		strcpy(lpszFileName, lpszPCIFileName);
	}

	if (CopyFileToMemory((UINT8 *)&nPCIDeviceCount, 4, lpszFileName, 0) != VOS_OK)
	{
		Diag_Output("Read File :%s failed!\n", lpszFileName);
		return;
	}
	for (iIndex = 0; iIndex < nPCIDeviceCount; iIndex++)
	{
		if (CopyFileToMemory(pucConfig, 0x100, lpszFileName, 4 + iIndex * 0x100) != VOS_OK)
		{
			Diag_Output("Read File :%s failed!\n", lpszFileName);
			break;
		}
		Diag_Output("PCI Index = %d\n", iIndex);
//		d(pucConfig, 256, 0, 0 - (UINT32)pucConfig);
		if (PCIDeviceExplain(pucConfig) != VOS_OK)
		{
			Diag_Output("Invalid PCI Config Space, Vendor Id = 0x%x, Device Id = 0x%x\n", *(UINT16 *)pucConfig, *(UINT16 *)(pucConfig + 2));
//			break;
		}
		else
		{
			d(pucConfig, 256, 0, 0 - (UINT32)pucConfig);
		}
	}
	
}
#endif


#ifdef __cplusplus
}
#endif

