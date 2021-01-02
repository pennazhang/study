#include <stdio.h>
#include <curses.h>
#include <unistd.h>
#include <sys/socket.h>
#include <string.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/select.h> 
#include <sys/time.h> 

#define SERVER_PORT 50021

/*
	Function: check that if some data is received by the socket.
	-1:  socket error.
	0: no data can be read from socket.
	1: data can be read from socket.
*/
int isSocketReadable(int socket)
{
	//use select function to judge that if the block socket is readable.
	int rc;
	fd_set fds;
	struct timeval tv = {0, 0};
	
	FD_ZERO(&fds);
	FD_SET(socket, &fds);
	rc = select(socket + 1, &fds, NULL, NULL, &tv);
	if (rc < 0) //error
	{
		return -1;
	}
	return FD_ISSET(socket, &fds) ? 1 : 0;
}

int main(int argc,char* argv[])
{
	int nLen = 0;
	
    if(argc!=2)                                                                                            
    {   
        printf("Usage: %s server_IP\n", argv[0]);
        return 1;
    }   
    char *str=argv[1];
    char sendBuf[1024];
	char recvBuf[1024];
	int sendCount = 0;

	struct sockaddr_in server_sock;
    int sock = socket(AF_INET,SOCK_STREAM,0);
    bzero(&server_sock,sizeof(server_sock));
    server_sock.sin_family=AF_INET;
    inet_pton(AF_INET,str,&server_sock.sin_addr);
    server_sock.sin_port=htons(SERVER_PORT);

    int ret= connect(sock, (struct sockaddr *)&server_sock, sizeof(server_sock));
    if (ret < 0)
    {
        printf("connect() failed!\n");
		close(sock);
        return 1;
    }
    printf("connect success, type command: \"quit\" to exit!\n");
	
    while(1)
    {
		usleep(1000);
		
		int ret = isSocketReadable(sock);
		if (ret == -1)
		{
			printf("socket is broken!\n");
			break;
		}
		else if (ret == 1)
		{
			nLen = recv(sock, recvBuf, sizeof(recvBuf), 0);
			if (nLen <= 0)
			{
				break;
			}
			recvBuf[nLen] = 0;
			printf("recv %d bytes: %s\n", nLen, recvBuf);
		}

		ret = isSocketReadable(STDIN_FILENO);
		if (ret == 1)
		{
			sendCount = read(STDIN_FILENO, sendBuf, sizeof(sendBuf));
			if (sendCount > 1)
			{
				// 去掉结尾的\n
				sendCount--;
				sendBuf[sendCount] = 0;
				printf("send %d bytes: %s\n", sendCount, sendBuf);
				send(sock, sendBuf, sendCount, 0);
				
				usleep(10000);
				if(strncasecmp(sendBuf,"quit",4)==0)
				{
					printf("quit\n");
					break;
				}
			}				
		}
    }
    close(sock);
    return 0;
}
