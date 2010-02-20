/**
 * @file
 *
 * @date 01.07.09
 * @author Alexey Fomin
 */

#include <crc32.h>

unsigned long count_crc32(unsigned char *addr, unsigned char *end_addr) {
	unsigned long crc_table[256];
	unsigned long crc;
	int i, j;

	for (i = 0; i < 256; i++) {
		crc = i;
		for (j = 0; j < 8; j++)
			crc = crc & 1 ? (crc >> 1) ^ 0xEDB88320UL : crc >> 1;
		crc_table[i] = crc;
	}
	crc = 0xFFFFFFFFUL;
	while (addr < end_addr)
		crc = crc_table[(crc ^ *addr++) & 0xFF] ^ (crc >> 8);

	return crc ^ 0xFFFFFFFFUL;
}
