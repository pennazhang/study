#ifdef __cplusplus
extern "C" {
#endif

/* include udptest head file */

#include "Diag_UDPTest.h"
#include "Common.h"

/*
#include "iflib.h"
#include "net/if.h"
#include "etherLib.h"
*/

#define FE_AUTO_SEND_PRI 120
#define FE_AUTO_REPLY_PRI 200
#define UDP_TEST_STACK  0x20000

BOOL g_bUDPLoopRunFlag = FALSE;
SOCKET g_Socket = VOS_ERROR;

/* 最多允许同时测试10个UDP端口*/
#define MAX_UDP_TEST    10
CUdpTest g_UdpTestPort[MAX_UDP_TEST];

void SetUdpTraceFlag(int nUdpIndex, BOOL bFlag)
{
	if (nUdpIndex < MAX_UDP_TEST)
	{
	    g_UdpTestPort[nUdpIndex].m_bUdpTraceFlag = bFlag;
	}
	else
	{
		printf("Invalid UDP Index:%d\n", nUdpIndex);
	}
}

char g_lpszAddr[40];
char * GetAddr(struct sockaddr_in *pAddr)
{
    sprintf(g_lpszAddr, "%d.%d.%d.%d:%d", 
        (int)((pAddr->sin_addr.s_addr >> 24) & 0xff),
        (int)((pAddr->sin_addr.s_addr >> 16) & 0xff),
        (int)((pAddr->sin_addr.s_addr >> 8) & 0xff),
        (int)((pAddr->sin_addr.s_addr >> 0) & 0xff),
        htons(pAddr->sin_port));

    return g_lpszAddr;
}

void * FEAutoRecvProc(void * pPoint)
{
	CUdpTest *pTest = (CUdpTest *)pPoint;
    VOS_STATUS iRet;
    unsigned char lpcBuffer[64 * 1024];
    struct sockaddr_in  sin;

    int nAddrLen = sizeof(struct sockaddr_in);
    memset((void *) &sin, 0, sizeof (struct sockaddr_in)); 

    if ((pTest->m_Socket == VOS_ERROR) || (pTest->m_bRunFlag == FALSE))
    {
        Diag_Output("FEAutoRecvProc failed for invalid socket...\n");
        return 0;
    }

    for (;;)
    {    
        if (pTest->m_bRunFlag == FALSE)
        {
            break;
        }

        iRet = recvfrom(pTest->m_Socket, (char *)lpcBuffer, 64 * 1024, 0, (struct sockaddr*)&sin, (UINT32 *)&nAddrLen);
/*        iRet = recv(pTest->m_Socket, lpcBuffer, 2000, 0);  */
        if (iRet == 0)
        {
            Diag_Output("RecvFrom return 0!\n");
            /* The Socket has been closed, so we can end the task now */
            break;
        }
        else if (iRet == VOS_ERROR)
        {
            Diag_Output("recvFrom return ERROR!\n");
            continue;
        }
        else
        {
            if (pTest->m_bAutoReplyFlag == TRUE)
            {
                SendMessage(pTest, (struct sockaddr*)&sin, lpcBuffer, iRet);
            }

            OnRecvMessage(pTest, (struct sockaddr*)&sin, lpcBuffer, iRet); 
        }
        
     }

/*    Diag_Output("FEAutoRecvProc end... this = %d\n", pTest->m_Socket);  */
	return (0);
}


void InitUDPTest(CUdpTest *pUdpTest)
{	
    UINT32 iIndex;

    pUdpTest->m_bRunFlag = FALSE;         
    pUdpTest->m_bAutoReplyFlag = FALSE;    
    pUdpTest->m_bUdpTraceFlag = FALSE;

    clearstatdata(pUdpTest);
	pUdpTest->m_Socket = VOS_ERROR;

	for (iIndex = 0; iIndex < 2000; iIndex++)
	{
        pUdpTest->m_pBuffer[iIndex] = (UINT8)iIndex;
	}
	pUdpTest->m_pDispatchMsgFunc = NULL;
}

void CloseUDPTest(CUdpTest *pUdpTest)
{
    if (pUdpTest->m_bRunFlag == TRUE)
    {
        /* Stop the socket task */
        pUdpTest->m_bRunFlag = FALSE;

        /* Wait for the task to be stop */
        /* Shut down the socket */
        usleep(1000);

        closesocket(pUdpTest->m_Socket);

	    /* Shut down the socket */
	    shutdown(pUdpTest->m_Socket, 2);
	    pUdpTest->m_Socket = VOS_ERROR;
    }
    pUdpTest->m_bUdpTraceFlag = FALSE;

}

void OnRecvMessage(CUdpTest *pUdpTest, struct sockaddr *pAddr, UINT8 *lpszBuffer, UINT32 ulLen)
{
    /* You can add your code here...  */
    struct sockaddr_in *pSin;

    pSin = (struct sockaddr_in *)pAddr;

    if (pUdpTest->m_bUdpTraceFlag == TRUE)
    {
        Diag_Output("Recv from %s,  %d Bytes: %s...\n", GetAddr(pSin), ulLen, FormatString(lpszBuffer, (ulLen > 10) ? 10: ulLen));
    }

	pUdpTest->m_ulRecvPacketNum++;

    if (pUdpTest->m_pDispatchMsgFunc != NULL)
    {
		pUdpTest->m_pDispatchMsgFunc(pUdpTest, pAddr, lpszBuffer, ulLen);
    }
    return;
}

VOS_STATUS SendMessage(CUdpTest *pUdpTest, struct sockaddr * pSin, UINT8 *lpszBuffer, UINT32 ulLen)
{
    INT32 iRet;
    int nCount;
    int nLen = sizeof(struct sockaddr);
    
    if (pUdpTest->m_bRunFlag == FALSE)
    {
        Diag_Output("SendMessage Error for m_bFlag is FALSE!\n");
        return VOS_ERROR;
    }
    else
    {
        for (nCount = 0; nCount < 10; nCount ++)
        {
            iRet = sendto(pUdpTest->m_Socket, (char *)lpszBuffer, ulLen, 0, pSin, nLen);
            if (iRet != (INT32) ulLen)
            {
                usleep(1);
            }
            else
            {
                break;
            }
        }
        if (nCount == 10)
        {
            Diag_Output("Send data failed!\n");
            return (VOS_ERROR);
        }

        if (pUdpTest->m_bUdpTraceFlag == TRUE)
        {
	        Diag_Output("Send to %s,  %d Bytes: %s...\n", GetAddr((struct sockaddr_in *)pSin), ulLen, FormatString(lpszBuffer, (ulLen > 10) ? 10: ulLen));
        }
        
		pUdpTest->m_ulSendPacketNum++;
        return (VOS_OK);
    }
}

VOS_STATUS Run(CUdpTest * pUdpTest, char *lpszName, char *strLocalIpAddr, UINT32 ulLocalPort)
{
	struct sockaddr_in  sin;
    int nLen = sizeof (struct sockaddr_in), nNameLen;
    char lpszRecvTaskName[20], lpszSendTaskName[20];
    UINT32 ulSendBuffer = 128 * 1024;
    UINT32 ulRecvBuffer = 128 * 1024;
    UINT32 ulSize = 4;


	nNameLen = strlen(lpszName);
	if (nNameLen > 9)
	{
		nNameLen = 9;
	}
	memcpy(pUdpTest->m_lpszName, lpszName, nNameLen);
	pUdpTest->m_lpszName[nNameLen] = 0;

    if (pUdpTest->m_bRunFlag == TRUE)
    {
        Diag_Output("Socket has been initialized!\n");
        return VOS_ERROR;
    }

	pUdpTest->m_Socket = socket(AF_INET, SOCK_DGRAM, 0);
	if (pUdpTest->m_Socket == VOS_ERROR)
	{
        Diag_Output("Socket Initialized Failed!\n");
        return VOS_ERROR;
	}

    /* Bind the IP Address */
    if ((strLocalIpAddr == NULL) || (strlen(strLocalIpAddr) == 0))
    {
        pUdpTest->m_ulLocalIp = 0;
    }
    else
    {
        pUdpTest->m_ulLocalIp = inet_addr(strLocalIpAddr);
    }

    /* Bind the Port */
    pUdpTest->m_ulLocalPort = ulLocalPort;

    if (pUdpTest->m_ulLocalIp == (UINT32)VOS_ERROR)
    {
        Diag_Output("Local IP Address:%s error!\n", strLocalIpAddr);
        return VOS_ERROR;
    }

    /* Save the peer port and IP */
    memset((char *) &(pUdpTest->m_SendPeerAddr), 0, sizeof (struct sockaddr_in)); 

    pUdpTest->m_SendPeerAddr.sin_family      = AF_INET;
	
    memset ((char *) &sin, 0, sizeof (struct sockaddr_in)); 
    sin.sin_family      = AF_INET;
    sin.sin_addr.s_addr = pUdpTest->m_ulLocalIp;/*  htonl(m_ulLocalIp);  */
    sin.sin_port        = htons(pUdpTest->m_ulLocalPort);

    if (bind(pUdpTest->m_Socket, (struct sockaddr *)&sin, sizeof(struct sockaddr_in)) != VOS_OK)
    {
        Diag_Output("Socket bind failed! errno = %d\n", errno);
        closesocket(pUdpTest->m_Socket);
        return VOS_ERROR;
    }

    setsockopt(pUdpTest->m_Socket, SOL_SOCKET, SO_SNDBUF, (char *)&ulSendBuffer, ulSize);
    setsockopt(pUdpTest->m_Socket, SOL_SOCKET, SO_RCVBUF, (char *)&ulRecvBuffer, ulSize);
    
    getsockname(pUdpTest->m_Socket, (struct sockaddr *)&sin, (UINT32 *)&nLen);
	pUdpTest->m_ulLocalPort = htons(sin.sin_port);
	pUdpTest->m_ulLocalIp = sin.sin_addr.s_addr;

    /* start the task */
    pUdpTest->m_bRunFlag = TRUE;

    clearstatdata(pUdpTest);

    sprintf(lpszSendTaskName, "tSend%d", pUdpTest->m_Socket);
    sprintf(lpszRecvTaskName, "tRecv%d", pUdpTest->m_Socket);


	pthread_t pid;
	pthread_create(&pid, NULL, FEAutoRecvProc, pUdpTest);

//	taskSpawn (lpszRecvTaskName, FE_AUTO_REPLY_PRI, 0, 0x20000, (FUNCPTR)FEAutoRecvProc, 
//                            (int)pUdpTest,0,0,0,0, 0,0,0,0,0);

     return VOS_OK;
}

void getstatdata(CUdpTest *pUdpTest, StatData_s *pStatData)
{
	pStatData->m_ulSendPacketNum	= pUdpTest->m_ulSendPacketNum;
	pStatData->m_ulRecvPacketNum	= pUdpTest->m_ulRecvPacketNum;
}

void   clearstatdata(CUdpTest *pUdpTest)
{
	pUdpTest->m_ulSendPacketNum = 0;
	pUdpTest->m_ulRecvPacketNum = 0;
}

void SetDispatchMessageFunc(CUdpTest *pUdpTest, RecvCallBack pDispatchMsgFunc)
{
    pUdpTest->m_pDispatchMsgFunc = pDispatchMsgFunc;
}

UINT32 GetLocalPort(CUdpTest *pUdpTest)
{
	return (pUdpTest->m_ulLocalPort);
}

/* 开始UDP测试，返回测试的ID号。如果为0XFFFFFFFF表示失败*/
UINT32 BeginUdpTest(char *lpszName, char *lpszLocalAddress, UINT32 ulLocalPort)
{
    VOS_STATUS iRet;
    UINT8 ucIndex;
    CUdpTest *pUdpTest;
    for (ucIndex = 0; ucIndex < MAX_UDP_TEST; ucIndex++)
    {
        if (g_UdpTestPort[ucIndex].m_bRunFlag == TRUE)
        {
            continue;
        }
        else
        {
        	pUdpTest = &(g_UdpTestPort[ucIndex]);
        	InitUDPTest(pUdpTest);
            iRet = Run(pUdpTest, lpszName, lpszLocalAddress, ulLocalPort);
            if (iRet == VOS_ERROR)
            {
                return (0XFFFFFFFF);
            }
            else
            {
                SetAutoReply(ucIndex, 1);
                return (ucIndex);
            }
        }
    }
    return (0XFFFFFFFF);
}

VOS_STATUS EndUdpTest(UINT32 ulTestID)
{
    CUdpTest *pUdpTest;

    if (ulTestID >= MAX_UDP_TEST)
    {
        Diag_Output("Invalid Udp Test ID:%d\n", ulTestID);
        return VOS_ERROR;
    }
    pUdpTest = &(g_UdpTestPort[ulTestID]);
    
    if (pUdpTest->m_bRunFlag == FALSE)
    {
        Diag_Output("Udp Test ID is not running!\n");
        return VOS_ERROR;
    }

    CloseUDPTest(pUdpTest);
    return VOS_OK;
}

VOS_STATUS SetAutoReply(UINT32 ulTestID, BOOL bFlag)
{
    CUdpTest *pUdpTest;
    
    if (ulTestID >= MAX_UDP_TEST)
    {
        Diag_Output("Invalid Udp Test ID:%d\n", ulTestID);
        return VOS_ERROR;
    }
    
    if (g_UdpTestPort[ulTestID].m_bRunFlag == FALSE)
    {
        Diag_Output("Udp Test ID is not running!\n");
        return VOS_ERROR;
    }

    pUdpTest = (CUdpTest *)(&g_UdpTestPort[ulTestID]);

    pUdpTest->m_bAutoReplyFlag = bFlag;
    return VOS_OK;
}


VOS_STATUS SendUdpData(UINT32 ulTestID, char *lpszPeerIp, UINT32 ulPeerPort, UINT8 *pucBuffer, UINT32 ulLength)
{
	CUdpTest *pUdpTest;
	struct sockaddr_in  SendPeerAddr;
	UINT32 ulPeerIpAddr;
	VOS_STATUS iRet;

    if (ulTestID >= MAX_UDP_TEST)
    {
        Diag_Output("Invalid Udp Test ID:%d\n", ulTestID);
        return VOS_ERROR;
    }
    
    if (g_UdpTestPort[ulTestID].m_bRunFlag == FALSE)
    {
        Diag_Output("Udp Test ID is not running!\n");
        return VOS_ERROR;
    }

    ulPeerIpAddr = inet_addr(lpszPeerIp);
    if (ulPeerIpAddr == (UINT32)VOS_ERROR)
    {
        Diag_Output("Invalid IP Address:%s\n", lpszPeerIp);
        return VOS_ERROR;
    }

    /* Save the peer port and IP */
    memset((char *) &SendPeerAddr, 0, sizeof (struct sockaddr_in)); 

    SendPeerAddr.sin_family      = AF_INET;
	
    memset((char *) &SendPeerAddr, 0, sizeof (struct sockaddr_in)); 
    SendPeerAddr.sin_family      = AF_INET;
    SendPeerAddr.sin_addr.s_addr = ulPeerIpAddr;
    SendPeerAddr.sin_port        = htons(ulPeerPort);

	pUdpTest = &(g_UdpTestPort[ulTestID]);
    iRet = SendMessage(pUdpTest, (struct sockaddr *)(&SendPeerAddr), pucBuffer, ulLength);
    return (iRet);
}

VOS_STATUS ClearStatData(UINT32 ulTestID)
{
	CUdpTest *pUdpTest;
    if (ulTestID >= MAX_UDP_TEST)
    {
        Diag_Output("Invalid Udp Test ID:%d\n", ulTestID);
        return VOS_ERROR;
    }
    
	pUdpTest = &(g_UdpTestPort[ulTestID]);
    clearstatdata(pUdpTest);
    return VOS_OK;
}

VOS_STATUS GetStatData(UINT32 ulTestID, StatData_s *pStatData)
{
	CUdpTest *pUdpTest;
    if (ulTestID >= MAX_UDP_TEST)
    {
        Diag_Output("Invalid Udp Test ID:%d\n", ulTestID);
        return VOS_ERROR;
    }
    
	pUdpTest = &(g_UdpTestPort[ulTestID]);
    getstatdata(pUdpTest, pStatData);
    return VOS_OK;
}

VOS_STATUS SetMessageDispatchFunc(UINT32 ulTestID, RecvCallBack pDispatchMsgFunc)
{
	CUdpTest *pUdpTest;
    if (ulTestID >= MAX_UDP_TEST)
    {
        Diag_Output("Invalid Udp Test ID:%d\n", ulTestID);
        return VOS_ERROR;
    }

	pUdpTest = &(g_UdpTestPort[ulTestID]);

    SetDispatchMessageFunc(pUdpTest, pDispatchMsgFunc);
	return VOS_OK;
}

void ShowUdpStat()
{
    UINT32 ulIndex;
    StatData_s StatData;

    Diag_Output("\n");
    Diag_Output("Index     Name      Send(Frame)       Recv(Frame)\n");
    for (ulIndex = 0; ulIndex < MAX_UDP_TEST; ulIndex++)
    {
        if (g_UdpTestPort[ulIndex].m_bRunFlag == FALSE)
        {
            continue;
        }
        GetStatData(ulIndex, &StatData);
        Diag_Output("  %d  %10s     %10d    %10d\n", ulIndex, g_UdpTestPort[ulIndex].m_lpszName, StatData.m_ulSendPacketNum, StatData.m_ulRecvPacketNum);
    }
}

void ClearUdpStat()
{
    UINT32 ulIndex;

    for (ulIndex = 0; ulIndex < MAX_UDP_TEST; ulIndex++)
    {
        if (g_UdpTestPort[ulIndex].m_bRunFlag == FALSE)
        {
            continue;
        }

        ClearStatData(ulIndex);
    }
}

void UdpForwardFunc(CUdpTest *pUdpTest, struct sockaddr* pAddr, UINT8 *lpszBuffer, UINT32 ulLen)
{
	struct sockaddr_in  sin;

    memset ((char *) &sin, 0, sizeof (struct sockaddr_in)); 
    sin.sin_family      = AF_INET;
    sin.sin_addr.s_addr = pUdpTest->m_ulRemoteIp; /*  htonl(m_ulLocalIp);  */
    sin.sin_port        = htons(pUdpTest->m_ulRemotePort);

    SendMessage((CUdpTest *)pUdpTest, (struct sockaddr*)&sin, lpszBuffer, ulLen);
}

/* 开始UDP测试，返回测试的ID号。如果为0XFFFFFFFF表示失败*/
UINT32 BeginUdpForward(char *lpszLocalAddress, UINT32 ulLocalPort, char *lpszRemoteAddress, UINT32 ulRemotePort)
{
    VOS_STATUS iRet;
    UINT8 ucIndex;
    CUdpTest *pUdpTest;

    UINT32 ulRemoteIp, ulLocalIp;

    /* 检查Local Address是否正确 */
    ulLocalIp = inet_addr(lpszLocalAddress);
    if (ulLocalIp == (UINT32)VOS_ERROR)
    {
        printf("Invalid local address: %s\n", lpszLocalAddress);
        return (VOS_ERROR);
    }
    
    /* 检查Remote Address是否正确 */
    ulRemoteIp = inet_addr(lpszRemoteAddress);
    if (ulRemoteIp == (UINT32)VOS_ERROR)
    {
        printf("Invalid remote address: %s\n", lpszRemoteAddress);
        return (VOS_ERROR);
    }
    
    for (ucIndex = 0; ucIndex < MAX_UDP_TEST; ucIndex++)
    {
        if (g_UdpTestPort[ucIndex].m_bRunFlag == TRUE)
        {
            continue;
        }
        else
        {
        	pUdpTest = &(g_UdpTestPort[ucIndex]);
        	InitUDPTest(pUdpTest);
        	pUdpTest->m_ulRemoteIp = ulRemoteIp;
        	pUdpTest->m_ulRemotePort = ulRemotePort;
        	
            iRet = Run(pUdpTest, "", lpszLocalAddress, ulLocalPort);
            if (iRet == VOS_ERROR)
            {
                return (0XFFFFFFFF);
            }
            else
            {
                SetAutoReply(ucIndex, 0);
                SetMessageDispatchFunc(ucIndex, (RecvCallBack)UdpForwardFunc);

                return (ucIndex);
            }
        }
    }
    return (0XFFFFFFFF);
}

VOS_STATUS EndUdpForward(UINT32 ulTestID)
{
    CUdpTest *pUdpTest;

    if (ulTestID >= MAX_UDP_TEST)
    {
        Diag_Output("Invalid Udp Test ID:%d\n", ulTestID);
        return VOS_ERROR;
    }
    pUdpTest = &(g_UdpTestPort[ulTestID]);
    
    if (pUdpTest->m_bRunFlag == FALSE)
    {
        Diag_Output("Udp Test ID is not running!\n");
        return VOS_ERROR;
    }

    CloseUDPTest(pUdpTest);
    return VOS_OK;    
}


#ifdef __cplusplus
}
#endif




