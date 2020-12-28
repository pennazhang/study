
/*
 *	This file was automatically generated by dbusxx-xml2cpp; DO NOT EDIT!
 */

#ifndef __dbusxx___home_pzhang_temp_jetstream_daemons_ir_daemon_src_dbus_CmdHandlerManagerProxy_glue_h__PROXY_MARSHAL_H
#define __dbusxx___home_pzhang_temp_jetstream_daemons_ir_daemon_src_dbus_CmdHandlerManagerProxy_glue_h__PROXY_MARSHAL_H

#include <dbus-c++/dbus.h>
#include <cassert>

namespace com {
namespace harman {
namespace videoworx {

class CmdHandlerManager_proxy
: public ::DBus::InterfaceProxy
{
public:

    CmdHandlerManager_proxy()
    : ::DBus::InterfaceProxy("com.harman.videoworx.CmdHandlerManager")
    {
        connect_signal(CmdHandlerManager_proxy, processSettings, _processSettings_stub);
        connect_signal(CmdHandlerManager_proxy, sendIR, _sendIR_stub);
        connect_signal(CmdHandlerManager_proxy, sendIRRaw, _sendIRRaw_stub);
        connect_signal(CmdHandlerManager_proxy, TestIRCommand, _TestIRCommand_stub);
        connect_signal(CmdHandlerManager_proxy, repeatIR, _repeatIR_stub);
        connect_signal(CmdHandlerManager_proxy, stopIR, _stopIR_stub);
    }

public:

    /* properties exported by this interface */
public:

    /* methods exported by this interface,
     * this functions will invoke the corresponding methods on the remote objects
     */
    void getCmdResponse(const std::string& cmdStr, const uint8_t& cmdType, const int32_t& errCode, const std::string& errStr)
    {
        ::DBus::CallMessage call;
        ::DBus::MessageIter wi = call.writer();

        wi << cmdStr;
        wi << cmdType;
        wi << errCode;
        wi << errStr;
        call.member("getCmdResponse");
        ::DBus::Message ret = invoke_method (call);
    }


public:

    /* signal handlers for this interface
     */
    virtual void processSettings(const std::string& cmdStr, const uint8_t& cmdType, const std::string& cmdArg) = 0;
    virtual void sendIR(const std::string& cmdStr, const uint8_t& cmdType, const std::string& cmdArg) = 0;
    virtual void sendIRRaw(const std::string& cmdStr, const uint8_t& cmdType, const std::string& cmdArg) = 0;
    virtual void TestIRCommand(const std::string& cmdStr, const uint8_t& cmdType) = 0;
    virtual void repeatIR(const std::string& cmdStr, const uint8_t& cmdType, const std::string& cmdArg) = 0;
    virtual void stopIR(const std::string& cmdStr, const uint8_t& cmdType) = 0;

private:

    /* unmarshalers (to unpack the DBus message before calling the actual signal handler)
     */
    void _processSettings_stub(const ::DBus::SignalMessage &sig)
    {
        ::DBus::MessageIter ri = sig.reader();

        std::string cmdStr;
        ri >> cmdStr;
        uint8_t cmdType;
        ri >> cmdType;
        std::string cmdArg;
        ri >> cmdArg;
        processSettings(cmdStr, cmdType, cmdArg);
    }
    void _sendIR_stub(const ::DBus::SignalMessage &sig)
    {
        ::DBus::MessageIter ri = sig.reader();

        std::string cmdStr;
        ri >> cmdStr;
        uint8_t cmdType;
        ri >> cmdType;
        std::string cmdArg;
        ri >> cmdArg;
        sendIR(cmdStr, cmdType, cmdArg);
    }
    void _sendIRRaw_stub(const ::DBus::SignalMessage &sig)
    {
        ::DBus::MessageIter ri = sig.reader();

        std::string cmdStr;
        ri >> cmdStr;
        uint8_t cmdType;
        ri >> cmdType;
        std::string cmdArg;
        ri >> cmdArg;
        sendIRRaw(cmdStr, cmdType, cmdArg);
    }
    void _TestIRCommand_stub(const ::DBus::SignalMessage &sig)
    {
        ::DBus::MessageIter ri = sig.reader();

        std::string cmdStr;
        ri >> cmdStr;
        uint8_t cmdType;
        ri >> cmdType;
        TestIRCommand(cmdStr, cmdType);
    }
    void _repeatIR_stub(const ::DBus::SignalMessage &sig)
    {
        ::DBus::MessageIter ri = sig.reader();

        std::string cmdStr;
        ri >> cmdStr;
        uint8_t cmdType;
        ri >> cmdType;
        std::string cmdArg;
        ri >> cmdArg;
        repeatIR(cmdStr, cmdType, cmdArg);
    }
    void _stopIR_stub(const ::DBus::SignalMessage &sig)
    {
        ::DBus::MessageIter ri = sig.reader();

        std::string cmdStr;
        ri >> cmdStr;
        uint8_t cmdType;
        ri >> cmdType;
        stopIR(cmdStr, cmdType);
    }
};

} } } 
#endif //__dbusxx___home_pzhang_temp_jetstream_daemons_ir_daemon_src_dbus_CmdHandlerManagerProxy_glue_h__PROXY_MARSHAL_H