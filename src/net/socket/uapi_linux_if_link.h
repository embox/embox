/*
 * uapi_linux_if_link.h
 *
 *  Created on: Oct 4, 2020
 *      Author: anton
 */

#ifndef SRC_NET_SOCKET_UAPI_LINUX_IF_LINK_H_
#define SRC_NET_SOCKET_UAPI_LINUX_IF_LINK_H_
/**
 * @file
 *
 * @brif It's similar like uapi/linux/netlink.h (linux/rtnetlink.h)
 *
 * @date Oct 4, 2020
 * @author Anton Bondarev
 */

/*
 * IFLA_AF_SPEC
 *   Contains nested attributes for address family specific attributes.
 *   Each address family may create a attribute with the address family
 *   number as type and create its own attribute structure in it.
 *
 *   Example:
 *   [IFLA_AF_SPEC] = {
 *       [AF_INET] = {
 *           [IFLA_INET_CONF] = ...,
 *       },
 *       [AF_INET6] = {
 *           [IFLA_INET6_FLAGS] = ...,
 *           [IFLA_INET6_CONF] = ...,
 *       }
 *   }
 */

enum {
	IFLA_UNSPEC,
	IFLA_ADDRESS,
	IFLA_BROADCAST,
	IFLA_IFNAME,
	IFLA_MTU,
	IFLA_LINK,
	IFLA_QDISC,
	IFLA_STATS,
	IFLA_COST,
#define IFLA_COST IFLA_COST
	IFLA_PRIORITY,
#define IFLA_PRIORITY IFLA_PRIORITY
	IFLA_MASTER,
#define IFLA_MASTER IFLA_MASTER
	IFLA_WIRELESS,   /* Wireless Extension event - see wireless.h */
#define IFLA_WIRELESS IFLA_WIRELESS
	IFLA_PROTINFO,   /* Protocol specific information for a link */
#define IFLA_PROTINFO IFLA_PROTINFO
	IFLA_TXQLEN,
#define IFLA_TXQLEN IFLA_TXQLEN
	IFLA_MAP,
#define IFLA_MAP IFLA_MAP
	IFLA_WEIGHT,
#define IFLA_WEIGHT IFLA_WEIGHT
	IFLA_OPERSTATE,
	IFLA_LINKMODE,
	IFLA_LINKINFO,
#define IFLA_LINKINFO IFLA_LINKINFO
	IFLA_NET_NS_PID,
	IFLA_IFALIAS,
	IFLA_NUM_VF,       /* Number of VFs if device is SR-IOV PF */
	IFLA_VFINFO_LIST,
	IFLA_STATS64,
	IFLA_VF_PORTS,
	IFLA_PORT_SELF,
	IFLA_AF_SPEC,
	IFLA_GROUP,        /* Group the device belongs to */
	IFLA_NET_NS_FD,
	IFLA_EXT_MASK,    /* Extended info mask, VFs, etc */
	IFLA_PROMISCUITY, /* Promiscuity count: > 0 means acts PROMISC */
#define IFLA_PROMISCUITY IFLA_PROMISCUITY
	IFLA_NUM_TX_QUEUES,
	IFLA_NUM_RX_QUEUES,
	IFLA_CARRIER,
	IFLA_PHYS_PORT_ID,
	IFLA_CARRIER_CHANGES,
	IFLA_PHYS_SWITCH_ID,
	IFLA_LINK_NETNSID,
	IFLA_PHYS_PORT_NAME,
	IFLA_PROTO_DOWN,
	IFLA_GSO_MAX_SEGS,
	IFLA_GSO_MAX_SIZE,
	__IFLA_MAX
};


#define IFLA_MAX (__IFLA_MAX - 1)



#endif /* SRC_NET_SOCKET_UAPI_LINUX_IF_LINK_H_ */
