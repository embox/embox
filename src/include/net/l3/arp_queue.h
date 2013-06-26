/**
 * @file
 * @brief Pending packet fate resolution.
 * @date 13.12.11
 * @author Alexander Kalmuk
 */

#ifndef NET_L3_ARP_QUEUE_H_
#define NET_L3_ARP_QUEUE_H_

#include <net/skbuff.h>

/**
 * Prototypes
 */
struct sk_buff;

/**
 * Try to send all pending packets with appropriate IP address
 * containing in ARP pack.
 * @param arp_skb - ARP pack received on ARP resolution request
 */
extern void arp_queue_process(const struct sk_buff *arp_skb);

/**
 * Add pending packet pack in specific queue to wait for further
 * transmitting. It called only if there are no hw address of destination host
 * in ARP cache.
 * @param skb - pending pack waiting for transmitting.
 */
extern int arp_queue_wait_resolve(struct sk_buff *skb);

#endif /* NET_L3_ARP_QUEUE_H_ */
