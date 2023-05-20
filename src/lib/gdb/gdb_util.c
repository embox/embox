/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 15.05.23
 */
#include <stddef.h>
#include <stdint.h>

#include "gdb_priv.h"

void gdb_pack_str(struct gdb_packet *pkt, const char *str) {
	char ch;

	while ((ch = *str++)) {
		pkt->checksum += ch;
		pkt->buf[pkt->size++] = ch;
	}
}

void gdb_pack_mem(struct gdb_packet *pkt, const void *mem, size_t nbyte) {
	int i;
	uint8_t byte;
	char ch;

	for (i = 0; i < nbyte; i++) {
		byte = *(uint8_t *)(mem + i);
		ch = HEXCHARS[byte >> 4];
		pkt->checksum += ch;
		pkt->buf[pkt->size++] = ch;
		ch = HEXCHARS[byte & 0xf];
		pkt->checksum += ch;
		pkt->buf[pkt->size++] = ch;
	}
}
