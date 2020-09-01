#ifndef _PCI_LIB_H_
#define _PCI_LIB_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "diag.h"

typedef struct {
	UINT16 m_usID;
	char *m_lpszType;
}SEARCH_STRING;
char * SearchString(SEARCH_STRING *aSearchString, UINT16 usID, int ItemCount);

void SetPCIController(int nController);
VOS_STATUS pciDeviceShow(void);

void pciHeaderShow(int nBusNo, int nDeviceNo, int nFuncNo);

void PciDeviceGetConfig(int nBusNo, int nDeviceNo, int nFunctionNo, void *pucPciConfig);

VOS_STATUS PCIDeviceExplain(UINT8 *pucPCIConfig);
char *PCIExplainSubClass(UINT8 ucClass, UINT8 ucSubClass, UINT8 ucInterface);
void PCIFormatTitle(UINT8 *pucPCIConfig, char *lpszTitle, int nStart, int nEnd);
char *PCIExplainClass(UINT8 ucClass);
BOOL IsBitSet(UINT32 ulData, int nBit, int nEndianMode);
void PciDeviceSaveAll(char *lpszSaveFileName);
void PciDeviceShowFromFile(char *lpszPCIFileName);
VOS_STATUS PCIDeviceExplainByDefault(UINT8 *pucPCIConfig);



#ifdef __cplusplus
}
#endif

#endif



