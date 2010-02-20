/**
 * @file
 *
 * @date 27.05.09
 * @author Nikolay Korotky
 */

#include <lib/bits/byteswap.h>
#include <net/in.h>
#include <netutils.h>
#include <stdio.h>
#include <string.h>

/*FIXME rename this file it contains only net_utils function*/
unsigned char *ipaddr_scan(unsigned char *addr, unsigned char *res) {
	char symbol_str[4];
	int i,j;
	int cur = 0;
	int tmp;
	for(i = 0; i < (sizeof(res) - 1); i ++){
		symbol_str[0]='\0';
		for (j = 0; j < array_len(symbol_str); j ++ ){
			if ('.' == addr[cur + j]){
				memcpy(symbol_str, &addr[cur], j );
				symbol_str[j] = '\0';
				break;
			}
		}
		if ('\0' == symbol_str[0]){
			return NULL;
		}
		if (1 != sscanf (symbol_str, "%d", &tmp)) {
			return NULL;
		}
		if (tmp > 0xFF)
			return NULL;
		res[i] = tmp;
		cur += j + 1;
	}
	strncpy(symbol_str, (char *)&addr[cur], array_len(symbol_str));
	if (1 != sscanf (symbol_str, "%d", &tmp)) {
		return NULL;
	}
	if (tmp > 0xFF)
		return NULL;
	res[i] = tmp;
	return res;
}

unsigned char *macaddr_scan(unsigned char *addr, unsigned char *res) {
	char symbol_str[4];
	int i,j;
	int cur = 0;
	unsigned int tmp;
	for(i = 0; i < 5; i ++){
		symbol_str[0]='\0';
		for (j = 0; j < array_len(symbol_str); j ++ ){
			if (':' == addr[cur + j]){
				memcpy(symbol_str, &addr[cur], j );
				symbol_str[j] = '\0';
				break;
			}
		}
		if ('\0' == symbol_str[0]){
			return NULL;
		}
		if (1 != sscanf (symbol_str, "%x", &tmp)) {
			return NULL;
		}
		if (tmp > 0xFF)
			return NULL;
		res[i] = tmp;
		cur += j + 1;
	}
	strncpy(symbol_str, (char *)&addr[cur], array_len(symbol_str));
	if (1 != sscanf (symbol_str, "%x", &tmp)) {
		return NULL;
	}
	if (tmp > 0xFF)
		return NULL;
	res[i] = tmp;
	return res;
}

void macaddr_print(unsigned char *buf, const unsigned char *addr) {
	sprintf((char *)buf, "%02X:%02X:%02X:%02X:%02X:%02X", addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);
}

int is_addr_from_net(const unsigned char *uip, const unsigned char *nip, unsigned char msk) {
	const unsigned shift = 0xFFFFFFFF;
	struct in_addr addr;
	int userip, netip;
	uint32_t mask, shiftMask;
	inet_aton((char *)uip, &addr);
	userip = addr.s_addr;

	inet_aton((char *)nip, &addr);
	netip = addr.s_addr;

	mask = msk;
	shiftMask = shift << (32 - mask);

	return (__bswap_32(netip) & shiftMask) == (__bswap_32(userip) & shiftMask) ? 0 : -1;
}
