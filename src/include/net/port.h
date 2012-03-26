/**
 * @file
 *
 * @date 18.11.11
 * @author Alex Kalmuk
 */
#ifndef NET_PORT_H_
#define NET_PORT_H_

#include <net/socket.h>

#define SYSTEM_PORT_MAX_NUMBER  (1024)
#define FLAGS_WORD_WIDTH        (32)

enum port_type {
	SYS_PORT = 0,
	TCP_PORT = 1,
	UDP_PORT = 2,
};

extern int socket_port_is_busy(short port, unsigned short port_type);
extern int socket_port_lock(short port, unsigned short port_type);
extern int socket_port_unlock(short port, unsigned short port_type);
extern unsigned short socket_get_free_port(unsigned short sock_type);
extern int socket_set_port_type(struct socket *sock);

#endif /* NET_PORT_H_ */
