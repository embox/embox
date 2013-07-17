/**
 * @file
 * @brief
 *
 * @date 16.07.13
 * @author Ilia Vaprol
 */

#ifndef NET_L0_NET_ENTRY_
#define NET_L0_NET_ENTRY_

/**
 * function must call from net drivers when packet was received
 * and need transmit one throw protocol's stack
 * @param net_packet *pack struct of network packet
 * @return on success, returns 0, on error, -1 is returned
 */
extern int netif_rx(void *pack);

#endif /* NET_L0_NET_ENTRY_ */
