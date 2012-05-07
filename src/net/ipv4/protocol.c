/**
 * @file
 * @brief INET protocol dispatch tables.
 *
 * @date 12.08.09
 * @author Nikolay Korotky
 */

#include <net/protocol.h>
#include <net/in.h>
#include <errno.h>

//TODO inet_protos not use now
const net_protocol_t *inet_protos[MAX_INET_PROTOS];

int inet_add_protocol(net_protocol_t *prot, unsigned char protocol) {
	int hash;

	assert(prot != NULL);

	hash = protocol & (MAX_INET_PROTOS - 1);

	assert(inet_protos[hash] == NULL);
	inet_protos[hash] = prot;

	return ENOERR;
}

int inet_del_protocol(net_protocol_t *prot, unsigned char protocol) {
	int hash;

	assert(prot != NULL);

	hash = protocol & (MAX_INET_PROTOS - 1);

	assert(inet_protos[hash] == prot);
	inet_protos[hash] = NULL;

	return ENOERR;
}
