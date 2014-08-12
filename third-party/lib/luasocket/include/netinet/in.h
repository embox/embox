#ifndef LUASOCKET_NETINET_IN_H_
#define LUASOCKET_NETINET_IN_H_

#include_next <netinet/in.h>

#define IP_MULTICAST_LOOP -1
#define IP_ADD_MEMBERSHIP -1
#define IP_DROP_MEMBERSHIP -1

struct ip_mreq {
	struct in_addr imr_multiaddr;
	struct in_addr imr_interface;
};

#endif /* LUASOCKET_NETINET_IN_H_ */

