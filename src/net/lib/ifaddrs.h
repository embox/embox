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

__END_DECLS

#endif /* IFADDRS_H_ */
