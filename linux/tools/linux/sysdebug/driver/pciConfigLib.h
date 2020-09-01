#ifndef _PCI_CONFIG_LIB_H_
#define _PCI_CONFIG_LIB_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "diag.h"

extern VOS_STATUS pciConfigInByte	(int ControlNo, int busNo, int	deviceNo, int funcNo, int address, UINT8 * pData);
extern VOS_STATUS pciConfigInWord(int ControlNo, int busNo, int	deviceNo, int funcNo, int address, UINT16 * pData);
extern VOS_STATUS pciConfigInLong(int ControlNo, int busNo, int	deviceNo, int funcNo, int address, UINT32 * pData);


#ifdef __cplusplus
}
#endif

#endif

