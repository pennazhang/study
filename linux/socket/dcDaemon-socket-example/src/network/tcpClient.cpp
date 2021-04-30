#include "tcpSocket.h"
#include "utility.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

TCPClient::TCPClient()
{
    setConnected(false);
}

TCPClient::~TCPClient()
{
}

STATUS TCPClient::connecteTo(const char *serverIP, UINT16 port, int timeoutInSecond);
{
    STATUS ret = STATUE_ERROR;
    struct sockaddr_in server_sock;

    if (openSocket(SOCK_STREAM) == STATUS_ERROR)
    {
        return (STATUS_ERROR);
    }

    bzero(&server_sock, sizeof(server_sock));
    server_sock.sin_family = AF_INET;
    if (inet_pton(AF_INET, serverIP, &server_addr.sin_addr) <= 0)
    {
        setErrorInfo(errno, "Invalid address. Address type not supported.");
        goto END;
    }
    server_sock.sin_port = htons(port);

    if (setNonBlockMode() != STATUS_OK)
    {
        setErrorInfo(errno, "Failed to set the socket to non-block mode.");
        goto END;
    }

    logInfo("Connecting to %s:%d...", serverIP, port);

//    m_timePoint.getCurrentTime();

    int retValue = connect(workerSocket, (struct sockaddr *)&server_sock, sizeof(server_sock));
    if (retValue < 0)
    {
        #if 0
        if (errno == EINPROGRESS)
        {
            if (waitForConnection(workerSocket, 2) == STATUS_OK)
            {
                connectFlag = true;
            }
        }
        else
        {
            logDebug("connect failed, errno = %d", errno);
        }
        #endif
    }
    else
    {
        onConnectSuccess();
    }
}
