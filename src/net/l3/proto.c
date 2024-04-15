/**
 * @file
 * @brief
 *
 * @date 04.07.11
 * @author Dmitry Zubarevich
 * @author Ilia Vaprol
 */

#include <embox/net/proto.h>
#include <stddef.h>
#include <lib/libds/array.h>

ARRAY_SPREAD_DEF(const struct net_proto, __net_proto_registry);

const struct net_proto * net_proto_lookup(unsigned short pack,
		unsigned char type) {
	const struct net_proto *nproto;

	net_proto_foreach(nproto) {
		if ((nproto->pack == pack)
				&& (nproto->type == type)) {
			return nproto;
		}
	}

	return NULL;
}
