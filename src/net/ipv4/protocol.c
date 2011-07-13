/**
 * @file
 * @brief INET protocol dispatch tables.
 *
 * @date 12.08.09
 * @author Nikolay Korotky
 */

#include <net/protocol.h>
#include <net/in.h>
//TODO inet_protos not use now
const net_protocol_t *inet_protos[MAX_INET_PROTOS];

int inet_add_protocol(net_protocol_t *prot, unsigned char protocol) {
	int hash, ret;
	hash = protocol & (MAX_INET_PROTOS - 1);

	if (inet_protos[hash]) {
		ret = -1;
	} else {
		inet_protos[hash] = prot;
		ret = 0;
	}
	return ret;
}

int inet_del_protocol(net_protocol_t *prot, unsigned char protocol) {
	int hash, ret;
	hash = protocol & (MAX_INET_PROTOS - 1);

	if (inet_protos[hash] == prot) {
		inet_protos[hash] = NULL;
		ret = 0;
	} else {
		ret = -1;
	}
	return ret;
}
