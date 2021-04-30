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

#include <socket.h>
#include <cerrno>
#include <fcntl.h>
#include "utility.h"
#include "typedef.h"


Socket::Socket(int socketId) : m_socket(socketId)
{

}

Socket::~Socket()
{
	closeSocket();
}

/* open a socket */
STATUS Socket::openSocket(int socketType)
{
	if (m_socket >= 0)
    {
		closeSocket();
	}
	
    m_socket = socket(AF_INET, socketType, 0);
	if (m_socket < 0)
    {
        setErrorInfo(errno, "Socket creating error.");
		return (STATUS_ERROR);
    }

    int opt = 1;    
	setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    logInfo("openSocket %d", m_socket);
	
    return (STATUS_OK);
}

/* sockaddr_in to string */
std::string Socket::ipToString(sockaddr_in addr)
{
    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(addr.sin_addr), ip, INET_ADDRSTRLEN);

    return std::string(ip);
}

/* Close the socket */
void Socket::closeSocket()
{
    if(m_socket < 0)
	{
		return;
	}
	
    logInfo("closeSocket %d", m_socket);
    ::close(this->m_socket); 
	m_socket = -1;
}

/* Set the socket to non-block mode */
STATUS Socket::setNonBlockMode()
{
    if (m_socket < 0)
    {
        setErrorInfo(-1, "Invalid socketID");
        return (STATUS_ERROR);
    }

    int flag = fcntl(m_socket, F_GETFL, 0);
    if (flag < 0) 
    {
        setErrorInfo(errno, "fcntl F_GETFL fail");
        return (STATUS_ERROR);
    }
    if (fcntl(m_socket, F_SETFL, flag | O_NONBLOCK) < 0) 
    {
        setErrorInfo(errno, "fcntl F_SETFL fail");
        return (STATUS_ERROR);
    }
    
    return (STATUS_OK);
}

/* We just log the Error info here. Maybe it can be used in future */
void setErrorInfo(int errorCode, std::string info)
{
	logError("ErrorCode = %d: %s\n", errorCode, info.c_str());
}

/* Shutdown can cause quiting a blocking recv function. We still need to close the socket later  */
void Socket::shutdown()
{
    if (m_socket >= 0)
    {
        logInfo("shutdown %d", m_socket);
        ::shutdown(m_socket, SHUT_RDWR);
    }
}
