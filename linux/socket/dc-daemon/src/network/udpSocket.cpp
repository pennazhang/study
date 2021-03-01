#include <udpSocket.h>
#include <string.h>
#include "utility.h"
#include "channelIndex.h"

UDPSocket::UDPSocket() : Socket()
{
}

UDPSocket::~UDPSocket()
{
    closeSocket();
}

STATUS UDPSocket::listenAt(UINT16 port)
{
    return (listenAt("0.0.0.0", port));
}

STATUS UDPSocket::listenAt(const char *address, UINT16 port)
{
    struct sockaddr_in server_addr;
    STATUS ret = STATUS_ERROR;

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, address, &server_addr.sin_addr) <= 0)
    {
        setErrorInfo(errno, "Invalid address. Address type not supported.");
        goto END;
    }

    m_address = std::string(address);
    m_localPort = port;

    if (openSocket(SOCK_DGRAM) != STATUS_OK)
    {
        goto END;
    }
    logInfo("socket = %d", m_socket);

    /* jetstream didn't support select function for "accept" in Block mode, so we have to use non-block mode for accept function.  */
    //    if (setNonBlockMode() != STATUS_OK)
    //    {
    //        setErrorInfo(errno, "Failed to set the socket to non-block mode.");
    //        closeSocket();
    //        goto END;
    //    }

    if (bind(m_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        setErrorInfo(errno, "Cannot bind the socket.");
        closeSocket();
        goto END;
    }

    ret = STATUS_OK;

END:
    return (ret);
}

SocketEventType UDPSocket::checkEvent()
{
    if (m_socket < 0)
    {
        logInfo("socket is not start.")
//        listenAt(m_address, m_localPort);
        return (SOCKET_EVENT_NONE);
    }

    if (isFdReadable(m_socket, 1))
    {
        socklen_t addressSize = sizeof(sockaddr_in);
        // int oldEfficientLen = m_efficientLen;
        memset((void *)&m_peerAddr, 0, sizeof(sockaddr_in));
        int recvLen = recvfrom(m_socket, m_buffer, MAX_UDP_FRAME_SIZE, 0, (sockaddr *)&m_peerAddr, &addressSize);
        if (recvLen <= 0)
        {
            /* connection is borken */
            setErrorInfo(-1, "Socket is borken.");
            return (SOCKET_EVENT_UDP_BROKEN);
        }
        else
        {
            m_efficientLen = recvLen;
        }
        return (SOCKET_EVENT_RECV_DATA);
    }
    return (SOCKET_EVENT_NONE);
}

BOOL UDPSocket::getReceivedData(ByteArray &byteArray)
{
    byteArray.resize(m_efficientLen + 1);
    UINT8 *pData = byteArray.data();
    memcpy(pData, m_buffer, m_efficientLen);
    pData[m_efficientLen] = 0;
    return (true);
}


void UDPSocket::sendTo(sockaddr_in& peerAddr, UINT8 *pData, int byteLength)
{
    sendto(m_socket, pData, byteLength, 0, (sockaddr*)&peerAddr, sizeof(sockaddr_in));
}

int UDPSocket::getChannelID()
{
    ChannelParamType channelType;
    channelType.m_channelType = CHANNEL_UDP;
    channelType.m_param.m_udpParam.m_objectPointer = this;
    memcpy((void *)&channelType.m_param.m_udpParam.m_sockAddr, &m_peerAddr, sizeof(sockaddr_in));
    channelType.m_param.m_udpParam.m_objectPointer = this;
    return g_channelIndex.getChannelIndex(channelType); 
}
