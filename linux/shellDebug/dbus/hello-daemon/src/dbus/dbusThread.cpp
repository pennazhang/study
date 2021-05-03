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
#include <stdlib.h>
#include <string.h>
#include "utility.h"
#include "dbusThread.h"
#include "commonInterface.h"
#include "config.h"

//
// Global Variables.
//
static const char *BUS_NAME = "com.harman.jetstream.helloDaemon";
static const char *OBJECT_PATH = "/com/harman/jetstream/helloDaemon";
clock_t g_StartClock = 0;

// Dbus connections.
DBus::BusDispatcher*    g_dbusDispatcherPtr = NULL;
DBus::Connection*       g_dbusConnPtr = NULL;

//
// DBus Interface.
//
CommonInterface* g_pCommonInterface = NULL;

DBusThread::DBusThread(const char * threadName) : Thread(threadName)
{

}

void DBusThread::quit() 
{ 
    if (g_dbusDispatcherPtr)
    {
        logInfo("call quit()");
        g_dbusDispatcherPtr->leave();
    }
}

void DBusThread::onClose()
{
    return;
}

void* DBusThread::run()
{
    // Initial the start clock.
    g_StartClock = clock();



    // Setup DBUS connection to cmsfs since we need config early
    // Create and start the DBus dispatcher
    g_dbusDispatcherPtr = new DBus::BusDispatcher();

    DBus::_init_threading();
    DBus::default_dispatcher = g_dbusDispatcherPtr;


    g_dbusConnPtr = new DBus::Connection(DBus::Connection::SessionBus());

	g_dbusConnPtr->request_name(BUS_NAME);
	std::string objectPath(OBJECT_PATH);
	g_pCommonInterface = new CommonInterface(*g_dbusConnPtr, objectPath.c_str());


    g_dbusDispatcherPtr->enter();

    logInfo("quit DBusThread1...");

    // Finished running...
    if (g_pCommonInterface) 
    {
        delete g_pCommonInterface;
    }

    if (g_dbusConnPtr) 
    {
        delete g_dbusConnPtr;
    }

    if (g_dbusDispatcherPtr) 
    {
        delete g_dbusDispatcherPtr;
    }
        
    logInfo("quit DBusThread2...");
    return (NULL);
}

