/**
 * @file
 * @brief A Reverse Address Resolution Protocol
 * @details RFC 903
 *
 * @date 29.09.12
 * @author Ilia Vaprol
 */

#ifndef NET_L3_RARP_H_
#define NET_L3_RARP_H_

#include <net/netdevice.h>

/**
 * Prototypes
 */
struct net_device;

/**
 * RARP Operation Codes
 */
enum rarp_op {
	RARP_OP_REQUEST = 3, /* RARP Request */
	RARP_OP_REPLY   = 4  /* RARP Reply */
};

#endif /* NET_L3_RARP_H_ */
