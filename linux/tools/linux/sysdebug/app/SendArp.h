#ifndef _SEND_ARP_H_
#define _SEND_ARP_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "diag.h"

VOS_STATUS GetMAC(char *lpszIfName, UINT8 pucMAC[6]);
VOS_STATUS GetIP(char *lpszIfName, UINT8 pucIP[4]);
VOS_STATUS GetNetMask(char *lpszIfName, UINT8 pucMASK[4]);

VOS_STATUS FillArpPacket(UINT8 *pucData, UINT8 *pucFrameDstMAC, UINT8 *pucFrameSrcMac, UINT8 usOpCode, UINT8 *pucSrcMAC, UINT8 *pucSrcIP, UINT8 * pucDstMAC, UINT8 *pucDstIP);
VOS_STATUS SendARP(char *lpszIfName, UINT8 *pucData, int nDataLen);
void ArpTest(char *lpszIfName);

#ifdef __cplusplus
}
#endif

#endif


