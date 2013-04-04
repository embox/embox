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
#include <kernel/thread/sched_lock.h>
#include <limits.h>
#include <netinet/in.h>
#include <stddef.h>

#define PORTS_PER_PROTOCOL OPTION_GET(NUMBER, ports_per_protocol)
#define PORTS_TABLE_SIZE ((PORTS_PER_PROTOCOL + LONG_BIT) / LONG_BIT)
#define PORTS_GET_FREE_SINCE OPTION_GET(NUMBER, ports_get_free_since)

static unsigned long int tcp_ports[PORTS_TABLE_SIZE] = { 0 };
static unsigned long int udp_ports[PORTS_TABLE_SIZE] = { 0 };

static int get_port_table(int type, unsigned long int **pports, size_t *size) {
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

	return 0;
}

int ip_port_lock(int type, unsigned short pnum) {
	int ret, bit_n;
	unsigned long int *ports;
	size_t size, word_n;

	if ((pnum <= 0) || (pnum > PORTS_PER_PROTOCOL)) {
		return -EINVAL; /* invalid port number */
	}

	ret = get_port_table(type, &ports, &size);
	if (ret != 0) {
		return ret;
	}

	word_n = (pnum - 1) / LONG_BIT;
	bit_n = (pnum - 1) % LONG_BIT;

	assert(ports != NULL);
	assert(word_n < size);

	sched_lock();
	{
		if (ports[word_n] & (1 << bit_n)) {
			sched_unlock();
			return -EBUSY; /* port is busy */
		}
		ports[word_n] |= 1 << bit_n;
	}
	sched_unlock();

	return 0;
}

int ip_port_unlock(int type, unsigned short pnum) {
	int ret, bit_n;
	unsigned long int *ports;
	size_t size, word_n;

	if ((pnum <= 0) || (pnum > PORTS_PER_PROTOCOL)) {
		return -EINVAL; /* invalid port number */
	}

	ret = get_port_table(type, &ports, &size);
	if (ret != 0) {
		return ret;
	}

	word_n = (pnum - 1) / LONG_BIT;
	bit_n = (pnum - 1) % LONG_BIT;

	assert(ports != NULL);
	assert(word_n < size);

	sched_lock();
	{
		if (!(ports[word_n] & (1 << bit_n))) {
			sched_unlock();
			return -EINVAL; /* port is not busy */
		}
		ports[word_n] &= ~(1 << bit_n);
	}
	sched_unlock();

	return 0;
}

unsigned short ip_port_get_free_since(int type, int since) {
	int ret, bit_n;
	unsigned long int *ports;
	size_t size, word_n;
	unsigned short try_pnum;

	if (since < 1) {
		return 0;
	}

	ret = get_port_table(type, &ports, &size);
	if (ret != 0) {
		return 0;
	}

	word_n = (since - 1) / LONG_BIT;
	bit_n = (since - 1) % LONG_BIT;

	assert(ports != NULL);
	assert(word_n < size);

	for (; word_n < size; ++word_n) {
		if (~ports[word_n] == 0) continue;
		for (; bit_n < LONG_BIT; ++bit_n) {
			try_pnum = word_n * LONG_BIT + bit_n + 1;
			ret = ip_port_lock(type, try_pnum);
			if (ret == 0) {
				return try_pnum;
			}
		}
		bit_n = 0;
	}

	return 0;
}

unsigned short ip_port_get_free(int type) {
	static unsigned short next_pnum = PORTS_GET_FREE_SINCE;
	unsigned short pnum;

	pnum = ip_port_get_free_since(type, next_pnum);
	if (pnum == 0) {
		pnum = ip_port_get_free_since(type, PORTS_GET_FREE_SINCE);
	}

	next_pnum = pnum != 0 ? pnum + 1 : next_pnum;

	return pnum;
}
