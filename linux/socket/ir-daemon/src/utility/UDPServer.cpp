//####
//# Legal Notice :
//#    Copyright, Harman International, 2017
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
/*-------------------------------------------------------------------------*
 * File:  UDPServer.cpp
 *-------------------------------------------------------------------------*/
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "Socket.h"
#include "UDPServer.h"
/*---------------------------------------------------------------------------*
 * Constants:
 *---------------------------------------------------------------------------*/
#define UDP_SERVER_POLL_INTERVAL        100 /* ms */

/*---------------------------------------------------------------------------*
 * Routine: UDPServer::UDPServer
 *---------------------------------------------------------------------------*/
/**
 * Create a new UDPServer object.  Use the create() function to set it
 * up and run it.  Until then, it is just a blank UDPServer object.
 *
 * <!------------------------------------------------------------------------*/
UDPServer::UDPServer() : Thread()
{
    iEventHandler = 0;
    isAttached = 0;
    iCanRun = 0;
    iDone = 0;
}

/*---------------------------------------------------------------------------*
 * Routine: UDPServerSocket::~UDPServerSocket
 *---------------------------------------------------------------------------*/
/**
 * End of UDPServerSocket.  No longer can run.
 *
 * <!------------------------------------------------------------------------*/
UDPServer::~UDPServer()
{
    iCanRun = 0;
    SVSI_SYSLOG("%s, destruct\n",__func__);
}

/*---------------------------------------------------------------------------*
 * Routine: UDPServerSocket::create
 *---------------------------------------------------------------------------*/
/**
 * Create a UDP Server socket.
 *
 * @param [in] aPort -- Port to serve up connections
 * @param [in] aMyIP -- IP number of this unit
 * @param [in] aName -- Name of the listener thread
 * @param [in] aEventHandler -- Object to receive accept events.  NOTE: This
 *      will be called on another thread!
 *
 * @return T_error
 *
 * <!------------------------------------------------------------------------*/
T_error UDPServer::create(
        unsigned short aPort,
        const IPAddress &aMyIP,
		int aIsBroadcast,
        const char *aName,
        UDPServerEventHandler *aEventHandler)
{
    if (isAttached)
        return ERROR_ALREADY_INITIALIZED;

    iPort = aPort;

    // Who handles events?
    iEventHandler = aEventHandler;

    SVSI_SYSLOG("UDP bind ip: %s:%d\n", aMyIP.asString(), aPort);

    aMyIP.clone(iAddress);
    iSocket = new UDPSocket;
    T_error error = iSocket->create(aMyIP,aIsBroadcast);
    if (error) {
        SVSI_SYSLOG("Could not create socket! Error %d", error);
    }
    iAddress.setPort(aPort);
    if (iSocket->Bind(iAddress) != ERROR_NONE) {
        SVSI_SYSLOG("UDP bind failure! Port (%d): %s\n", aPort, strerror(errno));
        return ERROR_COULD_NOT_BIND;
    }

    iDone = 0;
    iCanRun = 1;
    start(aName);

    return ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine: UDPServerSocket::run
 *---------------------------------------------------------------------------*/
/**
 * Run UDPServer Socket thread.accept the revent and call the event handler with the
 * opened socket.
 *
 * @return Always 0.
 *
 * <!------------------------------------------------------------------------*/
void *UDPServer::run()
{
    T_error error;
    SVSI_SYSLOG("UDPServer::run()\n");
    while (iCanRun) {
        error = iSocket->Poll(UDP_SERVER_POLL_INTERVAL);
        if (error == ERROR_NONE) {
            SVSI_SYSLOG("%s:%d: Poll event!\n", iName, iPort);
            error = iEventHandler->eventSocketAccepted(iSocket, iPort);
            if (error)
                SVSI_SYSLOG("%s: Error starting UDP Socket Handler", iName);
        } else if (error == ERROR_TIMEOUT) {
            // Do nothing.  Will loop around again.
        } else {
            SVSI_SYSLOG("Error with polling: %s\n", strerror(errno));
        }
    }
    if (iSocket)
    {
        iSocket->Close(); // the socket is freed too
        iSocket = nullptr;
    }

    iDone = 1;
    return 0;
}

T_error UDPServer::Stop()
{
    int timeout = 1000;

    iCanRun = 0;
    while ((!iDone) && (timeout--)) {
        usleep(1000);
    }

    if (timeout <= 0) {
        SVSI_SYSLOG("Timeout on UDPServer %d stop!", iPort);
        return ERROR_TIMEOUT;
    }

    SVSI_SYSLOG("UDPServer %d stopped!", iPort);
    return ERROR_NONE;
}
/*-------------------------------------------------------------------------*
 * End of File:  UDPServer.cpp
 *-------------------------------------------------------------------------*/
