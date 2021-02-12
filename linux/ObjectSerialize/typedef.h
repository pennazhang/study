
#pragma once

#include <stdbool.h>
#include <stdio.h>

//
// Re-define the Basic type of variable.
//
typedef unsigned long   SYSWORD;

typedef unsigned long   ULONG;
typedef long            LONG;

typedef  unsigned int   UINT;
typedef  int            INT;

typedef unsigned long   UINT64;
typedef long            INT64;

typedef unsigned int    UINT32;
typedef signed int      INT32;
    
typedef unsigned short  UINT16;
typedef short           INT16;

typedef  unsigned char  UINT8;
typedef  char    INT8;


typedef     int STATUS;
#define     STATUS_OK   0
#define     STATUS_ERROR -1

#ifndef  FALSE
#define  FALSE false
#endif

#ifndef  TRUE
#define  TRUE true
#endif

#ifndef BOOL
#define BOOL bool
#endif

