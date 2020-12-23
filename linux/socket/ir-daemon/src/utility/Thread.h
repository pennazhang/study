/*
 * Thread.h
 *
 *  Created on: Apr 7, 2014
 *      Author: Lysle
 */

#ifndef THREAD_H_
#define THREAD_H_

#include <pthread.h>

/*---------------------------------------------------------------------------*
 * Class: Thread
 *---------------------------------------------------------------------------*/
/**
 * This is an encapsulation of a pthread in the system.  A thread type is
 * subclassed, instantiated, and then called with run.  The thread will
 * then run until complete.
 *
 * <!------------------------------------------------------------------------*/
class Thread {
    protected:
        unsigned char iIsRunning;
        pthread_t iThreadStruct;
        const char *iName;
        int iThreadID;
    public:
        Thread();
        void start(const char *aName);
        unsigned char isRunning(void) { return this->iIsRunning; }
        void SetName(const char *aName) { iName = aName; }
    protected:
        virtual void *run(void) = 0;
        virtual ~Thread() {}
        static void *runThread(void *aThread);
        void SleepMS(unsigned int aMilliseconds);
        virtual void end(void) {};
        virtual int SetPriority(int aPriority);
        int GetThreadID(void) { return iThreadID; }
};

#endif /* THREAD_H_ */
