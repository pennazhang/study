/*-------------------------------------------------------------------------*
 * File:  IPAddress.cpp - Copyright (C) 2013 All Rights Reserved - SVSi
 *-------------------------------------------------------------------------*/
/**
 *  @addtogroup
 *  @{
 *  @brief
 *
 * Long description goes here
 */
/*-------------------------------------------------------------------------*/
#include <arpa/inet.h>
#include <net/if.h>
#include <stdio.h>
#include <string.h>
#include "IPAddress.h"

/*---------------------------------------------------------------------------*
 * Constants:
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
 * Macros:
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
 * Prototypes:
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
 * Routine: IPAddress::isIPv4String
 *---------------------------------------------------------------------------*/
/**
 * Determine if the given string is an IPv4 address.
 *
 * @return 1 if is an IPv4 address, else 0.
 *
 * <!------------------------------------------------------------------------*/
int IPAddress::isIPv4String(const char *aString)
{
    // Just return true if we find a period anywhere
    // (e.g. www.place.com or 127.0.0.1)
    return (strchr(aString, '.'))?1:0;
}

/*---------------------------------------------------------------------------*
 * Routine: IPv4Address::IPv4Address
 *---------------------------------------------------------------------------*/
/**
 * Create a new IPv4Address object.
 *
 * <!------------------------------------------------------------------------*/
IPAddress::IPAddress()
{
    iString[0] = '\0';
    memset(&iAddress, 0, sizeof(iAddress));
    ((struct sockaddr_in &)iAddress).sin_family = AF_INET;
    iIsIPv4 = 1;
}

/*---------------------------------------------------------------------------*
 * Routine: IPAddress::set
 *---------------------------------------------------------------------------*/
/**
 * Set the IP to the given string representation of the IPv4 or IPv6 address.
 *
 * @param [in] aIP -- String to use.
 *
 * @return ERROR_INVALID_PARAMETER if could not parse, else ERROR_NONE.
 *
 * <!------------------------------------------------------------------------*/
