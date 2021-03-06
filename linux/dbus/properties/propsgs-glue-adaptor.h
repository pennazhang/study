
/*
 *	This file was automatically generated by dbusxx-xml2cpp; DO NOT EDIT!
 */

#ifndef __dbusxx__propsgs_glue_adaptor_h__ADAPTOR_MARSHAL_H
#define __dbusxx__propsgs_glue_adaptor_h__ADAPTOR_MARSHAL_H

#include <dbus-c++/dbus.h>
#include <cassert>

namespace org {
namespace freedesktop {
namespace DBus {

class PropsGSDemo_adaptor
: public ::DBus::InterfaceAdaptor
{
public:

    PropsGSDemo_adaptor()
    : ::DBus::InterfaceAdaptor("org.freedesktop.DBus.PropsGSDemo")
    {
        bind_property(Version, "i", true, false);
        bind_property(Message, "s", true, true);
        bind_property(Data, "d", false, true);
    }

    ::DBus::IntrospectedInterface *introspect() const 
    {
        static ::DBus::IntrospectedArgument MessageChanged_args[] = 
        {
            { "message", "s", false },
            { 0, 0, 0 }
        };
        static ::DBus::IntrospectedArgument DataChanged_args[] = 
        {
            { "Data", "d", false },
            { 0, 0, 0 }
        };
        static ::DBus::IntrospectedMethod PropsGSDemo_adaptor_methods[] = 
        {
            { 0, 0 }
        };
        static ::DBus::IntrospectedMethod PropsGSDemo_adaptor_signals[] = 
        {
            { "MessageChanged", MessageChanged_args },
            { "DataChanged", DataChanged_args },
            { 0, 0 }
        };
        static ::DBus::IntrospectedProperty PropsGSDemo_adaptor_properties[] = 
        {
            { "Version", "i", true, false },
            { "Message", "s", true, true },
            { "Data", "d", false, true },
            { 0, 0, 0, 0 }
        };
        static ::DBus::IntrospectedInterface PropsGSDemo_adaptor_interface = 
        {
            "org.freedesktop.DBus.PropsGSDemo",
            PropsGSDemo_adaptor_methods,
            PropsGSDemo_adaptor_signals,
            PropsGSDemo_adaptor_properties
        };
        return &PropsGSDemo_adaptor_interface;
    }

public:

    /* properties exposed by this interface, use
     * property() and property(value) to get and set a particular property
     */
    ::DBus::PropertyAdaptor< int32_t > Version;
    ::DBus::PropertyAdaptor< std::string > Message;
    ::DBus::PropertyAdaptor< double > Data;

public:

    /* methods exported by this interface,
     * you will have to implement them in your ObjectAdaptor
     */

public:

    /* signal emitters for this interface
     */
    void MessageChanged(const std::string& arg1)
    {
        ::DBus::SignalMessage sig("MessageChanged");
        ::DBus::MessageIter wi = sig.writer();
        wi << arg1;
        emit_signal(sig);
    }
    void DataChanged(const double& arg1)
    {
        ::DBus::SignalMessage sig("DataChanged");
        ::DBus::MessageIter wi = sig.writer();
        wi << arg1;
        emit_signal(sig);
    }

private:

    /* unmarshalers (to unpack the DBus message before calling the actual interface method)
     */
};

} } } 
#endif //__dbusxx__propsgs_glue_adaptor_h__ADAPTOR_MARSHAL_H
