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
 * File:  UDPServer.hpp
 *-------------------------------------------------------------------------*/
#ifndef _UDPSERVER_H_
#define _UDPSERVER_H_

#include "Error.h"
#include "Socket.h"
/*****************************************************************************/
/**
*   \brief     UDPServer Class
*   \details   UDPServer is a class to manage
*              udp port listening socket .
*   \author    Yvonne.Chen
*   \version   0.0.1
*   \date      2020
*   \pre       None
*   \bug       None
*   \warning   Improper use can crash your application
*   \copyright Harman International
*****************************************************************************/

/*---------------------------------------------------------------------------*
 * Class: UDPServerEventHandler
 *---------------------------------------------------------------------------*/
/**
 * Virtual event handler for the UDP Socket Server.  Put onto class that
 * needs to receive UDP server events.
 *
 * <!------------------------------------------------------------------------*/
class UDPServerEventHandler
{
    public:
        //! UDP Socket Server has received a new event.
        virtual T_error eventSocketAccepted(UDPSocket *aSocket, unsigned short aPort) = 0;

        //! Deconstructor step.
        virtual ~UDPServerEventHandler() {}
};

/*---------------------------------------------------------------------------*
 * Class: UDPServer
 *---------------------------------------------------------------------------*/
/**
 * The UDP Server is a thread that listens on a socket.  When an incoming
 * connection occurs, it notifies an object of the event.
 *
 * <!------------------------------------------------------------------------*/
class UDPServer : public Thread {
    public:
        UDPServer();
        T_error create(
                unsigned short aPort,
                const IPAddress &aMyIP,
                int aIsBroadcast,
                const char *aName,
                UDPServerEventHandler *aEventHandler);
        ~UDPServer();
        T_error Stop(void);
        UDPSocket *iSocket;
        unsigned char isAttached;
     protected:
        unsigned short iPort;
        unsigned char iCanRun;
        unsigned char iDone;
        UDPServerEventHandler *iEventHandler;
        IPAddress iAddress;

        void *run(void);
};

#endif /* _UDPSERVER_H_ */
/*-------------------------------------------------------------------------*
 * End of File:  UDPServer.h
 *-------------------------------------------------------------------------*/
