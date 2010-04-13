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

#define WRONG_DESC_NUMBER 0 /*FIXME It seems 3 protocols in list
 not 2 we defined only 0x1 and 0x11 but see also 0x3 type of protocols*/

#if WRONG_DESC_NUMBER
extern net_protocol_t udp_protocol, icmp_protocol;
#endif

int inet_protocols_init(void) {
#if !WRONG_DESC_NUMBER
	extern net_protocol_t *__ipstack_protos_start, *__ipstack_protos_end;
	net_protocol_t ** p_netproto = &__ipstack_protos_start;

	for (; p_netproto < &__ipstack_protos_end; p_netproto++) {
		if (inet_add_protocol((*p_netproto), (*p_netproto)->type) < 0) {
			LOG_ERROR("inet_protocols_init: Cannot add 0x%X protocol\n",
					(*p_netproto)->type);
		}
		TRACE("added 0x%X\n", (*p_netproto)->type);
	}
#else
	if (inet_add_protocol(&icmp_protocol, IPPROTO_ICMP) < 0) {
		LOG_ERROR("inet_init: Cannot add ICMP protocol\n");
	}

	if (inet_add_protocol(&udp_protocol, IPPROTO_UDP) < 0) {
		LOG_ERROR("inet_init: Cannot add UDP protocol\n");
	}
#endif
	return 0;
}

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
