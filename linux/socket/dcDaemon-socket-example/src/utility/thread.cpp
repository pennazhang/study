//####
//# Legal Notice :
//#    Copyright, Harman International, 2020
//#    Private, proprietary information, the sole property of Harman.  The
//#    contents, ideas, and concepts expressed herein are not to be disclosed
//#    except within the confines of a confidential relationship and only
//#    then on a need to know basis.
//#
//#    Any entity in possession of this Harman Software shall not, and shall not
//#    permit any other person to, disclose, display, loan, publish, transfer
//#    (whether by sale, assignment, exchange, gift, operation of law or
//#    otherwise), license, sublicense, copy, or otherwise disseminate this
//#    Harman Software.
//#
//#    This Harman Software is owned by Harman and is protected by United States
//#    copyright laws, patent laws, international treaty provisions, and/or
//#    state of Texas trade secret laws.
//#
//#    Portions of this Harman Software may, from time to time, include
//#    pre-release code and such code may not be at the level of performance,
//#    compatibility and functionality of the final code. The pre-release code
//#    may not operate correctly and may be substantially modified prior to
//#    final release or certain features may not be generally released. Harman
//#    is not obligated to make or support any pre-release code. All pre-release
//#    code is provided "as is" with no warranties.
//#
//#    This Harman Software is provided with restricted rights. Use,duplication,
//#    or disclosure by the Government is subject to restrictions as set forth
//#    in subparagraph (1)(ii) of The Rights in Technical Data and Computer
//#    Software clause at DFARS 252.227-7013 or subparagraphs (1) and (2) of
//#    the Commercial Computer Software Restricted Rights at 48 CFR 52.227-19,
//#    as applicable.
//####
/*-------------------------------------------------------------------------*/
#include <linux/unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/prctl.h>
#include <sys/resource.h>
#include <unistd.h>
#include <stdio.h>
#include "utility.h"
#include "thread.h"

/*---------------------------------------------------------------------------*
 * Constants:
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
 * Macros:
 *---------------------------------------------------------------------------*/

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
Thread::Thread(const char * threadName)
{
    m_isRunning = false;
	m_needStop = false;
    m_threadReturn = NULL;
    m_threadID = 0;
    if (threadName != NULL)
    {
        m_threadName = std::string(threadName);
    }
}

Thread::~Thread()
{
    stop();
}

void Thread::setThreadName(const char * threadName)
{
    m_threadName = std::string(threadName);
}

void Thread::quit()
{
    m_needStop = true;
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
void Thread::runThread(void *aThread)
{
    Thread *p_thread = (Thread *)aThread;

    if (p_thread->onStart() == STATUS_OK)
    {
        p_thread->m_threadID = (UINT32)_threadID();
        p_thread->m_isRunning = TRUE;
        logInfo("Thread: %s[%d] is running...", p_thread->getThreadName().c_str(), p_thread->getThreadID());

        p_thread->m_threadReturn = p_thread->run();
        p_thread->onQuit();
        logInfo("Thread: %s[%d] is stopped!", p_thread->getThreadName().c_str(), p_thread->getThreadID());
        p_thread->m_isRunning = false;
    }
    p_thread->m_threadID = 0;
}

/*---------------------------------------------------------------------------*
 * Routine: Thread::start
 *---------------------------------------------------------------------------*/
/**
 * Create an instance of this thread type and start running.
 *
 *
 * <!------------------------------------------------------------------------*/
void Thread::start()
{
    std::thread t(Thread::runThread, this);
    t.detach();
}

void Thread::stop()
{
    int count;
    m_needStop = true;
//    logInfo("stop: %s, running flag = %d", m_threadName.c_str(), m_isRunning);
    if (m_isRunning)
    {
        quit();

        // We will wait for 5 seconds at most.
        for (count = 0; count < 50; count++)
        {
            if (m_isRunning == false)
            {
                break;
            }
            mSleep(100);
        }
    }
    if (m_isRunning)
    {
        logError("Thread: %s stop failed! count = %d", m_threadName.c_str(), count);
    }
}

/*---------------------------------------------------------------------------*
 * Routine: Thread::mSleep
 *---------------------------------------------------------------------------*/
/**
 * Sleep a given number of milliseconds.
 *
 * @param [in] aMilliseconds -- Number of milliseconds to sleep
 *
 * <!------------------------------------------------------------------------*/
void Thread::mSleep(unsigned int aMilliseconds)
{
    usleep(aMilliseconds * 1000);
}

