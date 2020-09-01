#ifdef __cplusplus
extern "C" {
#endif

#include "diag.h"
#include "Common.h"

#if !defined(AFX_UDPTEST_H__D358530E_469B_4B90_A39D_FF6003CF915F__INCLUDED_)
#define AFX_UDPTEST_H__D358530E_469B_4B90_A39D_FF6003CF915F__INCLUDED_

typedef     int         SOCKET;
#define     closesocket close


typedef struct
{
	UINT32 m_ulSendPacketNum;		/* ���͵İ����� */
	UINT32 m_ulRecvPacketNum;		/* ���յİ����� */
} StatData_s;

typedef void (*RecvCallBack)(void *pUdpTest, struct sockaddr* pAddr, UINT8 *lpszBuffer, UINT32 ulLen);

typedef struct   
{
	char m_lpszName[10];		/* ���ڲ������� */
	BOOL m_bRunFlag;           /* �Ƿ��Ѿ���ɳ�ʼ��*/
    BOOL m_bAutoReplyFlag;      /* �Ƿ��Ѿ������Զ���������*/
	BOOL m_bUdpTraceFlag;		/* �Ƿ���Ҫ����Trace��ӡ */
	SOCKET m_Socket;				/* UDP Socket */
	struct sockaddr_in m_SendPeerAddr;  /* ���ͶԶ˵����ݣ���m_ulPeerIp��m_ulPeerPort���*/
	UINT32 m_ulLocalIp;			/* ���˵�IP��ַ */
	UINT32 m_ulLocalPort;		/* ���˵�Port�� */
	UINT32 m_ulRemoteIp;			/* Զ�˵�IP��ַ */
	UINT32 m_ulRemotePort;		/* Զ�˵�Port�� */

	
	UINT32 m_ulSendPacketNum;		/* ���͵İ����� */
	UINT32 m_ulRecvPacketNum;		/* ���յİ����� */


	UINT8  m_pBuffer[2000];         /* Buffer ������,������SendTestMessage*/
	RecvCallBack m_pDispatchMsgFunc;
}CUdpTest;

void InitUDPTest(CUdpTest *pUdpTest);

void CloseUDPTest(CUdpTest *pUdpTest);

UINT32 GetLocalPort(CUdpTest *pUdpTest);

VOS_STATUS Run(CUdpTest *pUdpTest, char *lpszName, char *strLocalIpAddr, UINT32 ulLocalPort);

void   getstatdata(CUdpTest *pUdpTest, StatData_s *pStatData);

void   clearstatdata(CUdpTest *pUdpTest);

void   SetDispatchMessageFunc(CUdpTest *pUdpTest, RecvCallBack pDispatchMsgFunc);

void OnRecvMessage(CUdpTest *pUdpTest, struct sockaddr* pAddr, UINT8 *lpszBuffer, UINT32 ulLen);

VOS_STATUS SendMessage(CUdpTest *pUdpTest, struct sockaddr *pSin, UINT8 *lpszBuffer, UINT32 ulLen);




/* ��ʼUDP�˿ڻ��ز������� */
UINT32 BeginUdpTest(char *lpszName, char *lpszLocalAddress, UINT32 ulLocalPort);

/* ����UDP�˿ڻ��ز������� */
VOS_STATUS EndUdpTest(UINT32 ulTestID);

/* �����յ�UDP�����Ƿ���Ҫ����UDP��������ȥ */
VOS_STATUS SetAutoReply(UINT32 ulTestID, BOOL bFlag);

/* ��ָ����UDP�˿��Ϸ������ݰ�*/
VOS_STATUS SendUdpData(UINT32 ulTestID, char *lpszPeerIp, UINT32 ulPeerPort, UINT8 *pucBuffer, UINT32 ulLength);

/* ���ͳ������ */
VOS_STATUS ClearStatData(UINT32 ulTestID);

/* ����UDP��Ϣ����ص����� */
VOS_STATUS SetMessageDispatchFunc(UINT32 ulTestID, RecvCallBack pDispatchMsgFunc);

/* ��ʾUDP��ͳ����Ϣ */
void ShowUdpStat();


/* �������UDP�ϵ�ͳ����Ϣ */
void ClearUdpStat();

/* UDP��ת����Ϣ */
UINT32 BeginUdpForward(char *lpszLocalAddress, UINT32 ulLocalPort, char *lpszRemoteAddress, UINT32 ulRemotePort);

/* ֹͣUDP�����ݵ�ת�� */
VOS_STATUS EndUdpForward(UINT32 ulTestID);

#define Diag_Output printf	

#endif /* !defined(AFX_UDPTEST_H__D358530E_469B_4B90_A39D_FF6003CF915F__INCLUDED_) */


#ifdef __cplusplus
}
#endif

