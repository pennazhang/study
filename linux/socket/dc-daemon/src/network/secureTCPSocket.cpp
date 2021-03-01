//####
//# Legal Notice :
//#    Copyright, Harman International, 2020
//#    Private, proprietary information, the sole property of Harman.  The
//#    contents, ideas, and concepts expressed herein are not to be disclosed
//#    except within the confines of a confidential relationship and only
//#    then on a need to know basis.
//#
//#    Any entity in possession of this Harman Software shall not, and shall not
//#    permit any other person to, disclose, display, loan, publish, transfer
//#    (whether by sale, assignment, exchange, gift, operation of law or
//#    otherwise), license, sublicense, copy, or otherwise disseminate this
//#    Harman Software.
//#
//#    This Harman Software is owned by Harman and is protected by United States
//#    copyright laws, patent laws, international treaty provisions, and/or
//#    state of Texas trade secret laws.
//#
//#    Portions of this Harman Software may, from time to time, include
//#    pre-release code and such code may not be at the level of performance,
//#    compatibility and functionality of the final code. The pre-release code
//#    may not operate correctly and may be substantially modified prior to
//#    final release or certain features may not be generally released. Harman
//#    is not obligated to make or support any pre-release code. All pre-release
//#    code is provided "as is" with no warranties.
//#
//#    This Harman Software is provided with restricted rights. Use,duplication,
//#    or disclosure by the Government is subject to restrictions as set forth
//#    in subparagraph (1)(ii) of The Rights in Technical Data and Computer
//#    Software clause at DFARS 252.227-7013 or subparagraphs (1) and (2) of
//#    the Commercial Computer Software Restricted Rights at 48 CFR 52.227-19,
//#    as applicable.
//####

#include "tcpSocket.h"
#include "utility.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <string.h>
#include "channelIndex.h"
#include "secureTCPSocket.h"
#include "sslLib.h"

SecureTCPSocket::SecureTCPSocket() : TCPSocket(), m_ssl(NULL)
{
}

SecureTCPSocket::~SecureTCPSocket() 
{
    closeSocket();
}

STATUS SecureTCPSocket::setSocket(int socket)
{
    if (socket < 0)
    {
        logError("Invalid socket");
        return (STATUS_ERROR);
    }

    if (m_socket >= 0)
    {
        closeSocket();
    }
    
    ASSERT(g_ctx != NULL);

    m_socket = socket;
    m_ssl = SSL_new(g_ctx);
    if (socket >= 0)
    {
        SSL_set_fd(m_ssl, socket);      /* set connection socket to SSL state */
    }

    if ( SSL_accept(m_ssl) == -1 )     /* do SSL-protocol accept */
    {
        ERR_print_errors_fp(stderr);
        closeSocket();
        return (STATUS_ERROR);
    }
    else
    {
        showCerts(m_ssl);        /* get any certificates */
    }
    return (STATUS_OK);
}

void SecureTCPSocket::closeSocket()
{
    if (m_ssl != NULL)
    {
        /* release SSL state */
        logInfo ("ssl is released here...");
        SSL_free(m_ssl);
        m_ssl = NULL;
    }
//    TCPSocket::closeSocket();
}


int SecureTCPSocket::send(std::string message)
{
    return this->send((UINT8 *)message.c_str(), message.length());
}

int SecureTCPSocket::send(UINT8 *bytes, int lengthInByte)
{
    if (m_connecteStatus != STATUS_CONNECTED)
    {
        setErrorInfo(-1, "send failed because the SSL socket is not connected");
        return (STATUS_ERROR);
    }

    int totalSent = 0;
    for (;;)
    {
        if (totalSent == lengthInByte)
        {
            break;
        }

        int sendLen = SSL_write(m_ssl, bytes + totalSent, lengthInByte - totalSent);
        if (sendLen <= 0)
        {
            setErrorInfo(errno, "send failed");
            return (STATUS_ERROR);
        }
        totalSent += sendLen;
    }

    return (STATUS_OK);
}

SocketEventType SecureTCPSocket::checkEvent()
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
			// For SSL Socket
            int ret = SSL_read(m_ssl, m_buffer + m_efficientLen, BUFFER_SIZE - m_efficientLen);
            if (recv <= 0)
            {
                /* connection is borken */
                ERR_print_errors_fp(stderr);
                return (SOCKET_EVENT_TCP_CONNECTION_BROKEN);
            }
            m_efficientLen += ret;

            return (SOCKET_EVENT_RECV_DATA);
        }            

        else
        {
            return (SOCKET_EVENT_NONE);
        }
    }   
}
