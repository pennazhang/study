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
#include "jsIrUtils.h"
#include "CmdHandlerInterface.h"
#include "CmsfsIrInterface.h"
#include "ConfigManagerAPI.h"
#include "NetworkCfg.h"
#include "ClientSetting.h"
#include "ServerSetting.h"
#include <HxLogCpp.h>
#include "BaseCfgAPI.h"
#include "DataManagementCfg.h"

using NJarvis::HxLogCpp;

//
// Global Variables.
//
const char *DAEMON_NAME = "ir-daemon";
clock_t g_StartClock = 0;
HxLogger* g_logger = nullptr;

// Dbus connections.
DBus::BusDispatcher*    g_dbusDispatcherPtr;
DBus::Connection*       g_dbusConnPtr;

//
// DBus Interface.
//
CmdHandlerInterface* g_pCmdHandlerInterface;
CmsfsIrInterface* g_pCmsfsIrInterface;

//
// Clean up all resource in this file before quit the appliation.
//
void cleanup()
{
    logDebug("[%s]%s.", DAEMON_NAME, __FUNCTION__);

    if (g_pCmdHandlerInterface) {
        delete g_pCmdHandlerInterface;
    }
    if (g_pCmsfsIrInterface)
    {
        delete g_pCmsfsIrInterface;
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

#ifdef DBUS_SYSTEM_BUS
    g_dbusConnPtr = new DBus::Connection(DBus::Connection::SystemBus());
#else
    g_dbusConnPtr = new DBus::Connection(DBus::Connection::SessionBus());
#endif

    g_pCmsfsIrInterface = new CmsfsIrInterface(*g_dbusConnPtr);
    g_pCmdHandlerInterface = new CmdHandlerInterface(*g_dbusConnPtr);
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
    g_client.stop();
    g_server.stop();
    exit(1);
}

//
// Process Start.
//
STATUS main(int argc, char** argv)
{
    SingletonProcess singletonProcess;

    /* Only one instance for this application */
    if (singletonProcess.isSingleton() == false)
    {
        fprintf(stderr, "Another instance is already running.\n");
        exit(1);
   }

    // Initial the start clock.
    g_StartClock = clock();

    // Logger initialization.
    g_logger = hxLogGetLogger("ir-daemon");
    printf("g_logger - %p\n", g_logger);
    hxLogSetLevel(g_logger, HXLOG_DEBUG);
    hxInfo(g_logger, "IR-Daemon starts.");

    static std::unique_ptr<HxLogCpp> _logger = make_unique<HxLogCpp>("ir-Daemon", true);
    _logger->setLevel(HXLOG_DEBUG);
    _logger->d << "IRMain connection DNE" << endl;
    _logger->i << "IRMain connection DNE" << endl;
    _logger->w << "MD5 set failed." << endl;
    _logger->e << "MD5 set failed." << endl;
    _logger->f << "IRMain connection DNE" << endl;

//    return (0);
    // Signal Handlers
    signal(SIGTERM, sighandler);
    signal(SIGINT, sighandler);

    // Setup DBUS connection to cmsfs since we need config early
    // Create and start the DBus dispatcher
    startDBusDispatcher();

    // get the global settings.
    g_irSetting.getIrConfig();

    logDebug("The Local IP Address is : %d.%d.%d.%d",
                        g_irSetting.m_myip[0],
                        g_irSetting.m_myip[1],
                        g_irSetting.m_myip[2],
                        g_irSetting.m_myip[3]);

    logDebug("Processing...");

    /* Start the thread of client */
    g_client.start(g_irSetting.m_passThroughFlag, g_irSetting.m_irPassThroughPeerIP);

    /* start the thread of server */
    g_server.start(g_irSetting.m_passThroughFlag, g_irSetting.m_irPassThroughPeerIP);

    // Setting readytorun flag to start handling datachanged signals
    g_pCmsfsIrInterface->SetReadyRun(true);

    g_dbusDispatcherPtr->enter();

    // Finished running...
    cleanup();

    logDebug("Daemon exited.");

    exit(STATUS_OK);
}
