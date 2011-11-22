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

#define SYSTEM_PORT_MAX_NUMBER  (1024)
#define FLAGS_WORD_WIDTH        (32)

static uint32_t udp_ports[SYSTEM_PORT_MAX_NUMBER / FLAGS_WORD_WIDTH]; /* busy flags */
static uint32_t tcp_ports[SYSTEM_PORT_MAX_NUMBER / FLAGS_WORD_WIDTH]; /* busy flags */
static uint32_t sys_ports[SYSTEM_PORT_MAX_NUMBER / FLAGS_WORD_WIDTH]; /* busy flags */

static uint32_t * ports_type[] = {sys_ports, tcp_ports, udp_ports};

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

short get_free_port(void) {
	unsigned short port_type, port_number;

	for(port_type = 0; port_type < 3; port_type++) {
		for(port_number = 0; port_number <= SYSTEM_PORT_MAX_NUMBER; port_number++) {
			if (!socket_port_is_busy(port_number, port_type)) {
				socket_port_lock(port_number, port_type);
				return port_number;
			}
		}
	}

	return 0;
}
