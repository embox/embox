/**
 * @file
 * @brief Pending packet fate resolution.
 * @date 13.12.11
 * @author Alexander Kalmuk
 */

#ifndef NET_ARP_QUEUE_H_
#define NET_ARP_QUEUE_H_

#include <net/skbuff.h>

#define MAX_WAIT_TIME 1000

/**
 * Try to send all pending packets with appropriate IP address
 * containing in ARP pack.
 * @param arp_pack - ARP pack received on ARP resolution request
 */
extern void arp_queue_process(sk_buff_t *arp_pack);

/**
 * Add pending packet pack in specific queue to wait for further
 * transmitting. It called only if there are no hw address of destination host
 * in ARP cache.
 * @param - pack - pending pack waiting for transmitting.
 */
extern int arp_queue_add(struct sk_buff *pack);


#endif /* NET_ARP_QUEUE_H_ */
