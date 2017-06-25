/**
 * @file
 * @brief Stub of module for encrypting packets
 *
 * @date 22.01.14
 * @author Ilia Vaprol
 */

#include <net/l0/net_crypt.h>

struct sk_buff * net_encrypt(struct sk_buff *skb) {
	/* no encryption, give it back */
	return skb;
}

struct sk_buff * net_decrypt(struct sk_buff *skb) {
	/* no encryption, give it back */
	return skb;
}
