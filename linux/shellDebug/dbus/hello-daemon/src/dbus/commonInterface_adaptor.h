
/*
 *	This file was automatically generated by dbusxx-xml2cpp; DO NOT EDIT!
 */

#ifndef __dbusxx___home_pzhang_workspace_study_linux_shellDebug_dbus_hello_daemon_src_dbus_commonInterface_adaptor_h__ADAPTOR_MARSHAL_H
#define __dbusxx___home_pzhang_workspace_study_linux_shellDebug_dbus_hello_daemon_src_dbus_commonInterface_adaptor_h__ADAPTOR_MARSHAL_H

#include <dbus-c++/dbus.h>
#include <cassert>

namespace com {
namespace harman {
namespace jetstream {

class commonInterface_adaptor
: public ::DBus::InterfaceAdaptor
{
public:

    commonInterface_adaptor()
    : ::DBus::InterfaceAdaptor("com.harman.jetstream.commonInterface")
    {
        register_method(commonInterface_adaptor, get, _get_stub);
        register_method(commonInterface_adaptor, set, _set_stub);
        register_method(commonInterface_adaptor, daemonControl, _daemonControl_stub);
    }

    ::DBus::IntrospectedInterface *introspect() const 
    {
        static ::DBus::IntrospectedArgument get_args[] = 
        {
            { "keyList", "as", true },
            { "jsonResult", "s", false },
            { 0, 0, 0 }
        };
        static ::DBus::IntrospectedArgument set_args[] = 
        {
            { "jsonPair", "s", true },
            { "jsonResult", "s", false },
            { 0, 0, 0 }
        };
        static ::DBus::IntrospectedArgument daemonControl_args[] = 
        {
            { "cmd", "s", true },
            { "jsonParam", "s", true },
            { "jsonResult", "s", false },
            { 0, 0, 0 }
        };
        static ::DBus::IntrospectedMethod commonInterface_adaptor_methods[] = 
        {
            { "get", get_args },
            { "set", set_args },
            { "daemonControl", daemonControl_args },
            { 0, 0 }
        };
        static ::DBus::IntrospectedMethod commonInterface_adaptor_signals[] = 
        {
            { 0, 0 }
        };
        static ::DBus::IntrospectedProperty commonInterface_adaptor_properties[] = 
        {
            { 0, 0, 0, 0 }
        };
        static ::DBus::IntrospectedInterface commonInterface_adaptor_interface = 
        {
            "com.harman.jetstream.commonInterface",
            commonInterface_adaptor_methods,
            commonInterface_adaptor_signals,
            commonInterface_adaptor_properties
        };
        return &commonInterface_adaptor_interface;
    }

public:

    /* properties exposed by this interface, use
     * property() and property(value) to get and set a particular property
     */

public:

    /* methods exported by this interface,
     * you will have to implement them in your ObjectAdaptor
     */
    virtual std::string get(const std::vector< std::string >& keyList) = 0;
    virtual std::string set(const std::string& jsonPair) = 0;
    virtual std::string daemonControl(const std::string& cmd, const std::string& jsonParam) = 0;

public:

    /* signal emitters for this interface
     */

private:

    /* unmarshalers (to unpack the DBus message before calling the actual interface method)
     */
    ::DBus::Message _get_stub(const ::DBus::CallMessage &call)
    {
        ::DBus::MessageIter ri = call.reader();

        std::vector< std::string > argin1; ri >> argin1;
        std::string argout1 = get(argin1);
        ::DBus::ReturnMessage reply(call);
        ::DBus::MessageIter wi = reply.writer();
        wi << argout1;
        return reply;
    }
    ::DBus::Message _set_stub(const ::DBus::CallMessage &call)
    {
        ::DBus::MessageIter ri = call.reader();

        std::string argin1; ri >> argin1;
        std::string argout1 = set(argin1);
        ::DBus::ReturnMessage reply(call);
        ::DBus::MessageIter wi = reply.writer();
        wi << argout1;
        return reply;
    }
    ::DBus::Message _daemonControl_stub(const ::DBus::CallMessage &call)
    {
        ::DBus::MessageIter ri = call.reader();

        std::string argin1; ri >> argin1;
        std::string argin2; ri >> argin2;
        std::string argout1 = daemonControl(argin1, argin2);
        ::DBus::ReturnMessage reply(call);
        ::DBus::MessageIter wi = reply.writer();
        wi << argout1;
        return reply;
    }
};

} } } 
#endif //__dbusxx___home_pzhang_workspace_study_linux_shellDebug_dbus_hello_daemon_src_dbus_commonInterface_adaptor_h__ADAPTOR_MARSHAL_H
