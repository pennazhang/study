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
#pragma once

#include <thread>
#include "typedef.h"

/*---------------------------------------------------------------------------*
 * Class: Thread
 *---------------------------------------------------------------------------*/
/**
 * This is an encapsulation of a pthread in the system.  A thread type is
 * subclassed, instantiated, and then called with run.  The thread will
 * then run until complete.
 *
 * <!------------------------------------------------------------------------*/
class Thread 
{
    protected:
        BOOL m_isRunning;
        BOOL m_needStop;
        void * m_threadReturn;
        UINT32 m_threadID;
        std::string m_threadName;
        
    public:
        Thread(const char * threadName = NULL);
        virtual ~Thread();

        UINT32 getThreadID() { return m_threadID; };

        std::string getThreadName() { return m_threadName; }

        void setThreadName(const char * threadName);

        /* Can be called by any Thread and any times, just trigger the quit process, never block!!! */
        virtual void quit() { };

        /* Must NOT be called by this Thread */
        void start();

        /* Must NOT be called by this Thread. it will block the caller! */
        void stop();

        /* Must NOT be called by this Thread */
        void * getThreadReturn() { return m_threadReturn; }

        /* Can be called by this Thread */
        /* If called by this thread, it always returns true. */
        bool isRunning(void) { return this->m_isRunning; }

        /* Must be called by this Thread */
        static void mSleep(unsigned int milliseconds);

        virtual STATUS onStart() {  return STATUS_OK; };

        // just called by the this thread itself.
        virtual void onClose() = 0;

    protected:
        virtual void *run() = 0;
        static void runThread(void *aThread);
};


