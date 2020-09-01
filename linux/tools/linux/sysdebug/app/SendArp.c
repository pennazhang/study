/* 
This program sends out one ARP packet with source/target IP 
and Ethernet hardware addresses suuplied by the user. It 
compiles and works on Linux and will probably work on any 
Unix that has SOCK_PACKET. volobuev@t1.chem.umn.edu 
*/ 
#include <netdb.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <stdio.h> 
#include <errno.h> 
#include <sys/ioctl.h> 
#include <net/if.h> 
#include <signal.h> 
#include <netinet/ip.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <arpa/inet.h> 
#include <netinet/ip_icmp.h> 
#include <linux/if_ether.h> 
#include <netpacket/packet.h>
#include <net/if_arp.h>
#include "diag.h"
#include "SendArp.h"
#include "Common.h"

#define ETH_HW_ADDR_LEN 6 
#define IP_ADDR_LEN 4 
#define ARP_FRAME_TYPE 0x0806 
#define ETHER_HW_TYPE 1 
#define IP_PROTO_TYPE 0x0800 
#define OP_ARP_REQUEST 2 
#define OP_ARP_QUEST 1 
#define DEFAULT_DEVICE "eth0" 

struct arp_packet 
{ 
    u_char targ_hw_addr[ETH_HW_ADDR_LEN]; 
    u_char src_hw_addr[ETH_HW_ADDR_LEN]; 
    u_short frame_type; 
    u_short hw_type; 
    u_short prot_type; 
    u_char hw_addr_size; 
    u_char prot_addr_size; 
    u_short op; 
    u_char sndr_hw_addr[ETH_HW_ADDR_LEN]; 
    u_char sndr_ip_addr[IP_ADDR_LEN]; 
    u_char rcpt_hw_addr[ETH_HW_ADDR_LEN]; 
    u_char rcpt_ip_addr[IP_ADDR_LEN]; 
    u_char padding[18]; 
}; 
#if 0

char usage[] = {"send_arp: sends out custom ARP packet. yuri volobuev usage: send_arp src_ip_addr src_hw_addr targ_ip_addr tar_hw_addr number"}; 

void die (char *); 
void get_ip_addr (struct in_addr *, char *); 
void get_hw_addr (char *, char *); 

