#ifndef LUASOCKET_NETINET_IN_H_
#define LUASOCKET_NETINET_IN_H_

#include_next <netinet/in.h>

#define IP_MULTICAST_LOOP -1
#define IP_ADD_MEMBERSHIP -1
#define IP_DROP_MEMBERSHIP -1
#define IP_MULTICAST_TTL -1
#define IP_MULTICAST_IF -1
#define IPV6_UNICAST_HOPS -1
#define IPV6_MULTICAST_HOPS -1
#define IPV6_MULTICAST_LOOP -1
#define IPV6_ADD_MEMBERSHIP -1
#define IPV6_DROP_MEMBERSHIP -1
#define IPV6_V6ONLY -1

struct ip_mreq {
	struct in_addr imr_multiaddr;
	struct in_addr imr_interface;
};

struct ipv6_mreq {
	struct in6_addr ipv6mr_multiaddr;
	unsigned int ipv6mr_interface;
};

#endif /* LUASOCKET_NETINET_IN_H_ */
