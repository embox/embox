/**
 * @file
 * @brief simple port allocation for stream/dgram
 * @date 29.05.2012
 * @author Anton Bondarev
 */
#include <types.h>
#include <errno.h>
#include <net/ip.h>
#include <kernel/thread/sched_lock.h>

#define WORD_LENGTH 32
#define AROUND_QUANTITY	(CONFIG_QUANTITY_PROTOCOL_PORTS + WORD_LENGTH - 1)\
	- (CONFIG_QUANTITY_PROTOCOL_PORTS + WORD_LENGTH - 1) % WORD_LENGTH

static uint32_t tcp_ports[AROUND_QUANTITY / WORD_LENGTH];
static uint32_t udp_ports[AROUND_QUANTITY / WORD_LENGTH];

int ip_port_lock(int type, uint16_t pnum) {
	uint32_t word_n, bit_n, *ports;

	switch (type) {
	default:
		return -EINVAL;
	case IPPROTO_TCP:
		if (pnum > sizeof tcp_ports * WORD_LENGTH) {
			return -EINVAL;
		}
		ports = tcp_ports;
		break;
	case IPPROTO_UDP:
		if (pnum > sizeof udp_ports * WORD_LENGTH) {
			return -EINVAL;
		}
		ports = udp_ports;
		break;
	}

	word_n = pnum / WORD_LENGTH;
	bit_n = pnum % WORD_LENGTH;

	sched_lock();

	if (ports[word_n] & (1 << bit_n)) {
		return -EBUSY; /* port is busy */
	}

	ports[word_n] |= (1 << bit_n);

	sched_unlock();

	return ENOERR;
}

int ip_port_unlock(int type, uint16_t pnum) {
	uint32_t word_n, bit_n, *ports;

	switch (type) {
	default:
		return -EINVAL;
	case IPPROTO_TCP:
		if (pnum > sizeof tcp_ports * WORD_LENGTH) {
			return -EINVAL;
		}
		ports = tcp_ports;
		break;
	case IPPROTO_UDP:
		if (pnum > sizeof udp_ports * WORD_LENGTH) {
			return -EINVAL;
		}
		ports = udp_ports;
		break;
	}

	word_n = pnum / WORD_LENGTH;
	bit_n = pnum % WORD_LENGTH;

	sched_lock();

	ports[word_n] &= ~(1 << bit_n);

	sched_unlock();

	return ENOERR;
}

uint16_t ip_port_get_free(int type) {
	uint32_t word_n, bit_n, word_mask;
	uint32_t *ports, size;

	switch (type) {
	default:
		return -EINVAL;
	case IPPROTO_TCP:
		size = sizeof tcp_ports;
		ports = tcp_ports;
		break;
	case IPPROTO_UDP:
		size = sizeof udp_ports;
		ports = udp_ports;
		break;
	}

	for (word_n = 0; word_n < size; ++word_n) {
		word_mask = 0x1;
		for (bit_n = 0; bit_n < WORD_LENGTH; ++bit_n) {
			if (0 == (word_mask & ports[word_n])) {
				ip_port_lock(type, word_n * WORD_LENGTH + bit_n);
				return 30000 + word_n * WORD_LENGTH + bit_n;
			}
		}
	}

	return 0;
}
