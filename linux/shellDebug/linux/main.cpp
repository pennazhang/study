#include "typedef.h"
#include "commonLib.h"
#include "tcpServerFrame.h"
#include <string>
#include <stdio.h>
#include <iostream>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include "shellFrame.h"

using namespace std;

void Stop(int signo) 
{
	printf("signo = %d\n", signo);

	// just dispatch Ctrl + C
	if (signo == SIGINT)
	{
	    if (endShellThread() < 0)
	    {
			printf("Shell Thread quit Abnormally!\n");
		}
    	_exit(0);
	}
}

void cmdLoop(void)
{
    std::string command;
    SYSWORD ulResult;
    int len;
//    char lpszSymbol[MAX_SYMBOL_COUNT][MAX_SYMBOL_LENGTH];
//    int nSymbolCount;
    //  BOOL bFlag;

	signal(SIGINT, Stop); 
	startShellThread(5023);

	printf("sizeof(char *) = %d\n", (int)sizeof(char *));
	printf("sizeof(UINT32) = %d\n", (int)sizeof(UINT32));
	printf("sizeof(UINT64) = %d\n", (int)sizeof(UINT64));
	
	if (sizeof(UINT32) != 4)
	{
		printf("Invalid UINT32, Please check it again\n");
		return;
	}
	
	if (sizeof(UINT64) != 8)
	{
		printf("Invalid UINT64, Please check it again\n");
		return;
	}

    mSleep(1000);
	
    for (;;)
    {
        char lpszCommand[256];
        outputDebug("\n->");
        fflush(stdout);

        for (;;)
        {
    		bool ret = isFdReadable(STDIN_FILENO, 10);
            if (ret != true)
            {
                mSleep(1000);
            }
            else
            {
                break;
            }
        }
//		if (ret == true)
		{
			len = read(STDIN_FILENO, lpszCommand, sizeof(lpszCommand));
			if (len > 1)
			{
				// we need to delect the tail character: '\n'
//				len--;
				lpszCommand[len] = 0;

                replaceString(lpszCommand, "\t\r\n", ' ');
                trimStringAtHead(lpszCommand);
                trimStringAtTail(lpszCommand);
                if (strcmp(lpszCommand, "exit") == 0)
                {
                    break;
                }

                if (explainCmd(lpszCommand, &ulResult) == STATUS_OK)
                {
                    printf("\nExecute function successfully, Result = 0x%lX = %ld\n", ulResult,
                                ulResult);
                }
                else
                {
                    printf("Execute function failed!\n");
                }
            }
        }
    }

	endShellThread();
}

int add(int a, int b)
{
	return (a + b);
}

int sub(int a, int b)
{
	return (a - b);
}


static FuncMapType theFuncList[] = 
{

		{"add",		  	(void *)add, 		"int add(int a, int b)"},		
		{"sub",		  	(void *)sub, 		"int sub(int a, int b)"}
};

	
int main (int argc, char *argv[])
{
#if 1
    int i;
    std::string lpszCmd;
    SYSWORD ulResult;
    char cBegin, cEnd;
    int nLen;

	addFunctionMap(theFuncList, sizeof(theFuncList) / sizeof(FuncMapType));
	
    if (argc == 2)
    {
        std::string fileName = argv[1];
        outputDebug("fileName = %s\n", fileName.c_str());
        return (0);
    }

	else
	{
	    cmdLoop();
	}
    return (0);


#else
    for (int i = 0; i < argc; i++)
    {
        printf("argv[%d] = %s\n", i, argv[i]);
    }
#endif
    return 0;
}

