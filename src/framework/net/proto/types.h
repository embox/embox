/**
 * @file
 * @brief Type declarations shared between net protos framework and each net proto suite.
 *
 * @date 04.07.11
 * @author Dmitry Zubarevich
 */

#ifndef FRAMEWORK_NET_PROTO_TYPES_H_
#define FRAMEWORK_NET_PROTO_TYPES_H_

#include <util/location.h>
#include <net/protocol.h>

/**
 * Each netproto implements this interface.
 */

typedef struct net_proto {
	void (*init)(void);
	net_protocol_t *netproto;
	/** The corresponding mod. */
	const struct mod *mod;
} net_proto_t;

#endif /* FRAMEWORK_NET_PROTO_TYPES_H_ */
