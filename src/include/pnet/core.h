/**
 * @file
 * @brief Network core interface
 *
 * @date 19.10.2011
 * @author Anton Kozlov
 */

#ifndef PNET_CORE_H_
#define PNET_CORE_H_

#include <pnet/types.h>


#define NET_RX_DFAULT -1
#define NET_TX_DFAULT -2

/* packet go to default handler  */
#define NET_HND_DFAULT     (0x01L << 0)

/* packet had going through all graph */
#define NET_PACK_ACCEPTED           (0x01L << 1)

#define NET_TYPE  0
#define PNET_TYPE 1

extern int pnet_entry(struct pnet_pack *pack);
extern int pnet_process(struct pnet_pack * pack);

extern int pnet_rx_thread_add(struct pnet_pack * pack);

extern net_node_t pnet_get_dev_by_device(struct net_device *dev);

extern int netif_rx(void *pack);

#endif /* PNET_CORE_H_ */
