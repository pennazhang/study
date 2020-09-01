
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

char g_lpszPrompt[32] = "-> ";
char g_lpszUserName[32] = "";
char g_lpszPassword[32] = "";

bool dispatchCommand(char *recv_buf)
{
	ULONG	ulResult;
	replaceString(recv_buf, "\t\r\n", ' ');
	trimStringAtHead(recv_buf);
	trimStringAtTail(recv_buf);
	if (strcmp(recv_buf, "exit") == 0)
	{
		return true;
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
	//	printf("result = %d\n", result);
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
	
	return false;
}

typedef enum
{
	STEP_CONNECTED = 0,
	STEP_USER_NAME,
	STEP_PASSWORD,
	STEP_VERIFY_OK,
} CONNECTION_STEP;

void *workerSocketProc(void * arg)
{
	CONNECTION_STEP s_connectionStep = STEP_CONNECTED;
	char s_userName[64], s_password[64];
    int recv_len = 0;
    char recv_buf[1024] = "";
    int clientfd = (long)arg;
	struct timeval timeout;
	int fd = dup(STDOUT_FILENO);	// save the standard output fd
	
	dup2(clientfd, STDOUT_FILENO); 

	/* Print Info */
	{
//		printf("sizeof(void *) = %d\n", (int)sizeof(void *));
//		printf("sizeof(long) = %d\n", (int)sizeof(LONG));

		int sizeOfInt = sizeof(UINT32);
		if (sizeOfInt != 4)
		{
			printf("sizeof(int) = %d, Please redefine UINT32 and rebuild the program again!!!\n", sizeOfInt);
			goto END;
		}
//		else
//		{
//			printf("sizeof(int) = %d, Check OK!\n", sizeOfInt);
//		}

		int sizeOfShort = sizeof(UINT16);
		if (sizeOfShort != 2)
		{
			printf("sizeof(short int) = %d, Please redefine UINT16 and rebuild the program again!!!\n", sizeOfShort);
			goto END;
		}
//		else
//		{
//			printf("sizeof(short int) = %d, Check OK!\n", sizeOfShort);
//		}

//		printf("\n");

		if (strlen(g_lpszUserName) != 0)
		{
			printf("User Name: ");
			s_connectionStep = STEP_USER_NAME;
		}
		else
		{
			printf("%s", g_lpszPrompt);
			s_connectionStep = STEP_VERIFY_OK;
		}
		fflush(stdout);
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

				switch (s_connectionStep)
				{
				case STEP_USER_NAME:
					if (recv_len > 64)
					{
						printf("Inavlid User Name.\nUser Name: ");
					}
					else
					{
						memcpy(s_userName, recv_buf, recv_len);
						replaceString(s_userName, "\t\r\n", ' ');
						trimStringAtHead(s_userName);
						trimStringAtTail(s_userName);

						printf("Password: ");
						s_connectionStep = STEP_PASSWORD;
					}
					break;
					
				case STEP_PASSWORD:
					if (recv_len > 64)
					{
						printf("Inavlid Password.\r\nUser Name: ");
						s_connectionStep = STEP_USER_NAME;						
					}
					else
					{
						memcpy(s_password, recv_buf, recv_len);
						replaceString(s_password, "\t\r\n", ' ');
						trimStringAtHead(s_password);
						trimStringAtTail(s_password);

						if ((strcmp(g_lpszUserName, s_userName) != 0) || (strcmp(g_lpszPassword, s_password) != 0))
						{
							printf("Invalid user name or password!\n");
							printf("\nUser Name: ");
							s_connectionStep = STEP_USER_NAME;
						}
						else
						{
							printf("\n%s", g_lpszPrompt);
							s_connectionStep = STEP_VERIFY_OK;
						}
					}
					break;
					
				default:
					if (dispatchCommand(recv_buf) == true)
					{
						/* run command "exit", and we need to quit and close the socket. */
						goto END;
					}
					
					printf("%s", g_lpszPrompt);
					break;
				}
				fflush(stdout);
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
//		struct timeval timeout;
//        fd_set readFDs;
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

bool startShellThread(int tcpPort, const char *prompt, const char *userName, const char *password)
{
	if ((s_exitFlag == false) && (isThreadAlive(s_tcpServerThread)))
	{
		printf("-- Warning: startShellThread failed for shell thread is already running!\n");
		return (false);
	}

	if ((prompt != NULL) && (strlen(prompt) != 0) && (strlen(prompt) < 30))
	{
		strcpy(g_lpszPrompt, prompt);
	}

	if ((userName != NULL) && (strlen(userName) != 0) && (strlen(userName) < 30)
		&& (password != NULL) && (strlen(password) != 0) && (strlen(password) < 30))
	{
		strcpy(g_lpszUserName, userName);
		strcpy(g_lpszPassword, password);
	}
	
    s_exitFlag = false;
	addBasicFuncMap();
    pthread_create(&s_tcpServerThread, NULL, serverSocketProc, (void *)(long)tcpPort );

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



