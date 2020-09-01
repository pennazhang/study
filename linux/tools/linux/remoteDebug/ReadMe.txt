remoteDebug 是为Linux下调试应用程序而开发的。
我们假设已经有一个Linux应用程序已经在运行了，那么我们将remoteDebug中的源程序加入到Linux APP中，然后调用初始函数：
debugInit（port）就可以在本地开启一个TCP Server端口。我们只需要通过secureCRT程序telnet到TCP Server上，就可以开始类似于vxworks的调试了。
Linux App退出时，需要调用debugExit（）函数来关闭TCP Server服务程序。

下面是telnet窗口的演示程序：

-> help
^J                help    0x562418664A50    void help()
                lkup    0x5624186648F1    void lkup(char *lpszFuncName)
              malloc    0x7F051D984B90    void * malloc(int size)
              strcpy    0x562418663120    char * strcpy(char *dest, char *src)
         printString    0x562418664FDC    void printString(char *lpszBuffer)
       readHexString    0x56241866504C    int readHexString(UINT8 *pucData, char *lpszBuffer, int nLen)
                   d    0x56241866514D    void d(void *lpszBuffer,  int nByteCount, int nWidth)
                 crc    0x56241866543B    UINT16 crc(UINT8 *ptr,  int nLen);
        getTickCount    0x56241866552F    unsigned long getTickCount();
Execute function successfully, Result = 0x9 = 9

-> p1 = malloc 9
^JExecute function successfully, Result = 0x7F0510000B20 = 139659720002336

-> strcpy p1, "goodluck"
^JExecute function successfully, Result = 0x7F0510000B20 = 139659720002336

-> crc p1 8
^JExecute function successfully, Result = 0xDF3E = 57150

-> crc "goodluck" 8
^JExecute function successfully, Result = 0xDF3E = 57150

-> getTickCount
^JExecute function successfully, Result = 0x16ADD86160A = 1558494713354

-> p1 = malloc 20
^JExecute function successfully, Result = 0x7F1AB4000B20 = 139752665778976

-> readHexString p1, "0a 0b 0c 0d 0e 0f", 6
^JExecute function successfully, Result = 0x6 = 6

-> d p1
^J7f1ab4000b20 : 0a  0b  0c  0d  0e  0f  00  00 -  00  00  00  00  00  00  00  00 
Execute function successfully, Result = 0x7F1AB4000B30 = 139752665778992

-> crc p1 6
^JExecute function successfully, Result = 0xB57E = 46462

如果我们要加入更多的调试函数，只需要在funcInterface.cpp中修改
FUNC_MAP g_theFuncReg[] = 
{
		{"help",			(void *)help, 			"void help()"											},	
		{"lkup",			(void *)lkup, 			"void lkup(char *lpszFuncName)"							},	
		{"malloc",			(void *)malloc,			"void * malloc(int size)"								},
		{"strcpy",			(void *)strcpy,			"char * strcpy(char *dest, char *src)"					},
		{"printString", 	(void *)printString,	"void printString(char *lpszBuffer)"					},
		{"readHexString",	(void *)readHexString,  "int readHexString(UINT8 *pucData, char *lpszBuffer, int nLen)" },
		{"d",				(void *)d, 				"void d(void *lpszBuffer,  int nByteCount, int nWidth)" },
		{"crc",				(void *)crc, 			"UINT16 crc(UINT8 *ptr,  int nLen);" 					},		
		{"getTickCount",	(void *)getTickCount, 	"unsigned long getTickCount();" 						},		
};

比如：我们想加一个add函数：那我们就需要定义一个 int add (int a, int b); 然后修改 g_theFuncReg的定义如下：
FUNC_MAP g_theFuncReg[] = 
{
		{"help",			(void *)help, 			"void help()"											},	
		{"lkup",			(void *)lkup, 			"void lkup(char *lpszFuncName)"							},	
		{"malloc",			(void *)malloc,			"void * malloc(int size)"								},
		{"strcpy",			(void *)strcpy,			"char * strcpy(char *dest, char *src)"					},
		{"printString", 	(void *)printString,	"void printString(char *lpszBuffer)"					},
		{"readHexString",	(void *)readHexString,  "int readHexString(UINT8 *pucData, char *lpszBuffer, int nLen)" },
		{"d",				(void *)d, 				"void d(void *lpszBuffer,  int nByteCount, int nWidth)" },
		{"crc",				(void *)crc, 			"UINT16 crc(UINT8 *ptr,  int nLen);" 					},		
		{"getTickCount",	(void *)getTickCount, 	"unsigned long getTickCount();" 						},	
		{"add",				(void *)add, 			"int add (int a, int b);" 								},			
};

然后重新编译，运行，在telnet窗口下输入：
-> add 1, 2