/*-------------------------------------------------------------------------*
 * File:  Thread.cpp - Copyright (C) 2013 All Rights Reserved - SVSi
 *-------------------------------------------------------------------------*/
/**
 *  @addtogroup
 *  @{
 *  @brief
 *
 * Long description goes here
 */
/*-------------------------------------------------------------------------*/
#include <linux/unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <sys/prctl.h>
#include <sys/resource.h>
#include <unistd.h>
#include "Thread.h"

#include <stdio.h>

/*---------------------------------------------------------------------------*
 * Constants:
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
 * Macros:
 *---------------------------------------------------------------------------*/
#define gettid() syscall(__NR_gettid)   /* missing in headers? */

/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
 * Prototypes:
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
 * Routine: Thread::Thread
 *---------------------------------------------------------------------------*/
/**
 * Construct a new thread that is not running.
 *
 * <!------------------------------------------------------------------------*/
Thread::Thread()
{
    iIsRunning = 0;
    iThreadID = 0;
    memset(&iThreadStruct, 0, sizeof(iThreadStruct));
    iName = "<unnamed>";
}

/*---------------------------------------------------------------------------*
 * Routine: Thread::runThread
 *---------------------------------------------------------------------------*/
/**
 * This internal function is called by the C pthread code.  It comes here,
 * converts the passed in parameter into a C object and runs it.
 *
 * @param [in] aThread -- Pointer to the Thread object
 *
 * @return Returned value from the thread.
 * <!------------------------------------------------------------------------*/
void *Thread::runThread(void *aThread)
{
    Thread *p_thread = (Thread *)aThread;

    // Get the thread id
    p_thread->iThreadID = gettid();

    // Rename the process (if any)
    if (p_thread->iName)
        prctl(PR_SET_NAME, p_thread->iName, 0, 0, 0);

    void *retValue = p_thread->run();
    p_thread->iIsRunning = 0;

    // The thread is now done, clear any needed memory or take any other actions!
    p_thread->end();
    p_thread->iThreadID = 0;

    pthread_exit(retValue);
}

/*---------------------------------------------------------------------------*
 * Routine: Thread::start
 *---------------------------------------------------------------------------*/
/**
 * Create an instance of this thread type and start running.
 *
 * @param [in] aName -- Name of thread, or 0 to use parent's name
 *
 * <!------------------------------------------------------------------------*/
void Thread::start(const char *aName)
{
    pthread_attr_t attr;
    int rc;

    if (!iIsRunning) {
        iName = aName;
        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

        rc = pthread_create(&iThreadStruct, &attr, Thread::runThread,
                (void *)this);
        iIsRunning = 1;
    }
}

/*---------------------------------------------------------------------------*
 * Routine: Thread::SleepMS
 *---------------------------------------------------------------------------*/
/**
 * Sleep a given number of milliseconds.
 *
 * @param [in] aMilliseconds -- Number of milliseconds to sleep
 *
 * <!------------------------------------------------------------------------*/
void Thread::SleepMS(unsigned int aMilliseconds)
{
    usleep(aMilliseconds * 1000);
}

/*---------------------------------------------------------------------------*
 * Routine: Thread::SetPriority
 *---------------------------------------------------------------------------*/
/**
 * Set the priority (niceness) of the Thread.
 *
 * @param [in] aPriority -- Priority level of the thread
 *
 * <!------------------------------------------------------------------------*/
int Thread::SetPriority(int aPriority)
{
    return setpriority(PRIO_PROCESS, iThreadID, aPriority);
}

/** @} */
/*-------------------------------------------------------------------------*
 * End of File:  Thread.cpp
 *-------------------------------------------------------------------------*/
