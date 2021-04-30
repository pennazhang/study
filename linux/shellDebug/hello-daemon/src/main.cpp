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

#include <iostream>
#include <string>
#include <sysexits.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <dbus-c++-1/dbus-c++/dbus.h>
#include "config.h"
#include "commonInterface.h"
#include "utility.h"
#include "typedef.h"
//
// Global Variables.
//
static const char *BUS_NAME = "com.harman.jetstream.helloDaemon";
static const char *OBJECT_PATH = "/com/harman/jetstream/helloDaemon";
clock_t g_StartClock = 0;

// Dbus connections.
DBus::BusDispatcher*    g_dbusDispatcherPtr;
DBus::Connection*       g_dbusConnPtr;

//
// DBus Interface.
//
CommonInterface* g_pCommonInterface = NULL;

//
// SingleonProcess is used to enusre that only one application instance is running in the system.
// Used to store the pid of ir-daemon */
const char g_SingletonFileName[] = "/tmp/hello-daemon-singleton"; 
//
// Clean up all resource in this file before quit the appliation.
//
void cleanup()
{
    logDebug("[%s]%s.", DAEMON_NAME, __FUNCTION__);

    if (g_pCommonInterface) {
        delete g_pCommonInterface;
    }
    if (g_dbusDispatcherPtr) {
        delete g_dbusDispatcherPtr;
    }
    if (g_dbusConnPtr) {
        delete g_dbusConnPtr;
    }
}

//
// New resource for DBus interface, but won't start the DBus Thread
//
void startDBusDispatcher()
{
    g_dbusDispatcherPtr = new DBus::BusDispatcher();

    DBus::_init_threading();
    DBus::default_dispatcher = g_dbusDispatcherPtr;


    g_dbusConnPtr = new DBus::Connection(DBus::Connection::SessionBus());

	g_dbusConnPtr->request_name(BUS_NAME);
	std::string objectPath(OBJECT_PATH);
	g_pCommonInterface = new CommonInterface(*g_dbusConnPtr, objectPath.c_str());
}

//
// Set flag to quit the DBus thread, if the DBus thread is already start.
//
void stopDBus()
{
    if (g_dbusDispatcherPtr)
    {
        g_dbusDispatcherPtr->leave();
    }
}

//
// Handle to dispatch "Ctrl + C"
//
void sighandler(int sig)
{
    logDebug("[%s]%s.", DAEMON_NAME, __FUNCTION__);

    stopDBus();
    exit(1);
}

void  mainLoop(int argc, char** argv);
//
// Process Start.
//
STATUS main(int argc, char** argv)
{
    SingletonProcess singletonProcess(g_SingletonFileName);

    /* Only one instance for this application */
    if (singletonProcess.isSingleton() == false)
    {
        fprintf(stderr, "Another instance is already running.\n");
        exit(1);
   }

    // Initial the start clock.
    g_StartClock = clock();


//    return (0);
    // Signal Handlers
    signal(SIGTERM, sighandler);
    signal(SIGINT, sighandler);

    // Setup DBUS connection to cmsfs since we need config early
    // Create and start the DBus dispatcher
    startDBusDispatcher();

    // get the global settings.

    logDebug("Processing...");


    g_dbusDispatcherPtr->enter();

    // Finished running...
    cleanup();

    logDebug("Daemon exited.");

    exit(STATUS_OK);
}
