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
    std::string lpszCommand;
    SYSWORD ulResult;
//    char lpszSymbol[MAX_SYMBOL_COUNT][MAX_SYMBOL_LENGTH];
//    int nSymbolCount;
    //  BOOL bFlag;

	signal(SIGINT, Stop); 
	startShellThread(5023, "-> ", "root", "dvi");

	printf("sizeof(char *) = %d\n", (int)sizeof(char *));
	printf("sizeof(long) = %d\n", (int)sizeof(long));
	printf("sizeof(int) = %d\n", (int)sizeof(int));
	
    for (;;)
    {
        outputDebug("\n->");
#if 0
		bool ret = isFdReadable(STDIN_FILENO, 10);
		if (ret == true)
		{
			len = read(STDIN_FILENO, lpszCommand, sizeof(lpszCommand));
			if (len > 1)
			{
				// we need to delect the tail character: '\n'
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
#else
		cin >> lpszCommand;

        if ((lpszCommand.compare("bye") == 0) ||
            (lpszCommand.compare("exit") == 0))
        {
            break;
        }
#endif
#if 0
        /* 判断是否有内容 */
        if (lpszCommand[0] != ':')
        {
            nSymbolCount = ReadSymbolTable(lpszCommand, lpszSymbol);
            if (nSymbolCount == 0)
            {
                continue;
            }
        }

        /* 解释输入是否正确 */
        if (ExplainCmd(lpszCommand, &ulResult) == VOS_OK)
        {
            Diag_Output("\nExecute function successfully, Result = 0x%lX = %ld\n", ulResult,
                        ulResult);
        }
        else
        {
            Diag_Output("Execute function failed!\n");
        }
#endif	
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


static FUNC_MAP theFuncList[] = 
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

	addFunctionMap(theFuncList, sizeof(theFuncList) / sizeof(FUNC_MAP));
	
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

