/*-------------------------------------------------------------------------*
 * File:  Socket.cpp - Copyright (C) 2013 All Rights Reserved - SVSi
 *-------------------------------------------------------------------------*/
/**
 *  @addtogroup Socket
 *  @{
 *  @brief	Socket System
 *
 * Handling of sockets can be tricky, but usually involve the same steps.
 * These classes attempt to make the system more modular and easy to
 * access as well as make use of threads where appropriate.
 */
/*-------------------------------------------------------------------------*/
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <unistd.h>
#include "Error.h"
#include "Socket.h"

/*---------------------------------------------------------------------------*
 * Constants:
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
 * Macros:
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
 * Prototypes:
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
 * Routine: Socket::Bind
 *---------------------------------------------------------------------------*/
/**
 * Bind a socket to the given IP and port.  Tells from which locations
 * we can accept connections.  Use an IP address of "255.255.255.255" to
 * accept from any location.
 *
 * @param [in] aIP -- IP and port address to receive from.
 *
 * @return ERROR_COULD_NOT_BIND if could not bind, else ERROR_NONE.
 *
 * <!------------------------------------------------------------------------*/
T_error Socket::Bind(IPAddress &aIP)
{
    struct sockaddr *p_addr = aIP.getSocketAddr();

    if (bind(iSock, p_addr, sizeof(struct sockaddr_storage)) != 0)
        return ERROR_COULD_NOT_BIND;

    return ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine: Socket::Listen
 *---------------------------------------------------------------------------*/
/**
 * Listen for incoming connections on the given bound socket.
 *
 * @param [in] aMaxIncoming -- Maximum number of incoming connections to
 *  support at one time.
 *
 * @return ERROR_COULD_NOT_LISTEN if could not listen, else ERROR_NONE.
 *
 * <!------------------------------------------------------------------------*/
T_error Socket::Listen(unsigned int aMaxIncoming)
{
    if (listen(iSock, aMaxIncoming) < 0)
        return ERROR_COULD_NOT_LISTEN;

    return ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine: Socket::Poll
 *---------------------------------------------------------------------------*/
/**
 * Poll to check if there is any incoming data or priority messages.
 *
 * @param [in] aTimeoutMS -- Minimum time in milliseconds to wait, 0 for
 *      no wait, or -1 for infinite timeout.
 *
 * @return ERROR_COULD_NOT_LISTEN if could not listen, else ERROR_NONE.
 *
 * <!------------------------------------------------------------------------*/
T_error Socket::Poll(int aTimeoutMS)
{
    int n;
    struct pollfd p;

    p.fd = iSock;
    p.events = POLLIN | POLLPRI;
    p.revents = 0;

    n = poll(&p, 1, aTimeoutMS);
    if (n < 0) {
        return ERROR_SYSTEM;
    } else if (n > 0) {
        if ((p.revents & POLLIN) || (p.revents & POLLPRI)) {
            return ERROR_NONE;
        }
    }

    return ERROR_TIMEOUT;
}

/*---------------------------------------------------------------------------*
 * Routine: Socket::Accept
 *---------------------------------------------------------------------------*/
/**
 * Accept an incoming connection on the given listening socket.
 *
 * @param [out] aNewSock -- Place to store the newly created socket.
 *
 * @return ERROR_COULD_NOT_ACCEPT if could not accept, else ERROR_NONE.
 *
 * <!------------------------------------------------------------------------*/
T_error Socket::Accept(Socket &aNewSock)
{
    int newsock;

    newsock = accept(iSock, NULL, NULL);
    if (newsock < 0) {
        SVSI_SYSLOG("Error with accept(): %s\n", strerror(errno));
        return ERROR_COULD_NOT_ACCEPT;
    }

    // Assigned the new socket number
    aNewSock.iSock = newsock;

    return ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine: Socket::Close
 *---------------------------------------------------------------------------*/
/**
 * Close the socket.  If not open, does not nothing.
 *
 * <!------------------------------------------------------------------------*/
void Socket::Close(void)
{
    if (iSock >= 0) {
        close(iSock);
        iSock = -1;
    }
}

/*---------------------------------------------------------------------------*
 * Routine: Socket::GetPeerIP
 *---------------------------------------------------------------------------*/
/**
 * Get the IP Address (and port) of the connected socket.
 *
 * @param [out] aPeerIP -- Place to store IP address of peer.
 *
 * @return ERROR_NONE if peer, else ERROR_COULD_NOT_FIND.
 *
 * <!------------------------------------------------------------------------*/
T_error Socket::GetPeerIP(IPAddress &aPeerIP) const
{
    struct sockaddr_storage addr;
    size_t addrlen = sizeof(addr);

    memset(&addr, 0, sizeof(addr));

    if (getpeername(iSock, (struct sockaddr *)&addr, (socklen_t *) &addrlen) == 0) {
        SVSI_DEBUG_SYSLOG("Peer family: %d\n", addr.ss_family);
        if (addr.ss_family == AF_INET) {
            // Create IPv4 version
            aPeerIP.set(*((struct sockaddr_in *)&addr));
            return ERROR_NONE;
        } else if (addr.ss_family == AF_INET6) {
            // Create IPv6 version
            aPeerIP.set(*((struct sockaddr_in6 *)&addr));
            return ERROR_NONE;
        } else {
        }
    } else {
        SVSI_DEBUG_SYSLOG("Get peer error: %s\n", strerror(errno));
    }

    // Didn't get a name!
    return ERROR_COULD_NOT_FIND;
}

/*---------------------------------------------------------------------------*
 * Routine: TCPSocket::Receive
 *---------------------------------------------------------------------------*/
/**
 * Receive bytes from the given connection.  Does not block.
 *
 * @param [out] aBuffer -- Bytes received
 * @param [in] aLength -- Maximum length of buffer
 * @param [out] aNumRead -- Number of bytes read, or 0 if not needed.
 *
 * @return ERROR_TIMEOUT if no bytes, ERROR_SYSTEM if closed or some network
 *      problem, or ERROR_NONE if bytes received.
 *
 * <!------------------------------------------------------------------------*/
T_error TCPSocket::Receive(void *aBuffer, int aLength, int *aNumRead)
{
    int num;

    // No data yet
    if (aNumRead)
        *aNumRead = 0;

    num = recv(iSock, aBuffer, aLength, MSG_DONTWAIT);

    if (num < 0) {
        // -1 is returned no data is found.  Do we see a timeout case?
        if ((errno == EAGAIN) || (errno == EWOULDBLOCK))
            return ERROR_TIMEOUT;

        return ERROR_SYSTEM;
    } else if (num > 0) {
        if (aNumRead)
            *aNumRead = num;

        // We got some data at least
        return ERROR_NONE;
    }

    // At this point, num returned is 0, meaning the socket closed
    return ERROR_CLOSED;
}

/*---------------------------------------------------------------------------*
 * Routine: Socket::Send
 *---------------------------------------------------------------------------*/
/**
 * Send bytes from through the given socket.  Blocks until buffered for
 * sending.
 *
 * @param [in] aBuffer -- Bytes to send
 * @param [in] aLength -- Number of bytes to send
 * @param [out] aNumWritten -- Number of bytes written, or 0 if not needed.
 *
 * @return ERROR_TIMEOUT if buffer full and would block, ERROR_SYSTEM if
 *      closed or some network problem, or ERROR_NONE if bytes buffered/sent.
 *
 * <!------------------------------------------------------------------------*/
T_error Socket::Send(const void *aBuffer, int aLength, int *aNumWritten)
{
    int num;

    // No data yet
    if (aNumWritten)
        *aNumWritten = 0;

    num = send(iSock, aBuffer, aLength, 0);
    if (num < 0) {
        // -1 is returned no data is found.  Do we see a timeout case?
        if ((errno == EAGAIN) || (errno == EWOULDBLOCK))
            return ERROR_TIMEOUT;

        return ERROR_SYSTEM;
    } else if (num > 0) {
        if (aNumWritten)
            *aNumWritten = num;

        // We got some data at least
        return ERROR_NONE;
    }

    // At this point, num returned is 0, meaning the socket closed
    return ERROR_CLOSED;
}

/*---------------------------------------------------------------------------*
 * Routine: TCPSocket::TCPSocket
 *---------------------------------------------------------------------------*/
/**
 * Construct a TCPSocket, but don't link to the socket system yet.
 *
 * <!------------------------------------------------------------------------*/
TCPSocket::TCPSocket()
{

}

void TCPSocket::Close(void)
{
    // Close like normal
    Socket::Close();

    // Closing a TCP socket destroys it!
    delete this;
}

T_error TCPSocket::Open(IPAddress &aDestination)
{
    if (connect(iSock, aDestination.getSocketAddr(), aDestination.getSocketAddrSize() < 0)) {
        return ERROR_COULD_NOT_OPEN;
    }

    return ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine: TCPSocket::create
 *---------------------------------------------------------------------------*/
/**
 * Create the actual system level socket for the TCPSocket and report
 * any errors.
 *
 * @return ERROR_COULD_NOT_CREATE if no more sockets, ERROR_COULD_NOT_REUSE if
 *      problem with reopening, or ERROR_NONE if properly created.
 *
 * <!------------------------------------------------------------------------*/
T_error TCPSocket::create(const IPAddress &aMyIP)
{
    int reuseOn = 1;

    // Create the socket
    if (aMyIP.isIPv6())
        iSock = socket(PF_INET6, SOCK_STREAM, 0);
    else
        iSock = socket(PF_INET, SOCK_STREAM, 0);
    if (iSock == -1)
        return ERROR_COULD_NOT_CREATE;

    // Reuse the socket even if it was already opened before
    if (setsockopt(iSock, SOL_SOCKET, SO_REUSEADDR, (const char *)&reuseOn,
            sizeof(reuseOn)) == -1)
        return ERROR_COULD_NOT_REUSE;

    // Make this socket exclusive to the process it is in
    fcntl(iSock, F_SETFD, fcntl(iSock, F_GETFD) | FD_CLOEXEC);

    return ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine: UDPSocket::UDPSocket
 *---------------------------------------------------------------------------*/
/**
 * Construct a UDPSocket, but don't link to the socket system yet.
 *
 * <!------------------------------------------------------------------------*/
UDPSocket::UDPSocket()
{
}

/*---------------------------------------------------------------------------*
 * Routine: UDPSocket::create
 *---------------------------------------------------------------------------*/
/**
 * Create the actual system level socket for the UDPSocket and report
 * any errors.
 *
 * @return ERROR_COULD_NOT_CREATE if no more sockets, ERROR_COULD_NOT_REUSE if
 *      problem with reopening, or ERROR_NONE if properly created.
 *
 * <!------------------------------------------------------------------------*/
T_error UDPSocket::create(const IPAddress &aMyIP, int aIsBroadcast)
{
    int reuseOn = 1;

    iMyIP = aMyIP;

    // Create the socket
    if (aMyIP.isIPv6())
        iSock = socket(AF_INET6, SOCK_DGRAM, 0);
    else
        iSock = socket(AF_INET, SOCK_DGRAM, 0);
    if (iSock == -1)
        return ERROR_COULD_NOT_CREATE;

    // Reuse the socket even if it was already opened before
    if (setsockopt(iSock, SOL_SOCKET, SO_REUSEADDR, (const char *)&reuseOn,
            sizeof(reuseOn)) == -1)
        return ERROR_COULD_NOT_REUSE;

    // Make this socket exclusive to the process it is in
    fcntl(iSock, F_SETFD, fcntl(iSock, F_GETFD) | FD_CLOEXEC);

    if (aIsBroadcast) {

        // Make this a broadcast socket connection
        if ((setsockopt(iSock, SOL_SOCKET, SO_BROADCAST, &aIsBroadcast,
                sizeof(aIsBroadcast))) == -1) {
            return ERROR_COULD_NOT_CREATE;
        }
    }
    return ERROR_NONE;
}

T_error UDPSocket::Receive(void *aBuffer, int aLength, int *aNumRead)
{
    int result;

    *aNumRead = 0;
    if (iMyIP.isIPv6()) {
        struct sockaddr_storage addr;
        socklen_t addrlen = sizeof(addr);
        result = recvfrom(iSock, aBuffer, aLength, MSG_DONTWAIT, (struct sockaddr *)&addr, &addrlen);
        if (result == 0)
            return ERROR_CLOSED;
        if (result < 0)
            return ERROR_COULD_NOT_READ;
        iPeerIP.set(*((sockaddr_in6 *)&addr));
        *aNumRead = result;
    } else {
        struct sockaddr_storage addr;
        memset(&addr, 0xC0, sizeof(addr));
        socklen_t addrlen = sizeof(addr);
        errno = 0;
        result = recvfrom(iSock, aBuffer, aLength, MSG_DONTWAIT, (struct sockaddr *)&addr, &addrlen);
        if (result == 0) {
            iPeerIP.set(*((sockaddr_in *)&addr));
            return ERROR_CLOSED;
        }
        if (result < 0) {
            return ERROR_COULD_NOT_READ;
        }
        iPeerIP.set(*((sockaddr_in *)&addr));
        *aNumRead = result;
    }

    return ERROR_NONE;
}

T_error UDPSocket::Send(const void *aBuffer, int aLength, int *aNumWritten)
{
    int numSent;
    struct sockaddr *p_addr = iPeerIP.getSocketAddr();
    socklen_t addrlen = sizeof(struct sockaddr_storage);

    numSent = sendto(iSock, aBuffer, aLength, MSG_DONTWAIT, p_addr, addrlen);
    if (numSent < 0)
        return ERROR_COULD_NOT_WRITE;
    if (aNumWritten)
        *aNumWritten = numSent;

    return ERROR_NONE;
}

T_error UDPSocket::SendTo(IPAddress& aDestination, const void *aBuffer, int aLength, int *aNumWritten)
{
    int numSent;
    struct sockaddr *p_addr = aDestination.getSocketAddr();
    socklen_t addrlen = sizeof(struct sockaddr_storage);

    numSent = sendto(iSock, aBuffer, aLength, MSG_DONTWAIT, p_addr, addrlen);
    if (numSent < 0)
        return ERROR_COULD_NOT_WRITE;
    if (aNumWritten)
        *aNumWritten = numSent;

    return ERROR_NONE;
}

T_error UDPSocket::GetPeerIP(IPAddress &aPeerIP) const
{
    aPeerIP = iPeerIP;
    return ERROR_NONE;
}

T_error UDPSocket::Open(IPAddress &aDestination)
{
    // UDP doesn't really connect so much as store where to go
    iPeerIP = aDestination;

    return ERROR_NONE;
}

/** @} */
/*-------------------------------------------------------------------------*
 * End of File:  Socket.cpp
 *-------------------------------------------------------------------------*/
