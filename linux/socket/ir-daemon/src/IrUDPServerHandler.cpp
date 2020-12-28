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
#include <dbus-c++/dbus.h>
#include "IrUDPServerHandler.h"

IrUDPServerHandler::IrUDPServerHandler() : UDPServerEventHandler()
{
}

T_error IrUDPServerHandler::eventSocketAccepted(UDPSocket *aSocket, unsigned short aPort)
{
    T_error ret;
    char buffer0[2048];
    int aNumRead;
    int isBcast = 0;
    IPAddress aPeerIP;
    struct sockaddr *addr;

    do
    {
        memset(buffer0, 0, 2048);
        ret = aSocket->Receive(buffer0, 2048, &aNumRead);
        if (ret == ERROR_NONE)
        {
            if (ERROR_NONE == aSocket->GetPeerIP(aPeerIP))
            {
                addr = aPeerIP.getSocketAddr();
            }
            processUDPpacket(buffer0, aNumRead, aSocket->iSock, addr, aPort);
        }
        else
        {
            //hxDebug( "udp: aSocket->Receive Err=%d!\n", ret);
        }
    } while (ret == ERROR_NONE);
    return ret;
}

void IrUDPServerHandler::processUDPpacket(char * buffer, int pktLen, int sockfd, struct sockaddr* addr, unsigned short aPortNum)
{
    unsigned char *p;
    char pktOut[1500];
    char remoteIP[4];
    bool isStatusCommand = false;

    memset(pktOut, 0, sizeof(pktOut));
    p = (unsigned char*) buffer;

	remoteIP[0]= ((struct sockaddr*)addr)->sa_data[2];
	remoteIP[1]= ((struct sockaddr*)addr)->sa_data[3];
	remoteIP[2]= ((struct sockaddr*)addr)->sa_data[4];
	remoteIP[3]= ((struct sockaddr*)addr)->sa_data[5];
    SVSI_SYSLOG("buffer=%s\n", buffer);

}
