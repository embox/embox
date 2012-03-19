/**
 * @file
 *
 * @brief
 *
 * @date 07.11.2011
 * @author Anton Bondarev
 */
#include <types.h>
#include <net/port.h>
#include <util/array.h>
#include <net/net.h>
#include <errno.h>
#include <net/inet_sock.h>

static uint32_t udp_ports[SYSTEM_PORT_MAX_NUMBER / FLAGS_WORD_WIDTH]; /* busy flags */
static uint32_t tcp_ports[SYSTEM_PORT_MAX_NUMBER / FLAGS_WORD_WIDTH]; /* busy flags */
static uint32_t sys_ports[SYSTEM_PORT_MAX_NUMBER / FLAGS_WORD_WIDTH]; /* busy flags */

static uint32_t * ports_type[] = { sys_ports, tcp_ports, udp_ports };

int socket_port_is_busy(short port, unsigned short port_type) {
	int word_offset;
	int flag_offset;

	if (port > SYSTEM_PORT_MAX_NUMBER) {
		return 0; //TODO find in common port's list
	}
	word_offset = port / FLAGS_WORD_WIDTH;
	flag_offset = port % FLAGS_WORD_WIDTH;

	return (ports_type[port_type][word_offset] >> flag_offset) & 0x1;
}

int socket_port_lock(short port, unsigned short port_type) {
	int word_offset;
	int flag_offset;

	if (port > SYSTEM_PORT_MAX_NUMBER) {
		return 0; //TODO find in common port's list
	}
	word_offset = port / FLAGS_WORD_WIDTH;
	flag_offset = port % FLAGS_WORD_WIDTH;

	ports_type[port_type][word_offset] |= 1 << flag_offset;

	return 0;
}

int socket_port_unlock(short port, unsigned short port_type) {
	int word_offset;
	int flag_offset;

	if (port > SYSTEM_PORT_MAX_NUMBER) {
		return 0; //TODO find in common port's list
	}
	word_offset = port / FLAGS_WORD_WIDTH;
	flag_offset = port % FLAGS_WORD_WIDTH;

	ports_type[port_type][word_offset] &= ~(1 << flag_offset);

	return 0;
}

short socket_get_free_port(unsigned short sock_type) {
	unsigned short port_number;

	if (sock_type != SOCK_RAW) {
		for (port_number = 0; port_number < SYSTEM_PORT_MAX_NUMBER;
				port_number++) {
			if (!socket_port_is_busy(port_number, sock_type)) {
				socket_port_lock(port_number, sock_type);
				return port_number;
			}
		}
	}

	return 0;
}

int socket_set_port_type(struct socket *sock) {
	struct inet_sock *inet;

	inet = inet_sk(sock->sk);

	if (sock->type != SOCK_RAW) {
		inet->sport_type = sock->type;
	}

	return 0;
}
