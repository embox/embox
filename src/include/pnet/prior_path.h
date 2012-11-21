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
#include <arpa/inet.h>
#include <pnet/types.h>

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

extern int pnet_path_set_prior(struct net_node * node, net_prior_t prior);

#endif /* PNET_PRIOR_PATH_H_ */
