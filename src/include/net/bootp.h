/**
 * @file
 * @brief Defenitions for Bootstrap Protocol.
 * @details RFC 951
 *
 * @date 28.08.09
 * @author Andrey Baboshin
 */

#ifndef BOOTP_H_
#define BOOTP_H_

#include <net/ip.h>
#include <net/net.h>

//#define HOSTNAME "embox"
//#define DHCP_SUPPORT 1
//#define DNS_SUPPORT 1

#ifdef DHCP_SUPPORT
#define VEND_LEN	(312 + 32)
#define MINPKTSZ	576
#else
#define VEND_LEN	 64
#define MINPKTSZ	300
#endif

/*
	BOOTP support. For details read RFC951 and RFC1395.
*/
struct _bootp_header_t {
	unsigned char    op;                     /* packet op code / message type */
	unsigned char    htype;                  /* hardware addr type */
	unsigned char    hlen;                   /* hardware addr length */
	unsigned char    hops;                   /* gateway hops */
	unsigned int     xid;                    /* transaction ID */
	unsigned short   secs;                   /* seconds since boot began */
	unsigned short   flags;                  /* RFC1532 broadcast, etc. */
	in_addr_t        ciaddr;                 /* client IP address */
	in_addr_t        yiaddr;                 /* 'your' (client) IP address */
	in_addr_t        siaddr;                 /* server IP address */
	in_addr_t        giaddr;                 /* gateway IP address */
	unsigned char    chaddr[16];             /* client hardware address */
	char             sname[64];              /* server host name */
	char             file[128];              /* boot file name */
	unsigned char    options[VEND_LEN];
} __attribute__((packed)) bootp_header_t;

#define __MAX(a,b) ((a)>(b)?(a):(b))
#define BOOTP_HEADER_SIZE __MAX(sizeof(struct _bootp_header_t), MINPKTSZ)

// UDP port numbers, server and client.
#define PORT_BOOTP_SERVER           67
#define PORT_BOOTP_CLIENT           68

// opcodes
#define BOOTPREQUEST             1
#define BOOTPREPLY               2
#define REQYEST_REVERSE          3 /* n.i */
#define REPLY_REVERSE            4 /* n.i */
#define DRARP_Request            5 /* n.i */
#define DRARP_Reply              6 /* n.i */
#define DRARP_Error              7 /* n.i */
#define InARP_Request            8 /* n.i */
#define InARP_Reply              9 /* n.i */
#define ARP_NAK                 10 /* n.i */

// Hardware types from Assigned Numbers RFC.
#define HTYPE_ETHERNET         1
#define HTYPE_EXP_ETHERNET     2
#define HTYPE_AX_25            3
#define HTYPE_PRONET           4
#define HTYPE_CHAOS            5
#define HTYPE_IEEE802          6
#define HTYPE_ARCNET           7
#define HTYPE_HYPERCHANNEL     8
#define HTYPE_LANSTAR          9
#define HTYPE_AUTONET         10
#define HTYPE_LOCALTALK       11
#define HTYPE_LOCALNET        12
#define HTYPE_ULTRALINK       13
#define HTYPE_SMDS            14
#define HTYPE_FRAME_RELAY     15
#define HTYPE_ATM1            16
#define HTYPE_HDLC            17
#define HTYPE_FIBRE_CHANNEL   18
#define HTYPE_ATM2            19
#define HTYPE_SERIAL_LINE     20
#define HTYPE_ATM3            21

// Vendor magic cookie's (v_magic)
#define VM_RFC1048      0x63825363

/*
 * Tag values used to specify what information is being supplied in
 * the vendor (options) data area of the packet.
 * For details read RFC 1048, 1395, 1497, 1533, 1256, 893.
 */
#define TAG_END                 ((unsigned char) 255)
#define TAG_PAD                 ((unsigned char)   0)
#define TAG_SUBNET_MASK         ((unsigned char)   1)
#define TAG_TIME_OFFSET         ((unsigned char)   2)
#define TAG_GATEWAY             ((unsigned char)   3)
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
#define TAG_DHCP_PARM_REQ_LIST  ((unsigned char)  55)
#define TAG_DHCP_TEXT_MESSAGE   ((unsigned char)  56)
#define TAG_DHCP_MAX_MSGSZ      ((unsigned char)  57)
#define TAG_DHCP_RENEWAL_TIME   ((unsigned char)  58)
#define TAG_DHCP_REBIND_TIME    ((unsigned char)  59)
#define TAG_DHCP_CLASSID        ((unsigned char)  60)
#define TAG_DHCP_CLIENTID       ((unsigned char)  61)
#define TAG_FQDN                ((unsigned char)  81)

/*
	DHCP Message Types.
	RFC 2131.
*/
#define DHCP_MESS_TYPE_DISCOVER  ((unsigned char) 1)
#define DHCP_MESS_TYPE_OFFER     ((unsigned char) 2)
#define DHCP_MESS_TYPE_REQUEST   ((unsigned char) 3)
#define DHCP_MESS_TYPE_DECLINE   ((unsigned char) 4)
#define DHCP_MESS_TYPE_ACK       ((unsigned char) 5)
#define DHCP_MESS_TYPE_NAK       ((unsigned char) 6)
#define DHCP_MESS_TYPE_RELEASE   ((unsigned char) 7)

#define RETRY_COUNT 1 /*8*/

#define SHOULD_BE_RANDOM  0x12345555

/**
 * Discovet an IP address with BOOTP protocol.
 * @param ifdev pointer to an ethernet interface
 * @return 1 if bootp success, 0 if there isn't bootp servers near.
 */
int bootp_discover (void* ifdev);

/**
 * Return current bootp info
 */
const struct _bootp_header_t const* get_bootp_info (void);

/**
 * Return ip address obtained from bootp/dhcp
 */
in_addr_t const get_ip_addr (void);

/**
 * Return network mask obtained from bootp/dhcp
 */
in_addr_t const get_ip_mask (void);

/**
 * Return gateway obtained from bootp/dhcp
 */
in_addr_t const get_ip_gate (void);

#endif /* BOOTP_H_ */
