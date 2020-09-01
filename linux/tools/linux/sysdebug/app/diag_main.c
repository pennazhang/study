
#ifdef __cplusplus
extern "C" {
#endif

/* include udptest head file */

#include "diag.h"
#include "FuncReg.h"

int add(int a, int b)
{
	printf("%d + %d = %d\n", a, b, a + b);

	return (a + b);
}

int main(int argc,char *argv[])
{
    int i;
    char lpszCmd[256];
    SYSWORD ulResult;
    char cBegin, cEnd;
    int nLen;

    if (argc != 1)
    {
        lpszCmd[0] = 0;
        for (i = 1; i < argc; i++)
        {
            /* 命令行中无法输入"，因此我们就用{}来代替"  */
            nLen = strlen(argv[i]);
            
            cBegin = argv[i][0];
            cEnd = argv[i][nLen - 1];
            if ((cBegin == '{') && (cEnd == '}'))
            {
                argv[i][0] = '"';
                argv[i][nLen - 1] = '"';
            }
            
            strcat(lpszCmd, argv[i]);
            strcat(lpszCmd, " ");
        }
        printf("->%s\n", lpszCmd);
        
		/* 解释输入是否正确 */
		if (ExplainCmd(lpszCmd, &ulResult) == VOS_OK)
		{
			printf("\nExecute function successfully, Result = 0x%lX = %ld\n", ulResult, ulResult);
		}
		else
		{
			printf("Execute function failed!\n");
		}
		return ((int)ulResult);
    }
        
	CmdLoop();

	return (0);
}



#ifdef __cplusplus
}
#endif

