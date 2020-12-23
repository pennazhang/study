/*-------------------------------------------------------------------------*
 * File:  IrUDPServerHandler.h
 *-------------------------------------------------------------------------*/

#ifndef _IRUDPSERVERHANDLER_H_
#define _IRUDPSERVERHANDLER_H_

#include <string.h>
#include <iostream>
#include <Error.h>
#include "Socket.h"
#include "UDPServer.h"

class IrUDPServerHandler : public UDPServerEventHandler
{
    public:
        IrUDPServerHandler();
        virtual ~IrUDPServerHandler() {};
        T_error eventSocketAccepted(UDPSocket *aSocket, unsigned short aPort);
    private:
        void processUDPpacket(char * buffer, int pktLen, int sock,struct sockaddr* addr, unsigned short aPortNum);
};

#endif // _IRUDPSERVERHANDLER_H_
