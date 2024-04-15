/**
 * @file
 *
 * @date 27.05.09
 * @author Nikolay Korotky
 */

#include <arpa/inet.h>
#include <net/util/macaddr.h>
#include <stdio.h>
#include <string.h>
#include <lib/libds/array.h>

unsigned char *macaddr_scan(const unsigned char *addr, unsigned char *res) {
	char symbol_str[5];
	size_t i, j, cur = 0;
	unsigned int tmp;
	for (i = 0; i < 5; i++) {
		symbol_str[0]='\0';
		for (j = 0; j < ARRAY_SIZE(symbol_str); j++) {
			if (':' == addr[cur + j]) {
				memcpy(symbol_str, &addr[cur], j);
				symbol_str[j] = '\0';
				break;
			}
		}
		if ('\0' == symbol_str[0]) {
			return NULL;
		}
		if (1 != sscanf (symbol_str, "%x", &tmp)) {
			return NULL;
		}
		if (tmp > 0xFF) {
			return NULL;
		}
		res[i] = tmp;
		cur += j + 1;
	}
	strncpy(symbol_str, (char *)&addr[cur], ARRAY_SIZE(symbol_str) - 1);
	if (1 != sscanf (symbol_str, "%x", &tmp)) {
		return NULL;
	}
	if (tmp > 0xFF)
		return NULL;
	res[i] = tmp;
	return res;
}

void macaddr_print(unsigned char *buf, const unsigned char *addr) {
	sprintf((char *) buf, MACADDR_FMT, MACADDR_FMT_ARG(addr));
}
