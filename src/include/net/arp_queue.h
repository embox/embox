/**
 * @file
 * @brief Pending packet fate resolution.
 * @date 13.12.11
 * @author Alexander Kalmuk
 */

#ifndef NET_ARP_QUEUE_H_
#define NET_ARP_QUEUE_H_

#include <net/skbuff.h>

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

/**
 *	Check if socket is wait for fate of pending packs resolution.
 *	@return
 *         - 1 if socket not wait
 *         - 0 in other case
 */
#define is_ready(sock) (sock->sk->arp_queue_info & 0x000000FF)

/**
 *	Check if pending pack from socket was transmitted
 */
#define was_transmitted(sock) ((sock->sk->arp_queue_info & 0x0000FF00) >> 8)

/**
 * Get result of pending pack transmitting
 * @return error number or success of transmitting
 */
#define get_answer(sock) ((sock->sk->arp_queue_info & 0x00FF0000) >> 16)

#endif /* NET_ARP_QUEUE_H_ */
