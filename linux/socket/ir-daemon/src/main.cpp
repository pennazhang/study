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
#include "videoEP.h"
#include "UDPServer.h"
#include "IrUDPServerHandler.h"
#include "irUtilities.h"
#include "CmdHandlerInterface.h"
#include "CmsfsIrInterface.h"
#include "ConfigManagerAPI.h"
#include "NetworkCfg.h"


const char *DAEMON_NAME = "ir-daemon";
//
// Global Variables.
//
clock_t gStartClock = 0;
HxLogger* logger = nullptr;

// The Golbal stuct.
VE_Settings_t G_veSet = {0};
irParamsFIFO_t G_irCommandFIFO = {0};

// Dbus connections.
DBus::BusDispatcher*    dbusDispatcherPtr;
DBus::Connection*       dbusConnPtr;

//
// DBus Interface.
//
CmdHandlerInterface* m_pCmdHandlerInterface;
CmsfsIrInterface* m_pCmsfsIrInterface;

// UDP Server and Handler.
UDPServer* m_pIrUDPServer = nullptr;
IrUDPServerHandler* m_pIrUDPServerHandler = nullptr;

void cleanup()
{
    hxDebug(logger, "[%s]%s.", DAEMON_NAME, __FUNCTION__);

    if (m_pCmdHandlerInterface) {
        delete m_pCmdHandlerInterface;
    }
    if (m_pCmsfsIrInterface)
    {
        delete m_pCmsfsIrInterface;
    }
    if (m_pIrUDPServer) {
        delete m_pIrUDPServer;
    }
    if (m_pIrUDPServerHandler) {
        delete m_pIrUDPServerHandler;
    }
    if (dbusDispatcherPtr) {
        delete dbusDispatcherPtr;
    }
    if (dbusConnPtr) {
        delete dbusConnPtr;
    }
}

void coastDBus()
{
    hxDebug(logger, "[%s]%s.", DAEMON_NAME, __FUNCTION__);
    dbusDispatcherPtr->enter();
}

void startDBusDispatcher()
{
    dbusDispatcherPtr = new DBus::BusDispatcher();

    DBus::_init_threading();
    DBus::default_dispatcher = dbusDispatcherPtr;

#ifdef DBUS_SYSTEM_BUS
    dbusConnPtr = new DBus::Connection(DBus::Connection::SystemBus());
#else
    dbusConnPtr = new DBus::Connection(DBus::Connection::SessionBus());
#endif
    // Request the dbus service.
    ///dbusConnPtr->request_name(IR_SERVER_NAME);
}

void stopDBus()
{
    if (dbusDispatcherPtr)
    {
        dbusDispatcherPtr->leave();
    }
}

void getIrConfig(void)
{
    std::map<std::string,std::string> m_sconfig = m_pCmsfsIrInterface->getAll();

    if (!m_sconfig.empty())
    {
        // Debug
        std::cout << "Config From CMFS: " << std::endl;
        for (auto const& x : m_sconfig)
        {
            std::cout << x.first  // string (key)
                << ':' 
                << x.second // string's value 
                << std::endl ;
        } 
        std::cout << std::endl;
        //end debug

        // Set ir parameters
        G_irCommandFIFO.timeCommandHoldoff = std::stoi(m_sconfig[IR_COMMAND_HOLD_OFF], nullptr, 0);
        G_irCommandFIFO.timeRepeatHoldoff = std::stoi(m_sconfig[IR_REPEAT_HOLD_OFF], nullptr, 0);
        //Setup my IP
        //establishes a connection to ConfigManagerService
        cmAPI* api = getCMApi();
  
        //NetworkCfg net instantiation gives access to network configuration sets/gets
        NetworkCfg net(api);
        std::string ipAddr = net.getIP();  // Currently eth0 link local. Should be br0/br1 once we get Jarvis support
        int err = inet_pton(AF_INET, ipAddr.c_str(), G_veSet.myip);
        if (err < 1) {
            printf("Invalid IPv4 Address, set to default.");
            G_veSet.myip[0] = 0;
            G_veSet.myip[1] = 0;
            G_veSet.myip[2] = 0;
            G_veSet.myip[3] = 0;
        }
        else
        {
            strncpy(G_veSet.currentIP, ipAddr.c_str(), 32);
        }
    }
    else
    {
        //empty config is not good
        hxDebug(logger, "[%s] Config Not Found In CMSFS", DAEMON_NAME);
        std::cerr << DAEMON_NAME << " Config Not Found In CMSFS." << std::endl;
        exit(1);
    }
}

