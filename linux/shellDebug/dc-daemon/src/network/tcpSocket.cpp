#include "tcpSocket.h"
#include "utility.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <string.h>
#include "channelIndex.h"

TCPSocket::TCPSocket(int socket) : Socket(socket)
{
	m_connecteStatus = STATUS_DISCONNECTED;
    m_efficientLen = 0;
}

TCPSocket::~TCPSocket()
{
}

STATUS TCPSocket::send(std::string message)
{
    return this->send((UINT8 *)message.c_str(), message.length());
}

STATUS TCPSocket::send(UINT8 *bytes, int lengthInByte)
{
    if (m_connecteStatus != STATUS_CONNECTED)
    {
        setErrorInfo(-1, "send failed because the TCP socket is not connected");
        return (STATUS_ERROR);
    }

    int totalSent = 0;
    for (;;)
    {
        if (totalSent == lengthInByte)
        {
            break;
        }

        int sendLen = ::send(this->m_socket, bytes + totalSent, lengthInByte - totalSent, 0);
        if (sendLen <= 0)
        {
            setErrorInfo(errno, "send failed");
            return (STATUS_ERROR);
        }
        totalSent += sendLen;
    }

    return (STATUS_OK);
}

void TCPSocket::setConnectStatus(ConnectStatus status)
{
    if (m_connecteStatus != STATUS_CONNECTED)
    {
        m_efficientLen = 0;
    }
	m_connecteStatus = status;
}

ConnectStatus TCPSocket::getConnectStatus() 
{ 
	return m_connecteStatus; 
}

SocketEventType TCPSocket::checkEvent_InBlockMode()
{
	// ASSERT(m_connecteStatus == STATUS_CONNECTED);
	
    if ((m_connecteStatus != STATUS_CONNECTED) || (m_socket < 0))
    {
        setErrorInfo(-1, "Socket is not in connected status!");
        return (SOCKET_EVENT_NONE);
    }
    else
    {
		// For SSL Socket
            int ret;
            ret = recv(m_socket, m_buffer, BUFFER_SIZE, 0);
            if (ret <= 0)
            {
                /* connection is borken */
                setErrorInfo(-1, " Socket is borken.");
                return (SOCKET_EVENT_TCP_CONNECTION_BROKEN);
            }
            else
            {
                m_efficientLen = ret;
                return (SOCKET_EVENT_RECV_DATA);
            }
    }   
}

SocketEventType TCPSocket::checkEvent()
{
    if ((m_connecteStatus != STATUS_CONNECTED) || (m_socket < 0))
    {
        return (SOCKET_EVENT_NONE);
    }
    else
    {
        struct tcp_info info; 
        int len=sizeof(info); 
        getsockopt(m_socket, IPPROTO_TCP, TCP_INFO, &info, (socklen_t *)&len); 
        if (info.tcpi_state != TCP_ESTABLISHED)
        { 
            /* connection is borken */
            setErrorInfo(-1, " Socket is borken.");
            return (SOCKET_EVENT_TCP_CONNECTION_BROKEN);
        }

        if (isFdReadable(m_socket, 1))
        {
//            int oldEfficientLen = m_efficientLen;
            int ret;
            ret = recv(m_socket, m_buffer, BUFFER_SIZE, 0);
            if (ret <= 0)
            {
                /* connection is borken */
                setErrorInfo(-1, " Socket is borken.");
                return (SOCKET_EVENT_TCP_CONNECTION_BROKEN);
            }
            else
            {
                m_efficientLen = ret;
                return (SOCKET_EVENT_RECV_DATA);
            }
        }            

        else
        {
            return (SOCKET_EVENT_NONE);
        }
    }
}

int TCPSocket::getChannelID()
{
    ChannelParamType channelType;
    channelType.m_channelType = CHANNEL_TCP;
    channelType.m_param.m_tcpParam.m_objectPointer = this;
    return g_channelIndex.getChannelIndex(channelType); 
}

UINT8 *TCPSocket::getReceivedData(int& len)
{
    len = m_efficientLen;
    return (m_buffer);
}

