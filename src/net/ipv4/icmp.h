/**
 * \file icmp.h
 *
 * \date 14.03.2009
 * \author sunnya
 */

#ifndef ICMP_H_
#define ICMP_H_

struct _net_packet;
typedef void (*ICMP_CALLBACK)(struct _net_packet* response);

/* Types */
#define ICMP_TYPE_ECHO_REPLY       0  /*<< Echo Reply */
#define ICMP_TYPE_DEST_UNREACHABLE 3  /*<< Destination Unreachable */
#define ICMP_TYPE_SOURCE_QUENCH    4  /*<< Source Quench */
#define ICMP_TYPE_REDIRECT_MESSAGE 5  /*<< Redirect Message */
#define ICMP_TYPE_ECHO_REQ         8  /*<< Echo Request */
#define ICMP_TYPE_ROUTER_ADVERT    9  /*<< Router Advertisement */
#define ICMP_TYPE_ROUTER_SOLICIT   10 /*<< Router Solicitation */
#define ICMP_TYPE_TIME_EXCEEDED    11 /*<< Time Exceeded */
#define ICMP_TYPE_PARAM_PROBLEM    12 /*<< Parameter Problem: Bad IP header */
#define ICMP_TYPE_TIMESTAMP        13 /*<< Timestamp */
#define ICMP_TYPE_TIMESTAMP_REPLY  14 /*<< Timestamp Reply */
#define ICMP_TYPE_INFORM_REQ       15 /*<< Information Request */
#define ICMP_TYPE_INFORM_REPLY     16 /*<< Information Reply */
#define ICMP_TYPE_ADDR_MASK_REQ    17 /*<< Address Mask Request */
#define ICMP_TYPE_ADDR_MASK_REPLY  18 /*<< Address Mask Reply */
#define ICMP_TYPE_TRACEROUTE       30 /*<< Traceroute */

typedef struct _icmphdr {
	unsigned char  type;
	unsigned char  code;
	unsigned short header_check_summ;
	unsigned char  data[0x40];
} __attribute__((packed)) icmphdr;

#define ICMP_HEADER_SIZE	(sizeof(icmphdr))

#endif /* ICMP_H_ */