VOS_STATUS SendArp () 
{ 
    struct in_addr src_in_addr, targ_in_addr; 
    struct sockaddr_in sin;
    struct arp_packet pkt; 
    struct sockaddr sa; 

    struct   ifreq   foo;
    unsigned   char   mac[6];
    unsigned   char   *p   =   NULL;

    int sock; 
    int i,j,number; 

    sock = socket(AF_INET, SOCK_PACKET, htons(ETH_P_ARP)); 
    if (sock < 0) 
    { 
        printf("socket create failed!\n"); 
        return (VOS_ERROR);
    }
    
    number=2; 
    pkt.frame_type = htons(ARP_FRAME_TYPE); 
    pkt.hw_type = htons(ETHER_HW_TYPE); 
    pkt.prot_type = htons(IP_PROTO_TYPE); 
    pkt.hw_addr_size = ETH_HW_ADDR_LEN; 
    pkt.prot_addr_size = IP_ADDR_LEN; 
    pkt.op = htons(OP_ARP_QUEST); 

/*
get_hw_addr(pkt.targ_hw_addr, argv[4]); 
get_hw_addr(pkt.rcpt_hw_addr, argv[4]); 
*/

memset(&foo,   0,   sizeof(struct   ifreq));   
  sprintf(foo.ifr_name,   "%s",   DEFAULT_DEVICE);

if(ioctl(sock, SIOCGIFHWADDR, &foo) < 0) 
  {   
  close(sock);   
  printf("error   :   %s\n",   strerror(errno));   
  return   -3;   
  }   
  
    
  p   =   (unsigned   char   *)&foo.ifr_addr.sa_data;   
  for(i   =   0;   i   <   6;   i++)   {   
  mac[i]   =   p[i];   
  }

printf("The mac addr is:%02x:%02x:%02x:%02x:%02x:%02x\n", mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);

 if (ioctl(sock, SIOCGIFADDR, &foo) < 0)
{
perror("ioctl");
return -1;
}

memcpy(&sin, &foo.ifr_addr, sizeof(sin));

printf("IP ADDR: %s \n", inet_ntoa(sin.sin_addr));

memcpy(pkt.src_hw_addr, mac, ETH_HW_ADDR_LEN);
memcpy(pkt.sndr_hw_addr, mac, ETH_HW_ADDR_LEN);

get_hw_addr(pkt.targ_hw_addr, "FFFFFFFFFFFF");
get_hw_addr(pkt.rcpt_hw_addr, "FFFFFFFFFFFF");

memcpy(pkt.sndr_ip_addr, &sin.sin_addr, IP_ADDR_LEN);
memcpy(pkt.rcpt_ip_addr, &sin.sin_addr, IP_ADDR_LEN);



//close(sock);
//return 0;

//get_hw_addr(pkt.targ_hw_addr, argv[4]);
//get_hw_addr(pkt.rcpt_hw_addr, argv[4]);
//get_hw_addr(pkt.src_hw_addr, argv[2]); 
//get_hw_addr(pkt.sndr_hw_addr, argv[2]); 
//get_ip_addr(&src_in_addr, argv[1]); 
//get_ip_addr(&targ_in_addr, argv[3]); 



//memcpy(pkt.sndr_ip_addr, &src_in_addr, IP_ADDR_LEN); 
//memcpy(pkt.rcpt_ip_addr, &targ_in_addr, IP_ADDR_LEN); 
bzero(pkt.padding,18); 
strcpy(sa.sa_data,DEFAULT_DEVICE); 
for (j=0;j<number;j++) 
{ 
if (sendto(sock,&pkt,sizeof(pkt),0,&sa,sizeof(sa)) < 0) 
{ 
perror("sendto"); 
exit(1); 
} 
} 
exit(0); 
} 
void die (char *str) 
{ 
fprintf(stderr,"%s ",str); 
exit(1); 
} 
void get_ip_addr (struct in_addr *in_addr, char *str) 
{ 
struct hostent *hostp; 
in_addr->s_addr = inet_addr(str); 
if(in_addr->s_addr == -1) 
{ 
if ((hostp = gethostbyname(str))) 
bcopy(hostp->h_addr, in_addr, hostp->h_length); 
else { 
fprintf(stderr, "send_arp: unknown host %s ", str); 
exit(1); 
} 
} 
} 
void get_hw_addr (char *buf, char *str) 
{ 
int i; 
char c, val; 
for(i = 0; i < ETH_HW_ADDR_LEN; i++) 
{ 
if (!(c = tolower(*str++))) 
die("Invalid hardware address"); 
if (isdigit(c)) 
val = c - '0'; 
else if (c >= 'a' && c <= 'f') 
val = c-'a'+10; 
else 
die("Invalid hardware address"); 
*buf = val << 4; 
if (!(c = tolower(*str++))) 
die("Invalid hardware address"); 
if (isdigit(c)) 
val = c - '0'; 
else if (c >= 'a' && c <= 'f') 
val = c-'a'+10; 
else 
die("Invalid hardware address"); 
*buf++ |= val; 
if (*str == ':') 
str++; 
} 
} 

#endif

int g_ARPSocket = -1;
int g_IPSocket = -1;

/* 根据网口名称获取网口的MAC地址 */
VOS_STATUS GetMAC(char *lpszIfName, UINT8 pucMAC[6])
{
    struct   ifreq   foo;
    UINT8 *p;
    int i;
//    int  optval = 1;
    
    if (lpszIfName == NULL)
    {
        printf("Invalid IfName, Please enter a valid if name such as: eth0!\n");
        return (VOS_ERROR);
    }

    memset(pucMAC, 0, 6);
    if (g_ARPSocket < 0)
    {
//        g_ARPSocket = socket(PF_PACKET, SOCK_RAW, htons (ETH_P_ARP));
        g_ARPSocket = socket(AF_INET, SOCK_PACKET, htons(ETH_P_ARP)); ;
        if (g_ARPSocket < 0) 
        { 
            printf("socket create failed!\n"); 
            return (VOS_ERROR);
        }
    }
    
    memset(&foo,   0,   sizeof(struct   ifreq));   
    sprintf(foo.ifr_name,   "%s", lpszIfName);

    if(ioctl(g_ARPSocket, SIOCGIFHWADDR, &foo) < 0) 
    {
        printf("Get Hardware errorerror :   %s\n",   strerror(errno));   
        return (VOS_ERROR);   
    }   
    
    p   =   (unsigned   char   *)&foo.ifr_addr.sa_data;   
    for( i = 0; i < 6; i++)
    {
        pucMAC[i] = p[i];
    }

    printf("MAC Addr of %s is:%02x:%02x:%02x:%02x:%02x:%02x\n", lpszIfName, pucMAC[0],pucMAC[1],pucMAC[2],pucMAC[3],pucMAC[4],pucMAC[5]);
    return (VOS_OK);   
}

