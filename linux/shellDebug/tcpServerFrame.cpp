
#include "tcpServerFrame.h"
#include "commonLib.h"
#include "shellFrame.h"
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h> // exit()
#include <string.h> // memset()
#include <unistd.h> // close()
#include <sys/socket.h>
#include <netinet/in.h> // IPPROTO_TCP, htons(), htons()
#include <arpa/inet.h> // inet_addr()
#include <pthread.h>
#include <sys/select.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>

static pthread_t s_tcpServerThread;
static bool s_exitFlag = true;
static int s_serverSocket = -1;

void *workerSocketProc(void * arg)
{
    int recv_len = 0;
    char recv_buf[1024] = "";
    int clientfd = (long)arg;
	struct timeval timeout;
	ULONG	ulResult;
	int fd = dup(STDOUT_FILENO);	// save the standard output fd
	
	dup2(clientfd, STDOUT_FILENO); 

	/* Print Info */
	{
		printf("sizeof(void *) = %d\n", (int)sizeof(void *));
		printf("sizeof(long) = %d\n", (int)sizeof(LONG));

		int sizeOfInt = sizeof(UINT32);
		if (sizeOfInt != 4)
		{
			printf("sizeof(int) = %d, Please redefine UINT32 and rebuild the program again!!!\n", sizeOfInt);
			goto END;
		}
		else
		{
			printf("sizeof(int) = %d, Check OK!\n", sizeOfInt);
		}

		int sizeOfShort = sizeof(UINT16);
		if (sizeOfShort != 2)
		{
			printf("sizeof(short int) = %d, Please redefine UINT16 and rebuild the program again!!!\n", sizeOfShort);
			goto END;
		}
		else
		{
			printf("sizeof(short int) = %d, Check OK!\n", sizeOfShort);
		}

		printf("\n");
	}
	
    do
    {
        fd_set readFDs;
        FD_ZERO(&readFDs);
        FD_SET(clientfd, &readFDs);
		timeout.tv_sec = 0;
		timeout.tv_usec = 1000;	/* timeout = 1ms */

        if (select(clientfd + 1, &readFDs, NULL, NULL, &timeout) > 0)
        {
            // some socket event was triggered, check which one
            if (FD_ISSET(clientfd, &readFDs))
            {
                recv_len = recv(clientfd, recv_buf, sizeof(recv_buf), 0);
                if (recv_len <= 0)
                {
                    printf("client:%d socket closed!\n", clientfd);
					break;
                }
				recv_buf[recv_len] = 0;

				replaceString(recv_buf, "\t\r\n", ' ');
				trimStringAtHead(recv_buf);
				trimStringAtTail(recv_buf);
				printf("->%s\n", recv_buf);
				if (strcmp(recv_buf, "exit") == 0)
				{
					break;
				}
				
				/* Explain the cmd */
				if (explainCmd(recv_buf, &ulResult) == STATUS_OK)
				{
					printf("\nExecute function successfully, Result = 0x%lX = %d\n\n", ulResult, (signed int)ulResult);
				}
				else
				{
#ifndef RUN_SYSTEM_WHEN_POSSIBLE 				
					printf("Execute function failed!\n\n");
#else
					int result = system(recv_buf);
//					printf("result = %d\n", result);
					if (result != 0)
					{
						printf("Execute command failed for: %s\n\n", recv_buf);
					}
					else
					{
						printf("\n");
					}
#endif
				}
            }
        }

		if (s_exitFlag == true)
		{
			printf("quit workerSocketProc\n");
			break;
		}

//	We didn't need to sleep because select timeout is 1ms.
//		mSleep(1);
   	}while (1);

END:
	dup2(fd, STDOUT_FILENO);
	close(clientfd);
	return (NULL);
}

void* serverSocketProc(void *port)
{
    int tcpPort = (long)port;
    struct sockaddr_in server_addr;

    s_serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (s_serverSocket < 0)
    {
        printf("Create socket failed!\n");
        return ((void *) -1);
    }

    int opt = 1;
    setsockopt(s_serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(tcpPort);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(s_serverSocket, (struct sockaddr *)&server_addr,
             sizeof(server_addr)) < 0)
    {
        printf("Socket bind failed!\n");
        goto END;
    }

    if (listen(s_serverSocket, 10) < 0)
    {
        printf("Socket listen failed!\n");
        goto END;
    }

    printf("Listening at port: %d...\n", tcpPort);

    while (1)
    {
		struct timeval timeout;
        fd_set readFDs;
		pthread_t thread_id;
        struct sockaddr_in client;
        socklen_t client_addr_len = sizeof(client);

		if (isFdReadable(s_serverSocket, 1000))
        {
			printf("server socket is readable\n");
			int workerSocket = accept(s_serverSocket, (struct sockaddr *)&client, &client_addr_len);
			if (workerSocket > 0)
			{
				char cli_ip[40] = "";
				inet_ntop(AF_INET, &client.sin_addr, cli_ip, INET_ADDRSTRLEN);
				printf("accept socket = %d, client ip=%s, port=%d\n", workerSocket, cli_ip,
					   ntohs(client.sin_port));
			
				pthread_create(&thread_id, NULL, workerSocketProc, (void *)(long)workerSocket);
				pthread_detach(thread_id);
			}
		}

        if (s_exitFlag == true)
        {
            break;
        }
    }

END:
	close(s_serverSocket);
	s_serverSocket = -1;
    return ((void *)0);
}

bool startShellThread(int tcpPort)
{
    int ret;
    int arg = 10;
    int *thread_ret = NULL;

	if ((s_exitFlag == false) && (isThreadAlive(s_tcpServerThread)))
	{
		printf("-- Warning: startShellThread failed for shell thread is already running!\n");
		return (false);
	}
	
    s_exitFlag = false;
	addBasicFuncMap();
    ret = pthread_create(&s_tcpServerThread, NULL, serverSocketProc, (void *)(long)tcpPort );

//    printf("startShellThread called here, ret = %d\n", ret);
    return (true);
}


int endShellThread()
{
    void *ret;

    printf("endShellThread called here...\n");

    s_exitFlag = true;
    pthread_join(s_tcpServerThread, &ret);

    printf("Quit shell thread: %ld\n", (long)ret);

    return ((long)ret);
}



