关于C语言下的远程调试程序，我先写了remoteDebug，后来以为找不到了，于是又写了shellDebug，其实这两个程序都是相同的功能，但我还是建议使用shellDebug。
在remoteDebug下运行函数时有个问题：没有对参数的个数进行检查。
比如：int add(int a, int b)
如果输入： -> add 1
程序也不会报错，因为代码实现任何函数都会传入10个参数，如果实际上只写了一个参数，那剩余9个参数值都为0；
如果有空，我们可以加入对参数个数的检查。
初步想法：我们定义函数名称和地址是，还要加入函数参数个数的定义；
目前我们的定义如下：
typedef struct
{
	const char *m_lpszName;
	void * m_pAddr;
	const char *m_lpszHelp;
}FUNC_MAP;

以后我们需要改成：
typedef struct
{
	const char *m_lpszName;
	void * m_pAddr;
	int m_paramCount;
	const char *m_lpszHelp;
}FUNC_MAP;

比如：
FUNC_MAP g_theFuncReg[] = 
{ 
		{"help",			(void *)help, 			, 0, "void help()"											},	
		{"lkup",			(void *)lkup, 			, 1, "void lkup(char *lpszFuncName)"							},	
		{"malloc",			(void *)malloc,			, 1, "void * malloc(int size)"								},
		{"strcpy",			(void *)strcpy,			, 2, "char * strcpy(char *dest, char *src)"					},
		{"printString", 	(void *)printString,	, 1, "void printString(char *lpszBuffer)"					},