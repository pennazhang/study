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

#define _PORT_ 9994
#define  LISTEN_COUNT 10

void *fun(void* arg)
{
    char buf[1024], temp;
	int client_sock = (int)arg;
	int i;
    while(1)
    {  
        int n = read(client_sock, buf, 1024);
		if (n <= 0)
		{
			break;
		}
		
		buf[n] = 0;
        printf("client %d bytes:# %s\n", n, buf);
		
        if(strncasecmp(buf,"quit",4)==0)
        {
            printf("quit\n");
            break;
        }
		
		for (i = 0; i < n / 2; i++)
		{
			temp = buf[i];
			buf[i] = buf[n - i - 1];
			buf[n - i - 1] = temp;
		}
        write(client_sock, buf, strlen(buf) + 1);
    }
    close(client_sock);
}

/*SIGINT信号截取函数*/
int g_serverSocket = 0;
static void sig_int(int num)
{
    if (g_serverSocket >= 0)
	{
		printf("Close socket: %d\n", g_serverSocket);
		close(g_serverSocket);
	}
	
    return;
}

int main()
{
	signal(SIGINT, sig_int);//注册信号 捕获SIGINT(中断)信号,然后调用sig_int函数 （Ctrl-C会产生这个信号）

    g_serverSocket=socket(AF_INET, SOCK_STREAM, 0);
    if (g_serverSocket < 0)
    {
        printf("socket()\n");
    }
		
	int on = 1;
	// SO_REUSEADDR 允许关闭某个socket后，立刻再使用该socket； 如果没有这条语句，这一般需要等待2-4分钟。
	setsockopt(g_serverSocket, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
	
    struct sockaddr_in server_socket;
    struct sockaddr_in socket;
     pthread_t thread_id;  
    bzero(&server_socket,sizeof(server_socket));
    server_socket.sin_family=AF_INET;
    server_socket.sin_addr.s_addr=htonl(INADDR_ANY);
    server_socket.sin_port=htons(_PORT_);
    if(bind(g_serverSocket,(struct sockaddr*)&server_socket,sizeof(struct sockaddr_in))<0)
    {
        printf("bind()\n");
        close(g_serverSocket);
        return 1;
    }
    if (listen(g_serverSocket, LISTEN_COUNT) < 0)
    {
        printf("listen()\n");
        close(g_serverSocket);
        return 2;
    }

    printf("Listening at %d, Press Ctrl+c to quit...\n", _PORT_);
    for(;;)
    {
        socklen_t len=0;
        int client_sock = accept(g_serverSocket,(struct sockaddr*)&socket,&len);
        if(client_sock < 0)
        {
            printf("accept()\n");
            return 3;
        }
        char buf_ip[INET_ADDRSTRLEN];
        memset(buf_ip,'\0',sizeof(buf_ip));     
        inet_ntop(AF_INET, &socket.sin_addr, buf_ip, sizeof(buf_ip));
        printf("get connect,ip is%s\n",buf_ip);
        printf("port=%d\n",ntohs(socket.sin_port));  
        pthread_create(&thread_id, NULL, (void *)fun, (void *)client_sock);  
        pthread_detach(thread_id); 
    }
    close(g_serverSocket);
    return 0;
}