/* 根据网口名称获取网口的MAC地址 */
VOS_STATUS GetIP(char *lpszIfName, UINT8 pucIP[4])
{
    struct   ifreq   foo;
    struct sockaddr_in *pSin;
//    int  optval = 1;
    
    if (lpszIfName == NULL)
    {
        printf("Invalid IfName, Please enter a valid if name such as: eth0!\n");
        return (VOS_ERROR);
    }

    memset(pucIP, 0, 4);

    if (g_ARPSocket < 0)
    {
//        g_ARPSocket = socket(PF_PACKET, SOCK_RAW, htons (ETH_P_ARP));
        g_ARPSocket = socket(AF_INET, SOCK_PACKET, htons(ETH_P_ARP)); ;
        if (g_ARPSocket < 0) 
        { 
            printf("socket create failed!\n"); 
            return (VOS_ERROR);
        }
    }
    
    memset(&foo,   0,   sizeof(struct   ifreq));   
    sprintf(foo.ifr_name,   "%s", lpszIfName);

    if(ioctl(g_ARPSocket, SIOCGIFADDR, &foo) < 0) 
    {
        printf("Get IP Address error, error cause:   %s\n",   strerror(errno));   
        return (VOS_ERROR);   
    }   
    pSin = (struct sockaddr_in *)&foo.ifr_addr;
    memcpy(pucIP, (void *)&pSin->sin_addr, IP_ADDR_LEN);
    
    printf("IP Addr of %s is:%d:%d:%d:%d\n", lpszIfName, pucIP[0], pucIP[1], pucIP[2], pucIP[3]);
    return (VOS_OK);   
}

/* 根据网口名称获取网口的子网掩码 */
VOS_STATUS GetNetMask(char *lpszIfName, UINT8 pucMASK[4])
{
    struct   ifreq   foo;
    struct sockaddr_in *pSin;
//    int  optval = 1;
    
    if (lpszIfName == NULL)
    {
        printf("Invalid IfName, Please enter a valid if name such as: eth0!\n");
        return (VOS_ERROR);
    }

    memset(pucMASK, 0, 6);

    if (g_ARPSocket < 0)
    {
//        g_ARPSocket = socket(PF_PACKET, SOCK_RAW, htons (ETH_P_ARP));
        g_ARPSocket = socket(AF_INET, SOCK_PACKET, htons(ETH_P_ARP)); ;
        if (g_ARPSocket < 0) 
        { 
            printf("socket create failed!\n"); 
            return (VOS_ERROR);
        }
    }
    
    memset(&foo,   0,   sizeof(struct   ifreq));   
    sprintf(foo.ifr_name,   "%s", lpszIfName);

    if(ioctl(g_ARPSocket, SIOCGIFNETMASK, &foo) < 0) 
    {
        printf("Get IP Network Mask error, error cause:   %s\n",   strerror(errno));   
        return (VOS_ERROR);   
    }   
    pSin = (struct sockaddr_in *)&foo.ifr_addr;
    memcpy(pucMASK, (void *)&pSin->sin_addr, 4);
    
    printf("IP Addr of %s is:%d:%d:%d:%d\n", lpszIfName, pucMASK[0], pucMASK[1], pucMASK[2], pucMASK[3]);
    return (VOS_OK);   
}


int get_iface_index(int fd, const char* interface_name)
{
    struct ifreq ifr;
    memset(&ifr, 0, sizeof(ifr));
    strcpy (ifr.ifr_name, interface_name);
    if (ioctl(fd, SIOCGIFINDEX, &ifr) == -1)
    {
        return (-1);
    }
    return ifr.ifr_ifindex;
}

