// videoEP.h

#ifndef _VIDEOEP_H_
#define _VIDEOEP_H_

#include <sys/syscall.h>

/*---------------------------------------------------------------------------*
 * Constants:
 *---------------------------------------------------------------------------*/
#define SERIES_1000         0x01
#define SERIES_2000         0x02


/*---------------------------------------------------------------------------*
 * Macros:
 *---------------------------------------------------------------------------*/
#define gettid() syscall(__NR_gettid)   /* missing in headers? */
#if 0
#define ARRAY_COUNT(ar)  (sizeof(ar)/sizeof(ar[0]))
#define gettid() syscall(__NR_gettid)   /* missing in headers? */
#define my_sched_setaffinity(a,b,c) sched_setaffinity(a, b, c)

#if 1
/* set real-time priority: 1 low - 99 high*/
#define SET_THREAD_PRIORITY( __prio,__tid ) { \
    struct sched_param schedpar; \
    schedpar.sched_priority = __prio; \
    pthread_setschedparam(__tid, SCHED_RR, &schedpar); \
}
#else
#define SET_THREAD_PRIORITY() { \
    int mttid = gettid(); \
    if (setpriority(PRIO_PROCESS, mttid, -10) != 0) { \
        SVSI_LOG(GENERAL_LOG,SVSI_LOG_INFO, "Could not set scheduling priority: %s\n", \
                strerror(errno)); \
    } \
}
#endif

#define SET_NICENESS( __prio ) { \
    if (setpriority(PRIO_PROCESS, gettid(), __prio ) != 0) { \
        SVSI_SYSLOG("could not set scheduling priority: %s\n", strerror(errno)); \
    } \
}


#define CHECK_THD_PRIORITY( __pri ) { \
    int __policy; \
    struct sched_param __param; \
    pthread_getschedparam(pthread_self(), &__policy, &__param); \
    if (__policy != SCHED_RR) SET_THREAD_PRIORITY( __pri, pthread_self()); \
}
#endif // 0

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
struct VE_Settings_t
{
    unsigned char myip[4];
    unsigned char setNewIPSettings;

    //irParamsFIFO_t irCommandFIFO;

    //! Time to pause between IR command and repeat (milliseconds)
    unsigned int irTimeRepeatHoldoff;

    //! Time to pause between the end of one IR command and the
    //! start of another (milliseconds)
    unsigned int irTimeCommandHoldoff;
    // use remote IR connection instead of local commands
    volatile unsigned char irPassThroughEnable;
    volatile unsigned char irClientIP[4];
    unsigned int irClientPort;

    //! Current IP Address, Netmask, and Gateway
    char currentIP[32];
    char currentNetmask[32];
    char currentGateway[32];
};

/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
 * Prototypes:
 *---------------------------------------------------------------------------*/
void VideoEP_MainLoop(VE_Settings_t *veSet);
void VideoEPRequestCloseAllSockets(void);
int VideoEPRequestTCPCloseSocket(unsigned short aSocket);
extern void VideoEP_SetupSSLCertificate(void);

#endif // _VIDEOEP_H_
