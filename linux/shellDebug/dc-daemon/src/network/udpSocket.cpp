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

STATUS UDPSocket::bind(UINT16 port)
{
    return (bind("0.0.0.0", port));
}

/*---------------------------------------------------------------------------*
 * Routine: UDPSocket::bind
 *---------------------------------------------------------------------------*/
/**
 * Bind the address and port.
 *
 * @param [in] address -- IP Address that bind to the UDP.
 * @param [in] port -- port that bind to the UDP.
 *
 * @return STATUS_OK if bind successfully, else STATUS_ERROR.
 *
 * <!------------------------------------------------------------------------*/
STATUS UDPSocket::bind(const char *address, UINT16 port)
{
    struct sockaddr_in server_addr;
    STATUS ret = STATUS_ERROR;
    int broadcastFlag = 1;

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
//    logInfo("socket = %d", m_socket);

    /* jetstream didn't support select function for "accept" in Block mode, so we have to use non-block mode for accept function.  */
    //    if (setNonBlockMode() != STATUS_OK)
    //    {
    //        setErrorInfo(errno, "Failed to set the socket to non-block mode.");
    //        closeSocket();
    //        goto END;
    //    }

    // Make this a broadcast socket connection
    if ((setsockopt(m_socket, SOL_SOCKET, SO_BROADCAST, &broadcastFlag, sizeof(broadcastFlag))) == -1) 
    {
        setErrorInfo(errno, "Failed to enable UDP Borad .");
        closeSocket();
        goto END;
    }

    if (::bind(m_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        setErrorInfo(errno, "Cannot bind the socket.");
        closeSocket();
        goto END;
    }

    ret = STATUS_OK;

END:
    return (ret);
}

/*---------------------------------------------------------------------------*
 * Routine: UDPSocket::checkEvent
 *---------------------------------------------------------------------------*/
/**
 * Check if there is incoming data. If yes, then received the data to the UDP's buffer.
 *
 * @return  SOCKET_EVENT_RECV_DATA if there is incoming data.
 *          SOCKET_EVENT_UDP_BROKEN if UDP is closed.
 *          SOCKET_EVENT_NONE if there is no incoming data and UDP is still working.
 * <!------------------------------------------------------------------------*/
SocketEventType UDPSocket::checkEvent()
{
    if (m_socket < 0)
    {
//        logInfo("socket is not start.")
//        listenAt(m_address, m_localPort);
        return (SOCKET_EVENT_NONE);
    }

    if (isFdReadable(m_socket, 1))
    {
        socklen_t addressSize = sizeof(sockaddr_in);
        // int oldEfficientLen = m_efficientLen;
        memset((void *)&m_peerAddr, 0, sizeof(sockaddr_in));

        /* Receive the UDP package to the buffer */
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

void UDPSocket::sendTo(sockaddr_in& peerAddr, UINT8 *pData, int byteLength)
{
//    char *ipAddr = inet_ntoa(peerAddr.sin_addr);
//    logInfo("Send UDP message to: %s:%d (len =%d)", ipAddr, htons(peerAddr.sin_port), byteLength);
//    dumpHex(pData, byteLength, 1);

    ::sendto(m_socket, pData, byteLength, 0, (sockaddr*)&peerAddr, sizeof(sockaddr_in));
}

void UDPSocket::sendTo(sockaddr_in& peerAddr, std::string message)
{
    sendTo(peerAddr, (UINT8 *)message.c_str(), (int)message.length());
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

/*---------------------------------------------------------------------------*
 * Routine: UDPSocket::getReceivedData
 *---------------------------------------------------------------------------*/
/**
 * Get the data received by UDP socket
 *
 * @param [out] len -- the length of the data
 * @return  the address of data.
 * 
 * <!------------------------------------------------------------------------*/
UINT8 *UDPSocket::getReceivedData(int& len)
{
    len = m_efficientLen;
    return (m_buffer);
}

