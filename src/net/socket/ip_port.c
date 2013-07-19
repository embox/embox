/**
 * @file
 * @brief simple ports allocator for dgram/stream sockets

 * @date 29.05.12
 * @author Anton Bondarev
 * @author Ilia Vaprol
 */

#include <assert.h>
#include <errno.h>
#include <framework/mod/options.h>
#include <kernel/sched/sched_lock.h>
#include <limits.h>
#include <netinet/in.h>
#include <stddef.h>
#include <stdint.h>
#include <util/math.h>
#include <util/array.h>

#define PORTS_PER_PROTOCOL OPTION_GET(NUMBER, ports_per_protocol)
#define PORTS_TABLE_SIZE ((PORTS_PER_PROTOCOL + LONG_BIT) / LONG_BIT)

static unsigned long int tcp_ports[PORTS_TABLE_SIZE] = { 0 };
static unsigned long int udp_ports[PORTS_TABLE_SIZE] = { 0 };

static int get_port_table(int type, unsigned long int **pports, size_t *size) {
	switch (type) {
	default:
		return -EINVAL;
	case IPPROTO_TCP:
		*pports = tcp_ports;
		*size = ARRAY_SIZE(tcp_ports);
		break;
	case IPPROTO_UDP:
		*pports = udp_ports;
		*size = ARRAY_SIZE(udp_ports);
		break;
	}

	return 0;
}

int ip_port_lock(int type, uint16_t pnum) {
	int ret, bit_n;
	unsigned long int *ports;
	size_t size, word_n;

	ret = get_port_table(type, &ports, &size);
	if (ret != 0) {
		return ret;
	}

	word_n = pnum / LONG_BIT;
	bit_n = pnum % LONG_BIT;

	if (word_n >= size) {
		return -EINVAL; /* invalid port number */
	}

	sched_lock();
	{
		assert(ports != NULL);
		if (ports[word_n] & (1 << bit_n)) {
			sched_unlock();
			return -EBUSY; /* port is busy */
		}
		ports[word_n] |= 1 << bit_n;
	}
	sched_unlock();

	return 0;
}

int ip_port_unlock(int type, uint16_t pnum) {
	int ret, bit_n;
	unsigned long int *ports;
	size_t size, word_n;

	ret = get_port_table(type, &ports, &size);
	if (ret != 0) {
		return ret;
	}

	word_n = pnum / LONG_BIT;
	bit_n = pnum % LONG_BIT;

	if (word_n >= size) {
		return -EINVAL; /* invalid port number */
	}

	sched_lock();
	{
		assert(ports != NULL);
		if (!(ports[word_n] & (1 << bit_n))) {
			sched_unlock();
			return -EINVAL; /* port is not busy */
		}
		ports[word_n] &= ~(1 << bit_n);
	}
	sched_unlock();

	return 0;
}

static int ip_port_get_free_clamp(int type, uint16_t since,
		uint16_t until) {
	int ret, bit_n;
	unsigned long int *ports;
	size_t size, word_n;
	uint16_t try_pnum;

	ret = get_port_table(type, &ports, &size);
	if (ret != 0) {
		return -EINVAL;
	}

	if (since > until) {
		return -EINVAL;
	}

	word_n = since / LONG_BIT;
	bit_n = since % LONG_BIT;

	if (word_n >= size) {
		return -ENOMEM; /* invalid port number */
	}

	size = min(until / LONG_BIT + 1, size);

	assert(ports != NULL);
	for (; word_n < size; ++word_n) {
		if (~ports[word_n] == 0) continue;
		for (; bit_n < LONG_BIT; ++bit_n) {
			try_pnum = word_n * LONG_BIT + bit_n;
			ret = ip_port_lock(type, try_pnum);
			if (ret == 0) {
				if (try_pnum > until) {
					ret = ip_port_unlock(type, try_pnum);
					assert(ret == 0);
					return -ENOMEM;
				}
				return try_pnum;
			}
		}
		bit_n = 0;
	}

	return -ENOMEM;
}

int ip_port_get_free(int type) {
	static uint16_t next_pnum = IPPORT_RESERVED;
	int port;

	port = ip_port_get_free_clamp(type, next_pnum,
			IPPORT_USERRESERVED - 1);
	if (port < 0) {
		port = ip_port_get_free_clamp(type, IPPORT_RESERVED,
				IPPORT_USERRESERVED - 1);
	}

	if (port >= 0) {
		next_pnum = port + 1;
		if (next_pnum == 0) {
			next_pnum = IPPORT_RESERVED;
		}
	}

	return port;
}
