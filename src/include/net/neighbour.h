/**
 * @file
 * @brief
 *
 * @date 12.08.11
 * @author Ilia Vaprol
 */

#ifndef NET_NEIGHBOUR_H_
#define NET_NEIGHBOUR_H_

#include <net/netdevice.h>
#include <time.h>
#include <util/list.h>

/**
 * Neighbour entity
 */
struct neighbour {
	struct list_link lnk;              /* lnk */
	unsigned short ptype;               /* protocol */
	unsigned char paddr[MAX_ADDR_LEN]; /* protocol address */
	unsigned char plen;                /* protocol address len  */
	const struct net_device *dev;      /* net device */
	int incomplete;                    /* is incomplete */
	unsigned short htype;               /* hw space */
	unsigned char haddr[MAX_ADDR_LEN]; /* hw address */
	unsigned char hlen;                /* hw address len */
	unsigned int flags;                /* flags */
	struct timeval expire;             /* expire */
	struct sk_buff_head w_queue;       /* waiting queue */
};

/**
 * Neighbour flags
 */
#define NEIGHBOUR_FLAG_PERMANENT 0x01 /* Permanent entity */

extern int neighbour_add(unsigned short ptype, const void *paddr,
		unsigned char plen, const struct net_device *dev,
		unsigned short htype, const void *haddr, unsigned char hlen,
		unsigned int flags);

extern int neighbour_set_haddr(unsigned short ptype,
		const void *paddr, const struct net_device *dev,
		const void *haddr);

extern int neighbour_get_haddr(unsigned short ptype,
		const void *paddr, const struct net_device *dev,
		unsigned short htype, unsigned char hlen_max,
		void *out_haddr);

extern int neighbour_get_paddr(unsigned short htype,
		const void *haddr, const struct net_device *dev,
		unsigned short ptype, unsigned char plen_max,
		void *out_paddr);

extern int neighbour_del(unsigned short ptype,
		const void *paddr, const struct net_device *dev);

extern int neighbour_clean(const struct net_device *dev);

typedef int (*neighbour_foreach_ft)(const struct neighbour *nbr,
		void *args);

extern int neighbour_foreach(neighbour_foreach_ft func, void *args);

#endif /* NET_NEIGHBOUR_H_ */