T_error IPAddress::set(const char *aIP, const char *aInterface)
{
    strncpy(iString, aIP, sizeof(iString));
    if (isIPv4String(aIP)) {
        struct sockaddr_in &v4 = (struct sockaddr_in &)iAddress;

        if (inet_pton(AF_INET, iString, &v4.sin_addr) < 1) {
            SVSI_SYSLOG("Invalid IPv4 Address: %s", aIP);
            return ERROR_INVALID_PARAMETER;
        }
        v4.sin_family = AF_INET;
        iIsIPv4 = 1;
    } else {
        struct sockaddr_in6 &v6 = (struct sockaddr_in6 &)iAddress;

        if (inet_pton(AF_INET6, iString, &v6.sin6_addr) < 1) {
            SVSI_SYSLOG("Invalid IPv6 Address being parsed: %s", aIP);
            return ERROR_INVALID_PARAMETER;
        }

        if (aInterface) {
            v6.sin6_scope_id = if_nametoindex(aInterface);
        } else
            v6.sin6_scope_id = 0;

        v6.sin6_family = AF_INET6;
        iIsIPv4 = 0;
    }

    return ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine: IPAddress::set
 *---------------------------------------------------------------------------*/
/**
 * Set the IP to the given ipv4 address structure.
 *
 * @param [in] aIPv4 -- IPv4 address.
 *
 * @return ERROR_INVALID_PARAMETER if could not process, else ERROR_NONE.
 *
 * <!------------------------------------------------------------------------*/
T_error IPAddress::set(struct sockaddr_in &aIPv4)
{
    struct sockaddr_in &v4 = (struct sockaddr_in &)iAddress;

    v4 = aIPv4;
    if (inet_ntop(AF_INET, &v4.sin_addr, iString, sizeof(iString)) == NULL) {
        SVSI_SYSLOG("Invalid IPv4 Address");
        return ERROR_INVALID_PARAMETER;
    }
    v4.sin_family = AF_INET;
    iIsIPv4 = 1;

    return ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine: IPAddress::set
 *---------------------------------------------------------------------------*/
/**
 * Set the IP to the given ipv6 address structure.
 *
 * @param [in] aIPv6 -- IPv6 address.
 *
 * @return ERROR_INVALID_PARAMETER if could not process, else ERROR_NONE.
 *
 * <!------------------------------------------------------------------------*/
T_error IPAddress::set(struct sockaddr_in6 &aIPv6)
{
    struct sockaddr_in6 &v6 = (struct sockaddr_in6 &)iAddress;

    v6 = aIPv6;
    if (inet_ntop(AF_INET6, &v6.sin6_addr, iString, sizeof(iString)) == NULL) {
        SVSI_SYSLOG("Invalid IPv6 Address");
        return ERROR_INVALID_PARAMETER;
    }
    v6.sin6_family = AF_INET6;
    iIsIPv4 = 0;

    return ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine: IPAddress::clone
 *---------------------------------------------------------------------------*/
/**
 * Make a duplicate of this IPv6Address.
 *
 * @return Duplicate or NULL.
 *
 * <!------------------------------------------------------------------------*/
void IPAddress::clone(IPAddress &aClone) const
{
    strncpy(aClone.iString, iString, sizeof(aClone.iString));
    aClone.iIsIPv4 = iIsIPv4;
    aClone.iAddress = iAddress;
}

/*---------------------------------------------------------------------------*
 * Routine: IPAddress::setPort
 *---------------------------------------------------------------------------*/
/**
 * Set the port number of this address.
 *
 * @param [in] aPort -- Port number
 *
 * <!------------------------------------------------------------------------*/
void IPAddress::setPort(unsigned short aPort)
{
    if (iIsIPv4) {
        struct sockaddr_in &v4 = (struct sockaddr_in &)iAddress;
        v4.sin_port = htons(aPort);
    } else {
        struct sockaddr_in6 &v6 = (struct sockaddr_in6 &)iAddress;
        v6.sin6_port = htons(aPort);
    }
}

/*---------------------------------------------------------------------------*
 * Routine: IPAddress::getPort
 *---------------------------------------------------------------------------*/
/**
 * Get the port number of this address.
 *
 * @return Port number.
 *
 * <!------------------------------------------------------------------------*/
unsigned short IPAddress::getPort(void) const
{
    if (iIsIPv4) {
        struct sockaddr_in &v4 = (struct sockaddr_in &)iAddress;
        return ntohs(v4.sin_port);
    } else {
        struct sockaddr_in6 &v6 = (struct sockaddr_in6 &)iAddress;
        return ntohs(v6.sin6_port);
    }
}

void IPAddress::getAsBytes(unsigned char *aBytes) const
{
    int i;

    if (iIsIPv4) {
        struct sockaddr_in &v4 = (struct sockaddr_in &)iAddress;
        for (i=0; i<4; i++)
            aBytes[i] = ((unsigned char *)&v4.sin_addr.s_addr)[i];
    } else {
        struct sockaddr_in6 &v6 = (struct sockaddr_in6 &)iAddress;
        for (i=0; i<16; i++)
            aBytes[i] = v6.sin6_addr.__in6_u.__u6_addr8[i];
    }
}

void IPAddress::setWithBytes(unsigned char *aBytes)
{
    int i;

    if (iIsIPv4) {
        struct sockaddr_in &v4 = (struct sockaddr_in &)iAddress;
        for (i=0; i<4; i++)
            ((unsigned char *)&v4.sin_addr.s_addr)[i] = aBytes[i];
        inet_ntop(AF_INET, &v4.sin_addr, iString, sizeof(iString));
    } else {
        struct sockaddr_in6 &v6 = (struct sockaddr_in6 &)iAddress;
        for (i=0; i<16; i++)
            v6.sin6_addr.__in6_u.__u6_addr8[i] = aBytes[i];
        inet_ntop(AF_INET6, &v6.sin6_addr, iString, sizeof(iString));
    }
}

unsigned int IPAddress::getSocketAddrSize(void) const
{
    if (iIsIPv4) {
        return sizeof(struct sockaddr_in);
    } else {
        return sizeof(struct sockaddr_in6);
    }
}

IPAddress IPAddress::anyIPv4(void)
{
    return IPAddress("0.0.0.0");
}

IPAddress IPAddress::anyIPv6(void)
{
    return IPAddress("::");
}

/** @} */
/*-------------------------------------------------------------------------*
 * End of File:  IPAddress.cpp
 *-------------------------------------------------------------------------*/
