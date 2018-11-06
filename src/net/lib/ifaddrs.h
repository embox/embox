/**
 * @file
 * @brief
 *
 * @date 06.09.13
 * @author Ilia Vaprol
 */

#ifndef IFADDRS_H_
#define IFADDRS_H_

#include <sys/cdefs.h>
#include <sys/socket.h>
#include <netinet/in.h>

__BEGIN_DECLS

struct ifaddrs {
	struct ifaddrs *ifa_next;           /* Next item in list */
	char *ifa_name;                     /* Name of interface */
	unsigned int ifa_flags;             /* Flags from SIOCGIFFLAGS */
	struct sockaddr *ifa_addr;          /* Address of interface */
	struct sockaddr *ifa_netmask;       /* Netmask of interface */
	union {
		struct sockaddr *ifu_broadaddr; /* Bcast address of interface */
		struct sockaddr *ifu_dstaddr;   /* P2P destination address */
	} ifa_ifu;
#define ifa_broadaddr ifa_ifu.ifu_broadaddr
#define ifa_dstaddr   ifa_ifu.ifu_dstaddr
	void *ifa_data;                     /* Address-specific data */
};

/**
 * Get interfaces address information
 */
int getifaddrs(struct ifaddrs **out_ifa);

/**
 * Free data returned by getifaddrs
 */
void freeifaddrs(struct ifaddrs *ifa);

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

__END_DECLS

#endif /* IFADDRS_H_ */
