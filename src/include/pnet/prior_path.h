/**
 * @file
 *
 * @brief
 *
 * @date 20.10.2011
 * @author Anton Bondarev
 */

#ifndef PNET_PRIOR_PATH_H_
#define PNET_PRIOR_PATH_H_

#include <net/netdevice.h>
#include <net/in.h>

struct pnet_path {
	unsigned char    *own_mac;
	unsigned char    partner_mac[MAX_ADDR_LEN];
	unsigned short   type; /* IP, ARP */
	struct in_addr   sin_addr; /*log addr*/
	short            sin_family; /* UDP, TCP */
	unsigned short   sin_port;
	char             prior_level;
};

extern struct pnet_path *pnet_get_dev_prior(struct net_device *dev);

#endif /* PNET_PRIOR_PATH_H_ */
