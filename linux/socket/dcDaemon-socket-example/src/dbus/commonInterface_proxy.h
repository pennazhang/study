
/*
 *	This file was automatically generated by dbusxx-xml2cpp; DO NOT EDIT!
 */

#ifndef __dbusxx__commonInterface_proxy_h__PROXY_MARSHAL_H
#define __dbusxx__commonInterface_proxy_h__PROXY_MARSHAL_H

#include <dbus-c++/dbus.h>
#include <cassert>
#include <exception>
#include "utility.h"

namespace com {
namespace harman {
namespace jetstream {

class commonInterface_proxy
: public ::DBus::InterfaceProxy
{
public:

    commonInterface_proxy()
    : ::DBus::InterfaceProxy("com.harman.jetstream.commonInterface")
    {
    }

public:

    /* properties exported by this interface */
public:

    /* methods exported by this interface,
     * this functions will invoke the corresponding methods on the remote objects
     */
    std::string get(const std::vector< std::string >& keyList)
    {
        ::DBus::CallMessage call;
        ::DBus::MessageIter wi = call.writer();

        try
        {
            wi << keyList;
            call.member("get");
            ::DBus::Message ret = invoke_method (call);
            ::DBus::MessageIter ri = ret.reader();

            std::string argout;
            ri >> argout;
            return argout;
        }
        catch (std::exception &e)
        {
            logError("DBus get exception : %s", e.what());
            return (resultInfo(STATUS_ERROR, e.what()));
        }        
    }

    std::string set(const std::string& jsonPair)
    {
        ::DBus::CallMessage call;
        ::DBus::MessageIter wi = call.writer();

        try
        {
            wi << jsonPair;
            call.member("set");
            ::DBus::Message ret = invoke_method (call);
            ::DBus::MessageIter ri = ret.reader();

            std::string argout;
            ri >> argout;
            return argout;
        }
        catch (std::exception &e)
        {
            logError("DBus get exception : %s", e.what());
            return (resultInfo(STATUS_ERROR, e.what()));
        }        
    }

    std::string daemonControl(const std::string& cmd, const std::string& jsonParam)
    {
        ::DBus::CallMessage call;
        ::DBus::MessageIter wi = call.writer();

        try
        {
            wi << cmd;
            wi << jsonParam;
            call.member("daemonControl");
            ::DBus::Message ret = invoke_method (call);
            ::DBus::MessageIter ri = ret.reader();

            std::string argout;
            ri >> argout;
            return argout;
        }
        catch (std::exception &e)
        {
            logError("DBus get exception : %s", e.what());
            return (resultInfo(STATUS_ERROR, e.what()));
        }        
    }

public:

    /* signal handlers for this interface
     */

private:

    /* unmarshalers (to unpack the DBus message before calling the actual signal handler)
     */
};

} } } 
#endif //__dbusxx__commonInterface_proxy_h__PROXY_MARSHAL_H
