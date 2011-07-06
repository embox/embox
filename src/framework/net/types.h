/**
 * @file
 * @brief Type declarations shared between net framework and each net packet suite.
 *
 * @date 01.07.11
 * @author Dmitry Zubarevich
 */

#ifndef FRAMEWORK_NET_TYPES_H_
#define FRAMEWORK_NET_TYPES_H_

#include <util/location.h>
#include <net/netdevice.h>

/**
 * Each netpack implements this interface.
 */

struct net {
	packet_type_t *netpack;
	/** The corresponding mod. */
	const struct mod *mod;
};

#endif /* FRAMEWORK_NET_TYPES_H_ */
