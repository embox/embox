/**
 * @file
 * @brief Defenitions for Bootstrap Protocol.
 * @details RFC 951 and RFC 1395
 *
 * @date 28.08.09
 * @author Andrey Baboshin
 * @author Ilia Vaprol
 */

#ifndef NET_LIB_BOOTP_H_
#define NET_LIB_BOOTP_H_

#include <netinet/in.h>
#include <linux/types.h>

/**
 * Bootp operation codes
 */
#define BOOTPREQUEST 1
#define BOOTPREPLY   2

/**
 * Bootp server and client port number
 */
#define BOOTP_SERVER_PORT 67
#define BOOTP_CLIENT_PORT 68

/**
 * Magic code
 */
extern const unsigned char dhcp_magic_cookie[4];

typedef struct bootphdr {
	__u8 op;          /* message type */
	__u8 htype;       /* hw_addr type */
	__u8 hlen;        /* hw_addr length */
	__u8 hops;        /* gateway hops */
	__be32 xid;       /* transaction ID */
	__be16 secs;      /* seconds since boot began */
	__be16 res;       /* reserved */
	in_addr_t ciaddr; /* client IP address */
	in_addr_t yiaddr; /* 'your' (client) IP address */
	in_addr_t siaddr; /* server IP address */
	in_addr_t giaddr; /* gateway IP address */
	__u8 chaddr[16];  /* client hardware address */
	__s8 sname[64];   /* server host name */
	__s8 file[128];   /* boot file name */
	__u8 vend[64];    /* optional vedor-specific data */
} __attribute__((packed)) bootphdr_t;

#define BOOTP_HEADER_SIZE sizeof(struct bootphdr)

/**
 * Tag values used to specify what information is being supplied in
 * the vendor (options) data area of the packet.
 */
#define TAG_END                 ((unsigned char) 255)
#define TAG_PAD                 ((unsigned char)   0)
#define TAG_SUBNET_MASK         ((unsigned char)   1)
#define TAG_GATEWAY             ((unsigned char)   3)
#define TAG_DHCP_PARM_REQ_LIST  ((unsigned char)  55)
#if 0
#define TAG_TIME_OFFSET         ((unsigned char)   2)
#define TAG_TIME_SERVER         ((unsigned char)   4)
#define TAG_NAME_SERVER         ((unsigned char)   5)
#define TAG_DOMAIN_SERVER       ((unsigned char)   6)
#define TAG_LOG_SERVER          ((unsigned char)   7)
#define TAG_COOKIE_SERVER       ((unsigned char)   8)
#define TAG_LPR_SERVER          ((unsigned char)   9)
#define TAG_IMPRESS_SERVER      ((unsigned char)  10)
#define TAG_RLP_SERVER          ((unsigned char)  11)
#define TAG_HOST_NAME           ((unsigned char)  12)
#define TAG_BOOT_SIZE           ((unsigned char)  13)
#define TAG_DUMP_FILE           ((unsigned char)  14)
#define TAG_DOMAIN_NAME         ((unsigned char)  15)
#define TAG_SWAP_SERVER         ((unsigned char)  16)
#define TAG_ROOT_PATH           ((unsigned char)  17)
#define TAG_EXTEN_FILE          ((unsigned char)  18)
#define TAG_IP_FORWARD          ((unsigned char)  19)
#define TAG_IP_NLSR             ((unsigned char)  20)
#define TAG_IP_POLICY_FILTER    ((unsigned char)  21)
#define TAG_IP_MAX_DRS          ((unsigned char)  22)
#define TAG_IP_TTL              ((unsigned char)  23)
#define TAG_IP_MTU_AGE          ((unsigned char)  24)
#define TAG_IP_MTU_PLAT         ((unsigned char)  25)
#define TAG_IP_MTU              ((unsigned char)  26)
#define TAG_IP_SNARL            ((unsigned char)  27)
#define TAG_IP_BROADCAST        ((unsigned char)  28)
#define TAG_IP_SMASKDISC        ((unsigned char)  29)
#define TAG_IP_SMASKSUPP        ((unsigned char)  30)
#define TAG_IP_ROUTERDISC       ((unsigned char)  31)
#define TAG_IP_ROUTER_SOL_ADDR  ((unsigned char)  32)
#define TAG_IP_STATIC_ROUTES    ((unsigned char)  33)
#define TAG_IP_TRAILER_ENC      ((unsigned char)  34)
#define TAG_ARP_TIMEOUT         ((unsigned char)  35)
#define TAG_ETHER_IEEE          ((unsigned char)  36)
#define TAG_IP_TCP_TTL          ((unsigned char)  37)
#define TAG_IP_TCP_KA_INT       ((unsigned char)  38)
#define TAG_IP_TCP_KA_GARBAGE   ((unsigned char)  39)
#define TAG_NIS_DOMAIN          ((unsigned char)  40)
#define TAG_NIS_SERVER          ((unsigned char)  41)
#define TAG_NTP_SERVER          ((unsigned char)  42)
#define TAG_VEND_SPECIFIC       ((unsigned char)  43)
#define TAG_NBNS_SERVER         ((unsigned char)  44)
#define TAG_NBDD_SERVER         ((unsigned char)  45)
#define TAG_NBOTCP_OTPION       ((unsigned char)  46)
#define TAG_NB_SCOPE            ((unsigned char)  47)
#define TAG_XFONT_SERVER        ((unsigned char)  48)
#define TAG_XDISPLAY_SERVER     ((unsigned char)  49)
#define TAG_DHCP_REQ_IP         ((unsigned char)  50)
#define TAG_DHCP_LEASE_TIME     ((unsigned char)  51)
#define TAG_DHCP_OPTOVER        ((unsigned char)  52)
#define TAG_DHCP_MESS_TYPE      ((unsigned char)  53)
#define TAG_DHCP_SERVER_ID      ((unsigned char)  54)
#define TAG_DHCP_TEXT_MESSAGE   ((unsigned char)  56)
#define TAG_DHCP_MAX_MSGSZ      ((unsigned char)  57)
#define TAG_DHCP_RENEWAL_TIME   ((unsigned char)  58)
#define TAG_DHCP_REBIND_TIME    ((unsigned char)  59)
#define TAG_DHCP_CLASSID        ((unsigned char)  60)
#define TAG_DHCP_CLIENTID       ((unsigned char)  61)
#define TAG_FQDN                ((unsigned char)  81)
#endif

extern int bootp_build(struct bootphdr *bph, unsigned char opcode,
		unsigned char hw_type, unsigned char hw_len,
		unsigned char *hw_addr);
extern int bootp_valid(struct bootphdr *bph, unsigned char opcode);

#endif /* NET_LIB_BOOTP_H_ */
