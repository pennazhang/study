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
	UINT32 m_ulSendPacketNum;		/* 发送的包个数 */
	UINT32 m_ulRecvPacketNum;		/* 接收的包个数 */
} StatData_s;

typedef void (*RecvCallBack)(void *pUdpTest, struct sockaddr* pAddr, UINT8 *lpszBuffer, UINT32 ulLen);

typedef struct   
{
	char m_lpszName[10];		/* 网口测试名称 */
	BOOL m_bRunFlag;           /* 是否已经完成初始化*/
    BOOL m_bAutoReplyFlag;      /* 是否已经启动自动接收任务*/
	BOOL m_bUdpTraceFlag;		/* 是否需要进行Trace打印 */
	SOCKET m_Socket;				/* UDP Socket */
	struct sockaddr_in m_SendPeerAddr;  /* 发送对端的数据，由m_ulPeerIp和m_ulPeerPort组成*/
	UINT32 m_ulLocalIp;			/* 本端的IP地址 */
	UINT32 m_ulLocalPort;		/* 本端的Port号 */
	UINT32 m_ulRemoteIp;			/* 远端的IP地址 */
	UINT32 m_ulRemotePort;		/* 远端的Port号 */

	
	UINT32 m_ulSendPacketNum;		/* 发送的包个数 */
	UINT32 m_ulRecvPacketNum;		/* 接收的包个数 */


	UINT8  m_pBuffer[2000];         /* Buffer 缓冲区,仅用于SendTestMessage*/
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




/* 开始UDP端口环回测试任务 */
UINT32 BeginUdpTest(char *lpszName, char *lpszLocalAddress, UINT32 ulLocalPort);

/* 结束UDP端口环回测试任务 */
VOS_STATUS EndUdpTest(UINT32 ulTestID);

/* 设置收到UDP包后，是否需要将该UDP包返还回去 */
VOS_STATUS SetAutoReply(UINT32 ulTestID, BOOL bFlag);

/* 在指定的UDP端口上发送数据包*/
VOS_STATUS SendUdpData(UINT32 ulTestID, char *lpszPeerIp, UINT32 ulPeerPort, UINT8 *pucBuffer, UINT32 ulLength);

/* 清除统计数据 */
VOS_STATUS ClearStatData(UINT32 ulTestID);

/* 设置UDP消息处理回掉函数 */
VOS_STATUS SetMessageDispatchFunc(UINT32 ulTestID, RecvCallBack pDispatchMsgFunc);

/* 显示UDP的统计信息 */
void ShowUdpStat();


/* 清除所有UDP上的统计信息 */
void ClearUdpStat();

/* UDP上转发消息 */
UINT32 BeginUdpForward(char *lpszLocalAddress, UINT32 ulLocalPort, char *lpszRemoteAddress, UINT32 ulRemotePort);

/* 停止UDP上数据的转发 */
VOS_STATUS EndUdpForward(UINT32 ulTestID);

#define Diag_Output printf	

#endif /* !defined(AFX_UDPTEST_H__D358530E_469B_4B90_A39D_FF6003CF915F__INCLUDED_) */


#ifdef __cplusplus
}
#endif

