#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h> 
#include <signal.h>
#include "common.h"
#include "debugFrame.h"

#define  LISTEN_COUNT 1

static int s_serverSocket = -1;
static int s_fd;    // standard output handle

void *clientFunc(void* arg)
{
    char lpszCmd[1024];
	int client_sock = (int)(SYSWORD)arg;
    SYSWORD result;

    /* set the standard output to client_sock */
    dup2(client_sock, STDOUT_FILENO);

    while(1)
    {  
        // Print Prompt:
        printf("\n-> ");
        fflush(stdout);
        int len = read(client_sock, lpszCmd, 1024);
        if (len <= 0)
        {
            break;
        }

        lpszCmd[len] = 0;

        /* Delete the /r/n at the end of the line */
        for (int i = 0; i < len; i++)
        {
            if ((lpszCmd[i] == '\r') || (lpszCmd[i] == '\n'))
            {
                lpszCmd[i] = 0;
                len = i;
                break;
            }
        }
        
		if (len == 0)
		{
			continue;
		}
		
        if(strncasecmp(lpszCmd,"exit",4)==0)
        {
            printf("exit!\n");
            break;
        }

		if (ExplainCmd(lpszCmd, &result) == STATUS_OK)
		{
			printf("Execute function successfully, Result = 0x%lX = %ld\n", result, result);
		}
		else
		{
#if 0            
			printf("Execute function failed!\n");
#else
            // Let's explain as linux shell command.
            {
                char buf[1024];
                FILE  *fp=popen(lpszCmd, "r");
                while (fgets(buf, 1024, fp) != NULL)
	            {
					buf[1023] = 0;
		            printf("%s", buf);
	            }

                pclose(fp);
            }
#endif            
		}

    }

    close(client_sock);

    /* restore the standard output */
    dup2(s_fd, STDOUT_FILENO);
    printf("client socket is closed.\n");
	return (NULL);
}

/*SIGINT信号截取函数*/
void tcpServerExit(void)
{
    if (s_serverSocket >= 0)
	{
		printf("Close socket: %d\n", s_serverSocket);
		close(s_serverSocket);
        s_serverSocket = -1;
        usleep(1000);
	}
	
    dup2(s_fd, STDOUT_FILENO);
    return;
}

void * listenServer(void *)
{
    pthread_t thread_id; 
	struct sockaddr_in sockAddr;
    for(;;)
    {
        socklen_t len=0;
        int client_sock = accept(s_serverSocket, (struct sockaddr*)&sockAddr, &len);
        if (client_sock < 0)
        {
            printf("accept()\n");
            return (NULL);
        }
        char buf_ip[INET_ADDRSTRLEN];
        memset(buf_ip,'\0',sizeof(buf_ip));     
        inet_ntop(AF_INET, &sockAddr.sin_addr, buf_ip, sizeof(buf_ip));
        printf("get connect,ip is%s\n",buf_ip);
        printf("port=%d\n", ntohs(sockAddr.sin_port));  
        pthread_create(&thread_id, NULL, clientFunc, (void *)(SYSWORD)client_sock);  
        pthread_detach(thread_id); 
    }
    tcpServerExit();
    return (NULL);
}

STATUS tcpServerInit(int port)
{
	static int s_init = 0;
    s_fd = dup(STDOUT_FILENO);

	if (s_init != 0)
	{
		printf("Error: Already init\n");
		return -1;
	}

    s_serverSocket=socket(AF_INET, SOCK_STREAM, 0);
    if (s_serverSocket < 0)
    {
		printf("Error: socket() failed\n");
        return -2;
    }
		
	int on = 1;
	// SO_REUSEADDR 允许关闭某个socket后，立刻再使用该socket； 如果没有这条语句，这一般需要等待2-4分钟。
	setsockopt(s_serverSocket, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
	
    struct sockaddr_in server_socket;
    pthread_t thread_id;  
    bzero(&server_socket,sizeof(server_socket));
    server_socket.sin_family = AF_INET;
    server_socket.sin_addr.s_addr = htonl(INADDR_ANY);
    server_socket.sin_port = htons(port);
    if(bind(s_serverSocket, (struct sockaddr*)&server_socket, sizeof(struct sockaddr_in)) <0 )
    {
        printf("Error: bind() failed\n");
        close(s_serverSocket);
        s_serverSocket = -1;
        return -3;
    }
    if (listen(s_serverSocket, LISTEN_COUNT) < 0)
    {
        printf("Error: listen() failed\n");
        close(s_serverSocket);
        s_serverSocket = -1;
        return -4;
    }

    printf("Listening at %d, Press Ctrl+c to quit...\n", port);
	pthread_create(&thread_id, NULL, listenServer, (void *)NULL);  
    pthread_detach(thread_id); 
	s_init = 1;
	return (0);
}
