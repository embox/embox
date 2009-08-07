/**
 * \file net_pack.h
 * \date Mar 7, 2009
 * \author anton
 */

#ifndef NET_PACK_H_
#define NET_PACK_H_

/**
 * Allocate net_packet on the pool.
 * @return net_packet
 */
net_packet *net_packet_alloc();

/**
 * Free net_packet on the pool.
 * @param pack
 */
void net_packet_free(net_packet *pack);

/**
 * Copy net_packet on the pool.
 * @param pack
 * @return net_packet
 */
net_packet *net_packet_copy(net_packet *pack);
/*
int net_packet_manager_init();
void *net_pack_manager_alloc();

void *net_pack_manager_lock_pack(void *manager, const unsigned char * data, unsigned short len);
void net_pack_manager_unlock_pack(void *manager, void *net_pack_info);
*/
#endif /* NET_PACK_H_ */
