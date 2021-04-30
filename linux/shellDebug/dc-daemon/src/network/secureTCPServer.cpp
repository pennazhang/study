#include <stdlib.h> // exit()
#include <string.h> // memset()
#include <unistd.h> // close()
#include <sys/socket.h>
#include <netinet/in.h> // IPPROTO_TCP, htons(), htons()
#include <arpa/inet.h> // inet_addr()
#include <fcntl.h>
#include "message.h"
#include "main.h"
#include "secureTCPServer.h"

SecureTCPServer::SecureTCPServer() : TCPServer()
{
}

SecureTCPServer::~SecureTCPServer() 
{
    closeSocket();
}

STATUS SecureTCPServer::listenAt(UINT16 port)
{
    return (listenAt("0.0.0.0", port));
}

STATUS SecureTCPServer::listenAt(const char *address, UINT16 port)
{
    return TCPServer::listenAt(address, port);
}

/*---------------------------------------------------------------------------*
 * Routine: checkIncomingConnection
 *---------------------------------------------------------------------------*/
/**
 *  Check if new incoming socket is connected to this server. This function must be called periodically, for example every 100ms.
 *
 *  @return socket ID if there is new incoming socket.
 *          or NULL if there is no new incoming socket. 
 */
/*---------------------------------------------------------------------------*/
SecureTCPSocket * SecureTCPServer::checkIncomingConnection() 
{ 
    struct sockaddr_in client;        
    socklen_t client_addr_len = sizeof(client);     
    
    int workerSocket = accept(m_socket, (struct sockaddr *)&client, &client_addr_len);
    if (workerSocket > 0)        
    {
        SecureTCPSocket *sslSocket = new SecureTCPSocket();
        if (sslSocket->setSocket(workerSocket) == STATUS_OK)
        {
            sslSocket->setConnectStatus(STATUS_CONNECTED);
            return (sslSocket);
        }
        else
        {
            delete sslSocket;
            return (NULL);
        }
    }
    return (NULL);
}
