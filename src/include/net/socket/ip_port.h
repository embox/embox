/**
 * @file
 * @brief
 *
 * @date 29.05.12
 * @author Anton Bondarev
 */

#ifndef NET_SOCKET_IP_PORT_H_
#define NET_SOCKET_IP_PORT_H_

#include <stdint.h>

extern int ip_port_lock(int type, uint16_t pnum);
extern int ip_port_unlock(int type, uint16_t pnum);
extern int ip_port_get_free(int type);

#endif /* NET_SOCKET_IP_PORT_H_ */
