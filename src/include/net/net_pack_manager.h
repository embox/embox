/**
 * @file net_pack_manager.h
 *
 * @date 07.03.2009
 * @author Anton Bondarev
 */
#ifndef NET_PACK_H_
#define NET_PACK_H_

#define ETHERNET_V2_FRAME_SIZE 1520 //TODO: 1518
//TODO this param must place in autoconf
#define PACK_POOL_SIZE  0x100

extern int net_buff_init(void);

/**
 * Allocate net_packet_buff for one ethernet packet on the pool.
 * @return net_packet
 */
extern unsigned char *net_buff_alloc(void);

/**
 * Free buff of net packet.
 * @param buff
 */
extern void net_buff_free(unsigned char *buff);

#endif /* NET_PACK_H_ */