/* opCode: 1 - arp 请求， 2 － arp响应  */ 
VOS_STATUS FillArpPacket(UINT8 *pucData, UINT8 *pucFrameDstMAC, UINT8 *pucFrameSrcMac, UINT8 usOpCode, UINT8 *pucSrcMAC, UINT8 *pucSrcIP, UINT8 * pucDstMAC, UINT8 *pucDstIP)
{
    struct arp_packet *pPacket = (struct arp_packet*)pucData;

    memcpy(pPacket->targ_hw_addr, pucFrameDstMAC, 6); /* 目的MAC地址 */    
    memcpy(pPacket->src_hw_addr, pucFrameSrcMac, 6);
    pPacket->frame_type = htons(ETH_P_ARP);
    pPacket->hw_type = htons(ARPHRD_ETHER); 
    pPacket->prot_type = htons(ETH_P_IP); 
    pPacket->hw_addr_size = 6; 
    pPacket->prot_addr_size = 4; 
    pPacket->op = htons(usOpCode);       /*1 - arp 请求， 2 － arp响应  */ 
    memcpy(pPacket->sndr_hw_addr, pucSrcMAC, ETH_HW_ADDR_LEN); /* 发送端MAC地址 */
    memcpy(pPacket->sndr_ip_addr, pucSrcIP, IP_ADDR_LEN); /* 发送端IP 地址 */
    memcpy(pPacket->rcpt_hw_addr, pucDstMAC, ETH_HW_ADDR_LEN); /* 接收端MAC地址 */
    memcpy(pPacket->rcpt_ip_addr, pucDstIP, IP_ADDR_LEN); /* 接收端IP 地址 */

    printf("Packet = %s\n", FormatString(pucData, 64 - 18));

    return (VOS_OK);
}

#if 0
void ArpTest(char *lpszIfName)
{
	char *p1, *p2, *p3, *p4, *p5, *p6, *p7;
	p1 = malloc(0x10);
	ReadFromString("ff ff ff ff ff ff", p1, 6);
	p2 = malloc(0x10);
	ReadFromString ("00 e0 81 59 c3 8f", p2, 6);
	p3 = p2; 
	p4 = malloc(0x10);
	ReadFromString("0a 38 16 0f", p4, 6);
	p5 = p2;
	p6 = p4;

	p7 = malloc(0x80);
	FillArpPacket(p7, p1, p2, 1, p3, p4, p5, p6);
	SendARP(lpszIfName, p7, 60);
}
#endif

#if 0

VOS_STATUS SendARP(char *lpszIfName, UINT8 *pucData, int nDataLen)
{
	struct ifreq if_data;
    int dev_index;
    struct sockaddr_ll from;    
    
    if (g_ARPSocket < 0)
    {
//        g_ARPSocket = socket(PF_PACKET, SOCK_RAW, htons (ETH_P_ARP));
        g_ARPSocket = socket(AF_INET, SOCK_PACKET, htons(ETH_P_RARP));
        if (g_ARPSocket < 0) 
        { 
            printf("socket create failed!\n"); 
            return (VOS_ERROR);
        }
    }

        
    // get the index of dev
    memset(&if_data, 0, sizeof(if_data));
    strcpy (if_data.ifr_name, lpszIfName);
    if (ioctl (g_ARPSocket, SIOCGIFINDEX, &if_data) < 0) 
    {
    	printf("Can not get if index!\n");
        return (VOS_ERROR);
    }
    printf("if index = %d\n", if_data.ifr_ifindex);

    dev_index = if_data.ifr_ifindex;
    bzero(&from, sizeof(from));
    from.sll_ifindex = dev_index;
        
    // send out the arp request
    if( sendto (g_ARPSocket, pucData, nDataLen, 0, (struct sockaddr *)&from, sizeof(from)) < 0)
    {
		printf("Send packet failed!\n");
		return (VOS_ERROR);
    }
    return (VOS_OK);
}

#else

VOS_STATUS SendARP(char *lpszIfName, UINT8 *pucData, int nDataLen)
{
	struct sockaddr sa; 
    
    if (g_ARPSocket < 0)
    {
//        g_ARPSocket = socket(PF_PACKET, SOCK_RAW, htons (ETH_P_ARP));
        g_ARPSocket = socket(AF_INET, SOCK_PACKET, htons(ETH_P_ARP)); ;
        if (g_ARPSocket < 0) 
        { 
            printf("socket create failed!\n"); 
            return (VOS_ERROR);
        }
    }

    bzero  (&sa, sizeof(sa));
    strcpy(sa.sa_data, lpszIfName); 
        
    // send out the arp request
    if( sendto (g_ARPSocket, pucData, nDataLen, 0, &sa, sizeof(sa)) < 0)
    {
		printf("Send packet failed!\n");
		return (VOS_ERROR);
    }
    return (VOS_OK);
}

#endif


