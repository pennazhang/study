/*-------------------------------------------------------------------------*
 * File:  Socket.h - Copyright (C) 2013 All Rights Reserved - SVSi
 *-------------------------------------------------------------------------*/
#ifndef _SVSI_SOCKET_H_
#define _SVSI_SOCKET_H_

#include <netinet/in.h>
#include "Error.h"
#include "IPAddress.h"
#include "Thread.h"
#include "config.h"

/*---------------------------------------------------------------------------*
 * Class: Socket
 *---------------------------------------------------------------------------*/
/**
 * Base socket class for tracking socket actions and performing basic
 * functions.
 *
 * <!------------------------------------------------------------------------*/
class Socket {
    public:
        int iSock;
    public:
        Socket() { iSock = -1; }
        virtual T_error Bind(IPAddress &aIP);
        virtual T_error Listen(unsigned int aMaxIncoming);
        virtual T_error Poll(int aTimeoutMS);
        virtual T_error Accept(Socket &aNewSock);
        virtual T_error GetPeerIP(IPAddress &aPeerIP) const;
        virtual T_error Receive(void *aBuffer, int aLength, int *aNumRead) = 0;
        virtual T_error Send(const void *aBuffer, int aLength, int *aNumWritten);
        virtual void Close(void);
        virtual int isUDP(void) = 0;
        virtual void SetPeerIP(IPAddress& aPeerIP) {}
        virtual T_error Open(IPAddress &aDestination) = 0;
};

/*---------------------------------------------------------------------------*
 * Class: TCPSocket
 *---------------------------------------------------------------------------*/
/**
 * A TCPSocket is merely a Socket that was created for IPv4 or IPv6 based
 * TCP communications.
 *
 * <!------------------------------------------------------------------------*/
class TCPSocket : public Socket {
    public:
        TCPSocket();
        T_error create(const IPAddress &aMyIP);
        virtual void Close(void);
        int isUDP(void) { return 0; }
        T_error Open(IPAddress &aDestination);
        T_error Receive(void *aBuffer, int aLength, int *aNumRead);
};

/*---------------------------------------------------------------------------*
 * Class: UDPSocket
 *---------------------------------------------------------------------------*/
/**
 * A UDPSocket is merely a Socket that was created for IPv4 or IPv6 based
 * UDP communications.
 *
 * <!------------------------------------------------------------------------*/
class UDPSocket : public Socket {
    public:
        UDPSocket();
        T_error create(const IPAddress &aMyIP, int aIsBroadcast);
        T_error Receive(void *aBuffer, int aLength, int *aNumRead);
        T_error Send(const void *aBuffer, int aLength, int *aNumWritten);
        T_error GetPeerIP(IPAddress &aPeerIP) const;
        void SetPeerIP(IPAddress& aPeerIP) { iPeerIP = aPeerIP; }
        int isUDP(void) { return 1; }
        int isBroadcast(void) { return m_isBroadcast; }
        T_error Open(IPAddress &aDestination);
        T_error SendTo(IPAddress& aDestination, const void *aBuffer, int aLength, int *aNumWritten);
    public:
        IPAddress iPeerIP;
        IPAddress iMyIP;
        unsigned short iPeerPort;
        int m_isBroadcast;
};

#endif /* _SVSI_SOCKET_H_ */
/*-------------------------------------------------------------------------*
 * End of File:  Socket.h - Copyright (C) 2013 All Rights Reserved - SVSi
 *-------------------------------------------------------------------------*/
