/**
 * \file net_packet.h
 * \date 07.08.09
 * \author sikmir
 */
#ifndef NET_PACKET_H_
#define NET_PACKET_H_

#define ETHERNET_V2_FRAME_SIZE 1518
//TODO this param must place in autoconf
#define PACK_POOL_SIZE	0x100

/**
 * Allocate net_packet_buff for one ethernet packet on the pool.
 * @return net_packet
 */
extern unsigned char *net_buff_alloc();

/**
 * Free buff of net packet.
 * @param pack
 */
extern void net_packet_free(unsigned char *pack);

#endif /* NET_PACKET_H_ */
