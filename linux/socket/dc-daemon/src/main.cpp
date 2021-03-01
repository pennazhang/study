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
#include "msgThread.h"
#include "tcpServer.h"
#include "tcpSocket.h"
#include "config.h"
#include "main.h"
#include "sslLib.h"

using namespace std;

ModuleThread *g_moduleThread;
NetworkThread *g_networkThread;

//#define KVM_DAEMON "kvm-Daemon"
//static const char *KVM_BUS_NAME = "com.harman.jetstream.kvmDaemon";
//static const char *KVM_OBJECT_PATH = "/com/harman/jetstream/kvmDaemon";

#define IR_DAEMON "ir-Daemon"
static const char *IR_BUS_NAME = "com.harman.jetstream.irDaemon";
static const char *IR_OBJECT_PATH = "/com/harman/jetstream/irDaemon";

DBus::BusDispatcher dispatcher;

BOOL g_quitApp = false;


void test()
{
  cout << "sizeof(sockaddr_in) = " << sizeof(sockaddr_in) << endl;
  cout << "sizeof(sockaddr) = " << sizeof(sockaddr) << endl;
  cout << "sizeof(unsigned long) = " << sizeof(unsigned long) << endl;
  cout << "sizeof(int) = " << sizeof(int) << endl;
  cout << "sizeof(void *) = " << sizeof(void *) << endl;

  if (initCTX() == STATUS_ERROR)
  {
    cout << "initCTX failed!\n";
    return;
  }


  g_moduleThread = new ModuleThread("ModuleThread");
  g_moduleThread->start();

  g_networkThread = new NetworkThread("NetworkThread");
  g_networkThread->start();

#if 0
  TestMessage testMsg(0);
  g_moduleThread->sendMessage(&testMsg);
#endif
  for (;;)
  {
    Thread::mSleep(1000);
    if (g_quitApp == true)
    {
      break;
    }
  }

  g_moduleThread->stop();
  delete g_moduleThread;

  g_networkThread->stop();
  delete g_networkThread;

  exitCTX();
}

void sighandler(int sig)
{
  g_quitApp = true;
}

int main()
{
  signal(SIGTERM, sighandler);
  signal(SIGINT, sighandler);
  test();
  return 0;
  DBus::default_dispatcher = &dispatcher;

  DBus::Connection conn = DBus::Connection::SessionBus();

  CommonInterface irDaemon(conn, IR_OBJECT_PATH, IR_BUS_NAME);
  //  CommonInterface kvmDaemon(conn, KVM_OBJECT_PATH, KVM_BUS_NAME);

  /* Run IR Command */
  /* Let execute command here */
  for (int i = 0; i < 5; i++)
  {
    //    int result;
    std::string outputInfo;

    cout << "test time: " << i << endl;
    cout << "------------------------------------------------------------------" << endl;

    std::vector<std::string> keyList = {"passThroughFlag", "passThroughIP", "txEnableFlag", "rxEnableFlag"};
    std::string resultJson = irDaemon.get(keyList);
    cout << "get method returned: " << resultJson << endl
         << endl;

    Json::Value jsonSetting;
    jsonSetting["commandHoldoff"] = 20;
    jsonSetting["repeatHoldoff"] = 90;
    cout << jsonSetting.toStyledString() << endl;
    outputInfo = irDaemon.set(resultJson);
    cout << "set method returned: " << outputInfo << endl
         << endl;

    Json::Value param;
    param["prontoCode"] = "0000 006d 0011 0002 0015 0040";
    param["count"] = 6;
    cout << param.toStyledString() << endl;
    resultJson = irDaemon.daemonControl("sendIRRaw", param.toStyledString());
    cout << "daemonControl returned: " << resultJson << endl;
    cout << endl;
  }

#if 0 
  /* Run KVM Command */
	  std::vector<std::string> keyList = { "baud", "stopBit", "pause", "parity"};
	  std::string resultJson = kvmDaemon.get(keyList);
    cout << "get method returned: " << resultJson << endl << endl;

    Json::Value jsonSetting;
    jsonSetting["usb20Enable"] = "true";
    jsonSetting["USB20_IP_ADDRESS"] = "192.168.1.100";
    cout << jsonSetting.toStyledString() << endl;
  	outputInfo = kvmDaemon.set(resultJson);
    cout << "set method returned: " << outputInfo << endl << endl;

		Json::Value param;
		param["value"] = "ps -a";
    cout << param.toStyledString() << endl;
		resultJson = kvmDaemon.daemonControl("sendString", param.toStyledString());
    cout << "daemonControl returned: " << resultJson << endl;
    cout << endl;
#endif
  return 0;
}
