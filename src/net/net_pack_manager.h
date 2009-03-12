/*
 * net_pack.h
 *
 *  Created on: Mar 7, 2009
 *      Author: anton
 */

#ifndef NET_PACK_H_
#define NET_PACK_H_

int net_packet_manager_init();
void *net_pack_manager_alloc();

void *net_pack_manager_lock_pack(void *manager, const unsigned char * data, unsigned short len);
void net_pack_manager_unlock_pack(void *manager, void *net_pack_info);

#endif /* NET_PACK_H_ */
