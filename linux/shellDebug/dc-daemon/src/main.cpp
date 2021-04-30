#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "commonInterface.h"
#include <cstdio>
#include <iostream>
#include <signal.h>
#include <json/json.h>
#include <stdlib.h> // exit()
#include <string.h> // memset()
#include <unistd.h> // close()
#include <sys/socket.h>
#include <netinet/in.h> // IPPROTO_TCP, htons(), htons()
#include <arpa/inet.h>  // inet_addr()
#include "mainThread.h"
#include "tcpServer.h"
#include "tcpSocket.h"
#include "config.h"
#include "main.h"
#include "sslLib.h"
#include "dcUtility.h"
#include "dcDaemonServer.h"

using namespace std;

//
// Global Variables.
//
MainThread *g_mainThread;
clock_t g_StartClock = 0;

static const char *DCSERVER_BUS_NAME = "com.harman.jetstream.dcDaemon";
static const char *DCSERVER_OBJECT_PATH = "/com/harman/jetstream/dcDaemon";

//#define KVM_DAEMON "kvm-Daemon"
//static const char *KVM_BUS_NAME = "com.harman.jetstream.kvmDaemon";
//static const char *KVM_OBJECT_PATH = "/com/harman/jetstream/kvmDaemon";

// ir-Daemon
static const char *IR_BUS_NAME = "com.harman.jetstream.IrDaemon";
static const char *IR_OBJECT_PATH = "/com/harman/jetstream/irDaemon";

// av-Daemon
static const char *AV_BUS_NAME = "com.harman.jetstream.avdaemon";
static const char *AV_OBJECT_PATH = "/com/harman/jetstream/avdaemon";

// kvm-Daemon
static const char *KVM_BUS_NAME = "com.harman.jetstream.KvmDaemon";
static const char *KVM_OBJECT_PATH = "/com/harman/jetstream/KvmDaemon";

// serialDaemon
static const char *SERIAL_BUS_NAME = "com.harman.jetstream.SerialDaemon";
static const char *SERIAL_OBJECT_PATH = "/com/harman/jetstream/serialDaemon";

/* Used to store the pid of dc-daemon */
const char g_SingletonFileName[] = "/tmp/dc-daemon-singleton"; 

DBus::BusDispatcher dispatcher;

BOOL g_quitApp = false;

void sighandler(int sig)
{
    /* Stop the MainThread and it's sub thread  */
    g_mainThread->stop();

    dispatcher.leave();
    exitCTX();
    exit(1);
}


int main(int argc, char **argv)
{
    SingletonProcess singletonProcess(g_SingletonFileName);

    /* Only one instance for this application */
    if (singletonProcess.isSingleton() == false)
    {
		logError("Another instance is already running.");
        exit(1);
    }

    /* Set DBus Interface */
    DBus::default_dispatcher = &dispatcher;
    DBus::_init_threading();

#ifdef SSL_SOCKET  
    ASSERT(initCTX() == STATUS_OK);
#endif    

    signal(SIGTERM, sighandler);
    signal(SIGINT, sighandler);

    DBus::Connection conn = DBus::Connection::SessionBus();

	conn.request_name(DCSERVER_BUS_NAME);
	DcDaemonServer server(conn, DCSERVER_OBJECT_PATH);
    g_pDcDaemonServer = &server;

    /* Create IR CommonInterface */
    CommonInterface irCommonInterface(conn, IR_OBJECT_PATH, IR_BUS_NAME);
    g_irCommonInterface = &irCommonInterface;

    /* Create AV CommonInterface */
    CommonInterface avCommonInterface(conn, AV_OBJECT_PATH, AV_BUS_NAME);
    g_avCommonInterface = &avCommonInterface;

    /* Create serial CommonInterface */
    CommonInterface serialCommonInterface(conn, SERIAL_OBJECT_PATH, SERIAL_BUS_NAME);
    g_serialCommonInterface = &serialCommonInterface;

    /* Create KVM CommonInterface */
    CommonInterface kvmCommonInterface(conn, KVM_OBJECT_PATH, KVM_BUS_NAME);
    g_kvmCommonInterface = &kvmCommonInterface;

    logInfo("Goodluck!");

    /* Start the main thread */
    g_mainThread = new MainThread("MainThread");
    g_mainThread->start();

    /* Enter DBus loop */
    dispatcher.enter();
    return 0;
}
