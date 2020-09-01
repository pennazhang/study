#ifndef _CPU_SHOW_H_
#define _CPU_SHOW_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "diag.h"

typedef struct
{
	SYSWORD	m_uiUser;
	SYSWORD 	m_uiNice;
	SYSWORD	m_uiSystem;
	SYSWORD 	m_uiIdle; 
}CPU_UTILITY_INFO;

void ShowCPUUtility();


#ifdef __cplusplus
}
#endif

#endif


