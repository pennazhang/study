#ifndef _DIAG_H_
#define _DIAG_H_

#ifdef __cplusplus
extern "C" {
#endif


#ifndef _DEFINE_H_
#include "define.h"
#define _DEFINE_H_
#endif

#include   <sys/socket.h> 
#include   <netinet/in.h> 
#include   <netdb.h> 
#include   <sys/types.h> 
#include   <sys/stat.h> 
#include   <fcntl.h> 
#include   <dirent.h> 
#include   <sys/time.h> 
#include   <unistd.h> 
#include   <errno.h> 
#include   <net/if.h> 
#include   <arpa/inet.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pthread.h"

typedef 	unsigned long	SYSWORD;

typedef  unsigned long long   UINT64;
typedef  long long     INT64;


typedef  unsigned int   UINT32;
typedef  int     INT32;

typedef  unsigned short UINT16;
typedef  short   INT16;

typedef  unsigned char  UINT8;
typedef  char    INT8;


#ifndef _DEF_STATUS_
#define _DEF_STATUS_
typedef	    int	STATUS;
#endif

#ifndef  FALSE
#define  FALSE 0
#endif

#ifndef  TRUE
#define  TRUE 1
#endif

#ifndef _DEF_BOOL_
#define _DEF_BOOL_
typedef	    unsigned short	BOOL;
#endif

typedef     STATUS      VOS_STATUS;
#define     VOS_ERROR   -1
#define     VOS_OK      0

void Diag_Output(char * pFormat, ...);

#define DIAG_VERSION "2009-12-07 Version: 1.6"


#ifdef __cplusplus
}
#endif

#endif

