

#include "stdio.h"
#include "unistd.h"
#include "fcntl.h"
#include "main.h"
#include "debugFrame.h"
#include <signal.h>
#include <string.h>

int main()
{
	int fd, f;
	char buffer[128];
	int len;

	printf("sizeof(int) = %ld\n", sizeof(int));
	printf("sizeof(void *) = %ld\n", sizeof(void *));
	printf("sizeof(long) = %ld\n", sizeof(long));
	
	printf("normal\n");
	fd = dup(STDOUT_FILENO);
	
	f = open("out.reopen", O_WRONLY | O_CREAT);
	dup2(f, STDOUT_FILENO);
	printf("STDOUT_FILENO = %x\n", (unsigned)STDOUT_FILENO);
	printf("fd = %x\n", (unsigned)fd);
	printf("redirected\n");
	
	dup2(fd, STDOUT_FILENO);
	printf("recoveried\n");
	printf("Press \"exit\" to quit...\n");
	debugInit(19272);
	for (;;)
	{
		usleep(1000);

		bool ret = isFdReadable(STDIN_FILENO);
		if (ret == true)
		{
			len = read(STDIN_FILENO, buffer, sizeof(buffer));
			if (len > 1)
			{
				// 去掉结尾的\n
				len--;
				buffer[len] = 0;
				if(strncasecmp(buffer, "exit", 4) == 0)
				{
					printf("exit\n");
					break;
				}
				else
				{
					printf("Press \"exit\" to quit...\n");
				}
				
			}			
		}
	}
	
	printf("Exiting...\n");
	debugExit();
	return (0);
}

