/**
 * @file
 * @brief Global definitions for the INET interface module.
 *
 * @date 03.12.09
 * @author Nikolay Korotky
 */

#ifndef NET_IF_H_
#define NET_IF_H_

#include <sys/socket.h>

/**
 * Maximum length of interface's name (including \0)
 */
#define IFNAMSIZ 6

/**
 * Standard interface flags
 */
#define IFF_UP          0x0001 /* interface is up */
#define IFF_BROADCAST   0x0002 /* broadcast address valid */
#define IFF_DEBUG       0x0004 /* turn on debugging */
#define IFF_LOOPBACK    0x0008 /* is a loopback net */
#define IFF_POINTOPOINT 0x0010 /* interface is has p-p link */
#define IFF_NOTRAILERS  0x0020 /* avoid use of trailers */
#define IFF_RUNNING     0x0040 /* interface RFC2863 OPER_UP */
#define IFF_NOARP       0x0080 /* no ARP protocol */
#define IFF_PROMISC     0x0100 /* receive all packets */
#define IFF_ALLMULTI    0x0200 /* receive all multicast packets */
#define IFF_MULTICAST   0x1000 /* supports multicast */

extern unsigned int if_nametoindex(const char *);

struct ifreq {
	char ifr_name[IFNAMSIZ]; /* Interface name */
	union {
		struct sockaddr ifr_addr;
		struct sockaddr ifr_dstaddr;
		struct sockaddr ifr_broadaddr;
		struct sockaddr ifr_netmask;
		struct sockaddr ifr_hwaddr;

		short ifr_flags;
		int ifr_ifindex;
		int ifr_metric;
		int ifr_mtu;
#if 0
		struct ifmap ifr_map;
#endif
		char ifr_slave[IFNAMSIZ];
		char ifr_newname[IFNAMSIZ];
		char *ifr_data;
	};
};

/**
 * Structure used in SIOCGIFCONF request. Used to retrieve interface
 * configuration for machine (useful for programs which must know all
 * networks accessible)
 */
struct ifconf {
	int ifc_len; /* size of buffer	*/
	union {
		char *ifcu_buf;
		struct ifreq *ifcu_req;
	} ifc_ifcu;
};

#define ifc_buf ifc_ifcu.ifcu_buf /* buffer address	*/
#define ifc_req ifc_ifcu.ifcu_req /* array of structures	*/

#endif /* NET_IF_H_ */
