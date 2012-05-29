/**
 * @file
 *
 * @brief
 *
 * @date 29.05.2012
 * @author Anton Bondarev
 */
#include <types.h>
#include <util/array.h>

static uint32_t tcp_ports[0x10];

#define WORD_LENGTH 32

int ip_port_lock(int type, uint16_t pnum) {
	uint32_t word_n = pnum / WORD_LENGTH;
	uint32_t bit_n = pnum % WORD_LENGTH;
	uint32_t *ports = tcp_ports;

	ports[word_n] |= (1 << bit_n);

	return 0;
}

int ip_port_unlock(int type, uint16_t pnum) {
	uint32_t word_n = pnum / WORD_LENGTH;
	uint32_t bit_n = pnum % WORD_LENGTH;
	uint32_t *ports = tcp_ports;

	ports[word_n] &= ~(1 << bit_n);

	return 0;
}

uint16_t ip_port_get_free(int type) {
	int word_n, bit_n;
	uint32_t *ports = tcp_ports;

	for(word_n = 0; word_n < ARRAY_SIZE(tcp_ports); word_n++) {
		uint32_t word_mask = 0x1;
		for(bit_n = 0; bit_n < WORD_LENGTH; ++bit_n) {
			if(0 == (word_mask & ports[word_n])) {
				ip_port_lock(type, word_n * WORD_LENGTH + bit_n);
				return 30000 + word_n * WORD_LENGTH + bit_n;
			}
		}
	}
	return 0;
}
