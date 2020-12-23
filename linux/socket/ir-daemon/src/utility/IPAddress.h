/*-------------------------------------------------------------------------*
 * File:  IPAddress.h - Copyright (C) 2014 All Rights Reserved - SVSi
 *-------------------------------------------------------------------------*/
#ifndef IPADDRESS_H_
#define IPADDRESS_H_

#include <netinet/in.h>
#include "Error.h"
#include "config.h"

class IPAddress {
    protected:
        int iIsIPv4;
        struct sockaddr_storage iAddress;
        char iString[INET6_ADDRSTRLEN+1];
    public:
        IPAddress();
        IPAddress(const char *aIP, const char *aInterface=0)
            { set(aIP, aInterface); }
        IPAddress(const char *aIP, unsigned int aPort, const char *aInterface=0)
            { set(aIP, aInterface); setPort(aPort); }
        IPAddress(struct sockaddr_in &aAddress)
            { set(aAddress); }
        IPAddress(struct sockaddr_in6 &aAddress)
            { set(aAddress); }
        IPAddress(struct sockaddr_in &aAddress, unsigned int aPort)
            { set(aAddress); setPort(aPort); }
        IPAddress(struct sockaddr_in6 &aAddress, unsigned int aPort)
            { set(aAddress); setPort(aPort); }
        T_error set(const char *aIPv4, const char *aInterface=0);
        T_error set(struct sockaddr_in &aIPv4);
        T_error set(struct sockaddr_in6 &aIPv6);
        void setWithBytes(unsigned char *aBytes);
        const char *asString(void) const { return iString; }
        void getAsBytes(unsigned char *aBytes) const;
        int isIPv4() const { return iIsIPv4; }
        int isIPv6() const { return !iIsIPv4; }
        void setPort(unsigned short aPort);
        unsigned short getPort(void) const;
        void clone(IPAddress& aClone) const;
        struct sockaddr *getSocketAddr(void)
            { return (struct sockaddr *)&iAddress; }
        unsigned int getSocketAddrSize(void) const;
        static int isIPv4String(const char *aString);
        static IPAddress anyIPv4(void);
        static IPAddress anyIPv6(void);
};

#endif /* IPADDRESS_H_ */
/*-------------------------------------------------------------------------*
 * End of File:  IPAddress.h - Copyright (C) 2014 All Rights Reserved - SVSi
 *-------------------------------------------------------------------------*/
