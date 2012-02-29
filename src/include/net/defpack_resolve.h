/**
 * @file
 *
 * @date 13.12.11
 * @author Alex Kalmuk
 */

#ifndef NET_DEFFPACKET_RESOLVE_H_
#define NET_DEFFPACKET_RESOLVE_H_

#include <net/skbuff.h>

#define DEF_PACKET_TTL 1000
#define MAX_PACKET_CNT 0x10

extern void def_process_list(sk_buff_t *arp_pack);
extern int def_add_packet(struct sk_buff *pack);

#define is_ready(sock) (sock->sk->sk_deferred_info & 0x000000FF)
#define was_transmit_deferred(sock) ((sock->sk->sk_deferred_info & 0x0000FF00) >> 8)
#define get_answer_from(sock) ((sock->sk->sk_deferred_info & 0x00FF0000) >> 16)


#endif /* NET_DEFFPACKET_RESOLVE_H_ */
