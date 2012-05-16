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

/**
 * @brief Result code of node handler
 */
enum net_hnd_results {
	NET_HND_FORWARD, /**< @brief Packet should go further to @link next_node @link */
	NET_HND_FORWARD_DEFAULT, /**< @brief Packet should go to default next node */
	NET_HND_STOP, /**< @brief Packet done processing and should not be
			  processed further */
	NET_HND_STOP_FREE, /**< @brief Packet done processing and should be freed */
};

#define NET_RX_DFAULT -1
#define NET_TX_DFAULT -2

extern int pnet_entry(struct pnet_pack *pack);
extern int pnet_process(struct pnet_pack * pack);

extern int pnet_rx_thread_add(struct pnet_pack * pack);

extern int netif_rx(void *pack);

#endif /* PNET_CORE_H_ */
