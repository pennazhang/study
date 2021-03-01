//####
//# Legal Notice :
//#    Copyright, Harman International, 2020
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
#pragma once

#include "config.h"
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netdb.h>
#include <string>
#include <functional>
#include <netinet/in.h> // IPPROTO_TCP, htons(), htons()
#include <sys/select.h>

const UINT32  BUFFER_SIZE = 0xFFFF;

extern void setErrorInfo(int errorCode, std::string info);

typedef enum 
{
    SOCKET_EVENT_NONE = 0,
    SOCKET_EVENT_NEW_INCOMINT_SOCKET,
    SOCKET_EVENT_TCP_CONNECTION_BROKEN,
    SOCKET_EVENT_RECV_DATA,
    SOCKET_EVENT_UDP_BROKEN,
} SocketEventType;
	
class Socket
{
public:
    explicit Socket(int socketId = -1);
	virtual ~Socket();
	STATUS openSocket(int socketType);
    virtual void closeSocket();
    int getSocketID() { return m_socket; }
    STATUS setNonBlockMode();

//    virtual SocketEventType checkEvent() = 0;

    static std::string ipToString(sockaddr_in addr);
	
protected:
    int m_socket;
};

