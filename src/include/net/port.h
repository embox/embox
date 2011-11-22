/**
 * @date 18.11.11
 */
#ifndef _NET_PORT_H
#define _NET_PORT_H

enum port_type {
	SYS_PORT = 0,
	TCP_PORT = 1,
	UDP_PORT = 2,
};

extern int socket_port_is_busy(short port, unsigned short sock_type);
extern int socket_port_lock(short port, unsigned short sock_type);
extern int socket_port_unlock(short port, unsigned short sock_type);
extern short get_free_port(void);

#endif /* _INET_COMMON_H */
