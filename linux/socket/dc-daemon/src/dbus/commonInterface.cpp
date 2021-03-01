#include "commonInterface.h"
#include <cstdio>
#include <iostream>
#include <signal.h>
#include <json/json.h>
#include "utility.h"

using namespace std;

CommonInterface::CommonInterface(DBus::Connection &connection, const char *objectPath, const char *busName)
: DBus::ObjectProxy(connection, objectPath, busName)
{
}
