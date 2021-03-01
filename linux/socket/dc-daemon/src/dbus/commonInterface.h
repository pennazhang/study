#ifndef __COMMON_INTERFACE_H
#define __COMMON_INTERFACE_H

#include <dbus-c++/dbus.h>
#include "commonInterface_proxy.h"
#include <map>

using namespace std;

class CommonInterface
: public com::harman::jetstream::commonInterface_proxy,
  public DBus::IntrospectableProxy,
  public DBus::ObjectProxy
{
public:

    CommonInterface(DBus::Connection &connection, const char *objectPath, const char *busName);
};

#endif//__COMMON_INTERFACE_H
