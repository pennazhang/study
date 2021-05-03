#include "tcpServer.h"
#include <stdlib.h> // exit()
#include <string.h> // memset()
#include <unistd.h> // close()
#include <sys/socket.h>
#include <netinet/in.h> // IPPROTO_TCP, htons(), htons()
#include <arpa/inet.h> // inet_addr()
#include <fcntl.h>
#include "message.h"
#include "main.h"

TCPServer::TCPServer() : Socket()
{
}

TCPServer::~TCPServer() 
{
    closeSocket();
}

STATUS TCPServer::listenAt(UINT16 port)
{
    return (listenAt("0.0.0.0", port));
}

//-
//- bind the TCP Server with the address:port
//-
STATUS TCPServer::listenAt(const char *address, UINT16 port)
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

    if (openSocket(SOCK_STREAM) != STATUS_OK)
    {
        goto END;
    }
//    logInfo("socket = %d", m_socket);

    /* jetstream didn't support select function for "accept" in Block mode, so we have to use non-block mode for accept function.  */
    if ((fcntl(m_socket, F_SETFL, O_NONBLOCK)) < 0)
    {
        setErrorInfo(errno, "Failed to set the socket to non-block mode.");
        goto END;
    }    

    if (bind(m_socket,  (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)   
    {
        setErrorInfo(errno, "Cannot bind the socket.");
        closeSocket();
        goto END;
    }

    if (listen(m_socket, 5) < 0)
    {
        setErrorInfo(errno, "Socket listen failed.");
        closeSocket();
        goto END;
    }

    ret = STATUS_OK;

END:
    return (ret); 
}

//-
//- To check if there is an incoming TCP Socket. 
//- If yes, accept it and return the TCP Socket.
//- If no, just return NULL.
//-
TCPSocket * TCPServer::checkIncomingConnection() 
{ 
    struct sockaddr_in client;        
    socklen_t client_addr_len = sizeof(client);     
    
    int workerSocket = accept(m_socket, (struct sockaddr *)&client, &client_addr_len);
    if (workerSocket > 0)        
    {
        TCPSocket *tcpSocket = new TCPSocket(workerSocket);
        tcpSocket->setConnectStatus(STATUS_CONNECTED);
        return (tcpSocket);
    }
    return (NULL);
}
