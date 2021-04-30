#include <json/json.h>
#include "dcDaemonServer.h"
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <stdio.h>
#include <limits.h>
#include <iostream>
#include "utility.h"

using namespace std;

DcDaemonServer::DcDaemonServer(DBus::Connection &connection, const char *ObjectPath)
: DBus::ObjectAdaptor(connection, ObjectPath)
{
}

DcDaemonServer *g_pDcDaemonServer = nullptr;
