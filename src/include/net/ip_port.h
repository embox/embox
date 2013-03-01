/**
 * @file
 * @brief
 *
 * @date 29.05.12
 * @author Anton Bondarev
 */

#ifndef NET_IP_PORT_H_
#define NET_IP_PORT_H_

extern int ip_port_lock(int type, unsigned short pnum);
extern int ip_port_unlock(int type, unsigned short pnum);
extern unsigned short ip_port_get_free(int type);

#endif /* NET_IP_PORT_H_ */