void sighandler(int sig)
{
    hxDebug(logger, "[%s]%s.", DAEMON_NAME, __FUNCTION__);

    stopDBus();
    exit(1);
}

void* irServiceThread(void *arg)
{
	VE_Settings_t *veSetLocal = (VE_Settings_t*)arg;
    ///VideoEP_MainLoop(veSetLocal);
    pthread_exit(NULL);
}

//
// Process Start.
//
int main(int argc, char** argv)
{
    pthread_t thread_ir_service;
    pthread_attr_t attr_ir_service;

    // Logger initialization.
    logger = hxLogGetLogger(DAEMON_NAME);
    hxLogSetLevel(logger, HXLOG_DEBUG);

    // Signal Handlers
    signal(SIGTERM, sighandler);
    signal(SIGINT, sighandler);

    // Setup DBUS connection to cmsfs since we need config early
    // Create and start the DBus dispatcher
    startDBusDispatcher();

    m_pCmsfsIrInterface = new CmsfsIrInterface(*dbusConnPtr);

    if (argc > 1)
    {
        strncpy(G_veSet.currentIP, argv[1], 32);

        int err = inet_pton(AF_INET, G_veSet.currentIP, G_veSet.myip);
        if (err < 1) {
            printf("Invalid IPv4 Address, set to default.");
            G_veSet.myip[0] = 0;
            G_veSet.myip[1] = 0;
            G_veSet.myip[2] = 0;
            G_veSet.myip[3] = 0;
            strncpy(G_veSet.currentIP, "0.0.0.0", 32);
        }
    }
    else
    {
        //Retrieve all serial service config from cmsfs
        getIrConfig();
    }

    printf("The Local IP Address is : %d.%d.%d.%d\n",
                        G_veSet.myip[0],
                        G_veSet.myip[1],
                        G_veSet.myip[2],
                        G_veSet.myip[3]);


    SVSI_SYSLOG("[%s] startup", DAEMON_NAME);
    // Initial the start clock.
    gStartClock = clock();

    hxDebug(logger, "[%s] Processing...", DAEMON_NAME);

    // Create an interface to the CmdHandlerInterface.
    m_pCmdHandlerInterface = new CmdHandlerInterface(*dbusConnPtr);

    m_pIrUDPServerHandler = new IrUDPServerHandler;
    m_pIrUDPServer = new UDPServer;

    int ret =  m_pIrUDPServer->create((unsigned short)VOLANTE_PORT_IR, IPAddress(G_veSet.currentIP), 0, DAEMON_NAME, m_pIrUDPServerHandler);
    if(ret != 0)
    {
        hxDebug(logger, "UDP Server Create failed!\n");
        exit(1);
    }

    //pthread_attr_init(&attr_ir_service);
    //pthread_attr_setdetachstate(&attr_ir_service, PTHREAD_CREATE_DETACHED);
//
    //int rc = pthread_create(&thread_ir_service, &attr_ir_service,
    //            irServiceThread,
    //            (void *)&G_veSet);

    // Setting readytorun flag to start handling datachanged signals
    m_pCmsfsIrInterface->SetReadyRun(true);

    coastDBus();
    // Finished running...
    cleanup();

    hxDebug(logger, "[%s] exited.", DAEMON_NAME);

    exit(EX_OK);
}