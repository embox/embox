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
extern void def_add_packet(sk_buff_t *pack);

#endif /* NET_DEFFPACKET_RESOLVE_H_ */
