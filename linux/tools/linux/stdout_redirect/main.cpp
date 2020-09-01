#include "stdio.h"
#include "unistd.h"
#include "fcntl.h"

int main()
{
	int fd, f;
	
	printf("normal\n");
	fd = dup(STDOUT_FILENO);
	
	f = open("out.reopen", O_WRONLY | O_CREAT);
	dup2(f, STDOUT_FILENO);
	printf("STDOUT_FILENO = %x\n", (unsigned)STDOUT_FILENO);
	printf("fd = %x\n", (unsigned)fd);
	printf("redirected\n");
	
	dup2(fd, STDOUT_FILENO);
	printf("recoveried\n");
	
	return (0);
}