/*
 * libcoap_embox_compat.h
 */

#define IPV6_JOIN_GROUP     0
#define IPV6_LEAVE_GROUP    1
#define IPV6_UNICAST_HOPS   2
#define IPV6_MULTICAST_LOOP 3
#define IPV6_MULTICAST_HOPS 4

#define IP_MULTICAST_LOOP   5
#define IP_MULTICAST_TTL    6
#define IP_ADD_MEMBERSHIP   7
#define IP_DROP_MEMBERSHIP  8

struct ipv6_mreq {
	struct in6_addr ipv6mr_multiaddr;
	unsigned int ipv6mr_interface;
};

