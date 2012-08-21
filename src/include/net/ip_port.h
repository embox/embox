/**
 * @file
 *
 * @brief
 *
 * @date 29.05.2012
 * @author Anton Bondarev
 */

#ifndef IP_PORT_H_
#define IP_PORT_H_

#include <types.h>

int ip_port_lock(int type, uint16_t pnum);
int ip_port_unlock(int type, uint16_t pnum);
extern uint16_t ip_port_get_free(int type);

#endif /* IP_PORT_H_ */
