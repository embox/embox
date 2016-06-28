/*
* libcoap_embox_compat.h
*/

#ifndef LIBCOAP_EMBOX_COMPAT_H_
#define LIBCOAP_EMBOX_COMPAT_H_

#define IPV6_JOIN_GROUP 0
#define IPV6_LEAVE_GROUP 1
#define IPV6_UNICAST_HOPS 2
#define IPV6_MULTICAST_LOOP 3
#define IPV6_MULTICAST_HOPS 4

#define IP_MULTICAST_LOOP 5
#define IP_MULTICAST_TTL 6
#define IP_ADD_MEMBERSHIP 7
#define IP_DROP_MEMBERSHIP 8
#define IN_CLASSD(i) (((long)(i) & 0xf0000000) == 0xe0000000)
#define IN_MULTICAST(i) IN_CLASSD(i)

#include <netinet/in.h>

struct ip_mreq {
	struct in_addr imr_multiaddr; /* IP multicast address of group */
	struct in_addr imr_interface; /* local IP address of interface */
};

struct ipv6_mreq
{
	/* IPv6 multicast address of group */
	struct in6_addr ipv6mr_multiaddr;
	/* local interface */
	unsigned int ipv6mr_interface;
};

#endif /* LIBCOAP_EMBOX_COMPAT_H_ */
