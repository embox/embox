/**
 * @file
 * @brief simple ports allocator for dgram/stream sockets
 * @date 29.05.2012
 * @author Anton Bondarev
 * @author Ilia Vaprol
 */

#include <types.h>
#include <errno.h>
#include <net/ip.h>
#include <kernel/thread/sched_lock.h>

#define WORD_LENGTH 32
#define AROUND_QUANTITY	(CONFIG_QUANTITY_PROTOCOL_PORTS + WORD_LENGTH - 1)\
	- (CONFIG_QUANTITY_PROTOCOL_PORTS + WORD_LENGTH - 1) % WORD_LENGTH
#define PORT_NUMBER_OFFSET 1 /* need because port number 1 stored at the zero position */

static uint32_t tcp_ports[AROUND_QUANTITY / WORD_LENGTH] = { 0 };
static uint32_t udp_ports[AROUND_QUANTITY / WORD_LENGTH] = { 0 };

static int get_port_table(int type, uint32_t **pports, uint32_t *size) {
	switch (type) {
	default:
		return -EINVAL;
	case IPPROTO_TCP:
		*pports = tcp_ports;
		*size = sizeof tcp_ports;
		break;
	case IPPROTO_UDP:
		*pports = udp_ports;
		*size = sizeof udp_ports;
		break;
	}

	return ENOERR;
}

int ip_port_lock(int type, uint16_t pnum) {
	int res;
	uint32_t word_n, bit_n, *ports, size;

	res = get_port_table(type, &ports, &size);
	if (res < 0) {
		return res;
	}

	word_n = (pnum - PORT_NUMBER_OFFSET) / WORD_LENGTH;
	bit_n = (pnum - PORT_NUMBER_OFFSET) % WORD_LENGTH;

	if ((pnum < PORT_NUMBER_OFFSET) || (word_n >= size)) {
		return -EINVAL; /* invalid port number */
	}

	sched_lock();

	if (ports[word_n] & (1 << bit_n)) {
		sched_unlock();
		return -EBUSY; /* port is busy */
	}

	ports[word_n] |= (1 << bit_n);

	sched_unlock();

	return ENOERR;
}

int ip_port_unlock(int type, uint16_t pnum) {
	int res;
	uint32_t word_n, bit_n, *ports, size;

	res = get_port_table(type, &ports, &size);
	if (res < 0) {
		return res;
	}

	word_n = (pnum - PORT_NUMBER_OFFSET) / WORD_LENGTH;
	bit_n = (pnum - PORT_NUMBER_OFFSET) % WORD_LENGTH;

	if ((pnum < PORT_NUMBER_OFFSET) || (word_n >= size)) {
		return -EINVAL; /* invalid port number */
	}

	sched_lock();

	if (!(ports[word_n] & (1 << bit_n))) {
		sched_unlock();
		return -EINVAL; /* port not locked */
	}

	ports[word_n] &= ~(1 << bit_n);

	sched_unlock();

	return ENOERR;
}

uint16_t ip_port_get_free(int type) {
	uint16_t try_pnum;
	uint32_t word_n, bit_n, *ports, size;

	if (get_port_table(type, &ports, &size) < 0) {
		return 0;
	}

	for (word_n = 0; word_n < size; ++word_n) {
		for (bit_n = 0; bit_n < WORD_LENGTH; ++bit_n) {
			try_pnum = word_n * WORD_LENGTH + bit_n + PORT_NUMBER_OFFSET;
			if (ip_port_lock(type, try_pnum) == ENOERR) {
				return try_pnum;
			}
		}
	}

	return 0;
}
